/**
  * Copyright (C)2011 by Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvtcpserver plugin of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

#include "TCPClientProducer.h"
#include <QtNetwork>
#include "Proto.h"

TCPClientProducer::TCPClientProducer() :
    m_tcpSocket(new QTcpSocket(this)),
    m_ipAddress(QHostAddress(QHostAddress::LocalHost).toString()),
    m_port(1337),
    m_blockSize(0),
    m_networkSession(0),
    m_configured( true )
{
    m_intOut      = plv::createOutputPin<int>("int", this);
    m_stringOut   = plv::createOutputPin<QString>("QString", this);
    m_doubleOut   = plv::createOutputPin<double>("double", this);
    m_cvScalarOut = plv::createOutputPin< cv::Scalar >("cv::Scalar", this);
    m_imageOut    = plv::createCvMatDataOutputPin("CvMatData", this);
}

TCPClientProducer::~TCPClientProducer()
{
}

bool TCPClientProducer::init()
{
    assert( m_networkSession == 0 );
    assert( m_tcpSocket != 0 );

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired)
    {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered)
        {
            config = manager.defaultConfiguration();
        }

        m_networkSession = new QNetworkSession(config);
        connect(m_networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

        qDebug() << "Opening network session.";
        m_networkSession->open();
    }
    return true;
}

bool TCPClientProducer::deinit() throw ()
{
    if( m_networkSession )
    {
        m_networkSession->close();
        delete m_networkSession;
        m_networkSession = 0;
    }

    return true;
}

bool TCPClientProducer::start()
{
    // connect to server

    // if we did not find one, use IPv4 localhost
    if( m_ipAddress.isEmpty() )
    {
        m_ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
        qWarning() << "No valid IP address given, using localhost";
        m_ipAddress = QHostAddress::LocalHost;
    }

    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(connected()));

    QHostAddress address( m_ipAddress );

    // Try to optimize the socket for low latency.
    // For a QTcpSocket this would set the TCP_NODELAY option
    // and disable Nagle's algorithm.
    m_tcpSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

    qDebug() << "Connecting to " << address.toString() << ":" << m_port;

    // if this fails, error is automatically called
    // by signal slots connection
    m_tcpSocket->connectToHost( address, m_port );

    int timeout = 5*1000;
    if(!m_tcpSocket->waitForConnected(timeout))
    {
        displayError(m_tcpSocket->error(), false);
        return false;
    }
    return true;
}

bool TCPClientProducer::stop()
{
    // disconnect from server
    qDebug() << "Disconnecting ... ";
    m_tcpSocket->disconnectFromHost();

    int timeout = 5*1000;
    if(!m_tcpSocket->waitForDisconnected(timeout))
    {
        displayError(m_tcpSocket->error(), false);
        return false;
    }
    return true;
}

bool TCPClientProducer::readyToProduce() const
{
    // check if there is data available
    return !m_frameList.isEmpty();
}

bool TCPClientProducer::produce()
{
    // get data from queue
    QMutexLocker lock( &m_frameListMutex );
    QVariantList frame = m_frameList.takeFirst();
    lock.unlock();

    // do primitive dynamic matching of pins with framedata
    QSet<QString> taken;
    foreach( const QVariant& v, frame )
    {
        for( OutputPinMap::const_iterator itr = m_outputPins.begin();
            itr != m_outputPins.end(); ++itr )
        {
            plv::RefPtr<plv::IOutputPin> out = itr->second;
            if( !taken.contains(out->getName()) )
            {
                if( out->getTypeId() == v.userType() )
                {
                    taken.insert( out->getName() );
                    unsigned int serial = this->getProcessingSerial();
                    out->putVariant(serial, v);
                }
            }
        }
    }
    return true;
}

void TCPClientProducer::readData()
{
    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

    if( m_blockSize == 0 )
    {
        if( m_tcpSocket->bytesAvailable() < (int)sizeof(quint32) )
            return;
        in >> m_blockSize;
    }

    qint64 bytesAvailable = m_tcpSocket->bytesAvailable();
    if( bytesAvailable < m_blockSize )
        return;

    qDebug() << "TCPClientProducer::readData() ready";
    if( !m_configured )
    {
        // send configuration request
    }
    else
    {
        bool data = true;
        //int frames = 0;
        while( data )
        {
            quint32 opcode;
            in >> opcode;

            switch( opcode )
            {
            case PROTO_INIT:
                //m_configured = parseConfig(in);
                break;
            case PROTO_FRAME:
                if( m_configured )
                {
                    parseFrame(in);
                }
                else
                {
                    qDebug() << "Frame received while not configured.";
                }
                break;
            default:
                // error, invalid opcode
                qWarning() << "Invalid opcode, skipping message";

                // skip this message
                in.skipRawData( m_blockSize - sizeof(quint32) );
            }

            if( m_tcpSocket->bytesAvailable() >= (int)sizeof(quint32) )
            {
                in >> m_blockSize;
            }
            else
            {
                m_blockSize = 0;
                data = false;
            }

            if( m_tcpSocket->bytesAvailable() < m_blockSize )
                data = false;

//            if( ++frames > 10 )
//            {
//                qint64 bytes = m_tcpSocket->bytesAvailable();
//                qDebug() << "bytes: " << bytes;
//                data = false;
//            }
        }
    }
}

void TCPClientProducer::ackFrame(quint32 frameNumber)
{
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    // write the header
    out << (quint32)(2*sizeof(quint32)); // size of message excluding 4 bytes for size
    out << (quint32)PROTO_ACK;
    out << (quint32)frameNumber;

    if( m_tcpSocket->write(bytes) == -1 )
    {
        setError( PlvNonFatalError, m_tcpSocket->errorString() );
    }
}

//bool TCPClientProducer::parseConfig( QDataStream& in )
//{
//    qint32 numargs;
//    in >> numargs;

//    if( numargs < 1 )
//    {
//        // no arguments sent with header, skip this frame
//        return false;
//    }

//    qDebug() << "Parsing argument list of CONFIG message";
//    for( int i=0; i < numargs; ++i )
//    {
//        QVariant::Type type;
//        in >> type;

//        QString name = QMetaType::typeName(type);
//        if( name != 0 )
//        {
//            qDebug() << "Found type " << type << " with name " << name;
//            m_types.append( type );
//        }
//        else
//        {
//            qDebug() << "Invalid type found with value " << type;
//            return false;
//        }
//    }
//    return true;
//}

bool TCPClientProducer::parseFrame( QDataStream& in )
{
    QVariantList frame;

    quint32 serial;
    quint32 numargs;

    in >> serial;
    in >> numargs;

    if( numargs < 1 )
    {
        // no arguments sent with header, skip this frame
        qWarning() << "Invalid frame received. Skipping.";

        // skip this message
        in.skipRawData( m_blockSize - 2*sizeof(quint32) );
        return false;
    }

    // parse all arguments using QVariant
    for( unsigned int i=0; i < numargs; ++i )
    {
        QVariant v;
        in >> v;

        if( !v.isValid() )
        {
            qDebug() << "Frame is not correct, invalid variant.";
            return false;
        }

        //qDebug() << "Variant parsed: " << v.type();
        //if( v.type() != m_types[i] )
        //{
        //    // frame is not according to specifications
        //    qDebug() << "Frame is not correct, variant of unexpected type.";
        //    return false;
        //}
        frame.append(v);
    }

    // send ack to the server for flow control
    ackFrame(serial);

    QMutexLocker lock( &m_frameListMutex );
    m_frameList.append(frame);

    return true;
}

void TCPClientProducer::sessionOpened()
{
    // Save the used configuration
    QNetworkConfiguration config = m_networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = m_networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();
}

void TCPClientProducer::connected()
{
    qDebug() << "TCPClientProducer connected";
}

void TCPClientProducer::disconnected()
{
    qDebug() << "TCPClientProducer disconnected";
    setError( PlvPipelineRuntimeError, tr("The remote host closed the connection."));
}

void TCPClientProducer::displayError(QAbstractSocket::SocketError socketError, bool signal)
{
    QString msg;
    PlvErrorType type = PlvNoError;

    switch (socketError)
    {
    case QAbstractSocket::RemoteHostClosedError:
        type = PlvPipelineRuntimeError;
        msg = tr("The remote host closed the connection.");
        break;
    case QAbstractSocket::HostNotFoundError:
        type = PlvPipelineRuntimeError;
        msg = tr("The host was not found. Please check the "
                 "host name and port settings.");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        type = PlvPipelineRuntimeError;
        msg = tr("The connection was refused by the peer. "
                 "Make sure the Parlevision server is running, "
                 "and check that the host name and port "
                 "settings are correct.");
        break;
    default:
       type = PlvPipelineRuntimeError;
       msg = tr("The following error occurred: %1.").arg(m_tcpSocket->errorString());
    }
    setError(type,msg);
    if(signal) emit onError(type,this);
}

/** propery methods */
int TCPClientProducer::getPort() const
{
    QMutexLocker lock(m_propertyMutex);
    return m_port;
}

void TCPClientProducer::setPort(int port, bool doEmit )
{
    QMutexLocker lock(m_propertyMutex);
    m_port = port;
    if( doEmit ) emit( portChanged(port) );
}

/** propery methods */
QString TCPClientProducer::getServerIP() const
{
    QMutexLocker lock(m_propertyMutex);
    return m_ipAddress;
}

void TCPClientProducer::setServerIP(const QString& ip, bool doEmit )
{
    QMutexLocker lock(m_propertyMutex);
    m_ipAddress = ip;
    if( doEmit ) emit( serverIPChanged(ip) );
}


