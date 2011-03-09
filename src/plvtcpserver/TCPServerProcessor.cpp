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

#include "TCPServerProcessor.h"
#include "Proto.h"

#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <plvgui/ImageConverter.h>
#include <QNetworkInterface>
#include <QImageWriter>

TCPServerProcessor::TCPServerProcessor() :
    m_port(1337),
    m_waiting(false),
    m_convertCvMatToQImage(false)

{
    plv::createDynamicInputPin( "generic pin", this, plv::IInputPin::CONNECTION_OPTIONAL );
    m_cvMatDataTypeId = QMetaType::type("plv::CvMatData");
    m_server = new Server(this);
}

TCPServerProcessor::~TCPServerProcessor()
{
}

bool TCPServerProcessor::init()
{
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i)
    {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }

    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    if( !m_server->listen( QHostAddress::Any, m_port ) )
    {
        QString msg = tr("Unable to start the server: %1.").arg(m_server->errorString());
        setError(PlvPipelineInitError, msg);
        return false;
    }

    // TODO notify user in normal way
    qDebug() << tr("The server is running on\nIP: %1\nport: %2\n")
                .arg(ipAddress).arg(m_port);

    connect(m_server, SIGNAL(onError(PlvErrorType, const QString&)),
            this, SLOT(serverError(PlvErrorType, const QString&)));

    return true;
}

bool TCPServerProcessor::deinit() throw ()
{
    m_server->disconnectAll();
    m_server->close();
    disconnect(m_server, SIGNAL(onError(PlvErrorType, const QString&)),
               this, SLOT(serverError(PlvErrorType, const QString&)));
    return true;
}

bool TCPServerProcessor::start()
{
    return true;
}

bool TCPServerProcessor::stop()
{
    return true;
}

bool TCPServerProcessor::isReadyForProcessing() const
{
    QMutexLocker lock( m_propertyMutex );
    return !m_waiting;
}

void TCPServerProcessor::inputConnectionSet(plv::IInputPin* pin, plv::PinConnection* connection)
{
    QString name = pin->getName();
    QString otherName = connection->fromPin()->getName();

    QString msg = tr("TCPServerProcessor input pin %1 connection set with %2")
                  .arg(name).arg(otherName);

    qDebug() << msg;

    QString newName = QString("%1: %2").arg(pin->getId()).arg(pin->getTypeName());
    pin->setName(newName);
    plv::createDynamicInputPin( "generic pin", this, plv::IInputPin::CONNECTION_OPTIONAL );
}

void TCPServerProcessor::inputConnectionRemoved(plv::IInputPin* pin, plv::PinConnection* connection)
{
    QString name = pin->getName();
    QString otherName = connection->fromPin()->getName();

    QString msg = tr("TCPServerProcessor input pin %1 connection removed with %2")
                  .arg(name).arg(otherName);

    qDebug() << msg;
    removeInputPin(pin->getId());
}

void TCPServerProcessor::outputConnectionAdded(plv::IOutputPin* pin, plv::PinConnection* connection)
{
    QString name = pin->getName();
    QString otherName = connection->toPin()->getName();

    QString msg = tr("TCPServerProcessor output pin %1 connection removed with %2")
                  .arg(name).arg(otherName);

    qDebug() << msg;
}

void TCPServerProcessor::outputConnectionRemoved(plv::IOutputPin* pin, plv::PinConnection* connection)
{
    QString name = pin->getName();
    QString otherName = connection->toPin()->getName();

    QString msg = tr("TCPServerProcessor output pin %1 connection removed with %2")
                  .arg(name).arg(otherName);

    qDebug() << msg;
}

bool TCPServerProcessor::process()
{
    QVariantList frameData;

    PipelineElement::InputPinMap::iterator itr = m_inputPins.begin();
    for( ; itr != m_inputPins.end(); ++itr )
    {
        plv::IInputPin* pin = itr->second.getPtr();
        if( pin->isConnected() )
        {

            if( pin->getTypeId() == m_cvMatDataTypeId &&
                m_convertCvMatToQImage )
            {
                QVariant v;
                pin->getVariant(v);
                plv::CvMatData data = v.value<plv::CvMatData>();
                QImage img = plvgui::ImageConverter::cvMatToQImageNoThrow(data);
                QVariant v2;
                v2.setValue(img);
                frameData.append(v2);
            }
            else
            {
                QVariant v;
                pin->getVariant(v);
                frameData.append(v);
            }
        }
    }
    quint32 frameNumber = (quint32)getProcessingSerial();

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    // write the header
    out << (quint32)0; // reserved for number of bytes later
    out << (quint32)PROTO_FRAME;    // msg type
    out << (quint32)frameNumber;    // frame number / serial number
    out << (quint32)frameData.size(); // number of arguments

    // write all data to the stream
    foreach( const QVariant& v, frameData )
    {
#if 0
        // we do not want QImage to convert
        // to the default PNG because that
        // is way too slow
        if( v.type() == QVariant::Image )
        {
            out << QVariant::UserType;

            if(out.version() >= QDataStream::Qt_4_2)
                out << qint8(v.isNull());

            QByteArray a("plv::QImageWrapper");
            out << a;

            if( !v.isValid() )
            {
                out << QString();
            }
            else
            {
                QImage img = v.value<QImage>();
                QImageWriter writer(out.device(), "png");
                if(!writer.write(img))
                {
                    QString err = writer.errorString();
                    qWarning() << "Failed to write image to stream because " << err;
                }
            }
        }
        else
        {
            out << v;
        }
#else
        out << v;
#endif
    }

    // calculate size of total data and write it as first 4 bytes
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));

    m_server->sendFrame(frameNumber, block);
    return true;
}

void TCPServerProcessor::acceptConfigurationRequest()
{
}

/** propery methods */
int TCPServerProcessor::getPort() const
{
    QMutexLocker lock(m_propertyMutex);
    return m_port;
}

void TCPServerProcessor::setPort(int port, bool doEmit )
{
    QMutexLocker lock(m_propertyMutex);
    m_port = port;
    lock.unlock();
    if( doEmit ) emit( portChanged(port) );
}

bool TCPServerProcessor::getConvertCvMatDataToQImage() const
{
    QMutexLocker lock(m_propertyMutex);
    return m_convertCvMatToQImage;
}

void TCPServerProcessor::setConvertCvMatDataToQImage(bool doConvert, bool doEmit)
{
    QMutexLocker lock(m_propertyMutex);
    m_convertCvMatToQImage = doConvert;
    lock.unlock();
    if( doEmit ) emit convertCvMatDataToQImageChanged(doConvert);
}

int TCPServerProcessor::getMaxFramesInFlight() const
{
    Q_ASSERT( m_server != 0 );
    return m_server->getMaxFramesInFlight();
}

int TCPServerProcessor::getMaxFramesInQueue() const
{
    Q_ASSERT( m_server != 0 );
    return m_server->getMaxFramesInQueue();
}

bool TCPServerProcessor::getLossless() const
{
    Q_ASSERT( m_server != 0 );
    return m_server->getLossless();
}

void TCPServerProcessor::setMaxFramesInQueue(int max)
{
    QMutexLocker lock(m_propertyMutex);
    if( max > 0 )
    {
        m_server->setMaxFramesInQueue(max);
        lock.unlock();
        emit maxFramesInQueueChanged(max);
    }
}

void TCPServerProcessor::setMaxFramesInFlight(int max)
{
    QMutexLocker lock(m_propertyMutex);
    if( max > 0 )
    {
        m_server->setMaxFramesInFlight(max);
        lock.unlock();
        emit maxFramesInFlightChanged(max);
    }
}

void TCPServerProcessor::setLossless(bool lossless)
{
    QMutexLocker lock(m_propertyMutex);
    m_server->setLossless(lossless);
    lock.unlock();
    emit losslessChanged(lossless);
}

void TCPServerProcessor::stalled(ServerConnection* connection)
{
    QMutexLocker lock( m_propertyMutex );
    qDebug() << "TCPServerProcessor: a connection stalled";
    m_waiting = true;
}

// TODO: do a connection check, we cannot be lossless with
// more than one connection now
void TCPServerProcessor::unstalled(ServerConnection* connection)
{
    QMutexLocker lock( m_propertyMutex );
    qDebug() << "TCPServerProcessor: a connection unstalled";
    m_waiting = false;
}

void TCPServerProcessor::serverError(PlvErrorType type, const QString& msg)
{
    // propagate to pipeline element error handling
    setError(type,msg);
    emit onError(type,this);
}

