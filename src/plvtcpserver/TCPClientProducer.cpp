#include "TCPClientProducer.h"
#include <QtNetwork>
#include "Proto.h"

TCPClientProducer::TCPClientProducer() :
    m_tcpSocket( new QTcpSocket(this) ),
    m_ipAddress( QHostAddress(QHostAddress::LocalHost).toString() ),
    m_port( 1337 )
{
    m_intOut    = plv::createOutputPin<int>("int", this);
    m_stringOut = plv::createOutputPin<QString>("QString", this);
    m_doubleOut = plv::createOutputPin<double>("double", this);
    m_cvScalarOut = plv::createOutputPin< cv::Scalar >("cv::Scalar", this);

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

        m_networkSession = new QNetworkSession(config, this);
        connect(m_networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

        qDebug() << "Opening network session.";
        m_networkSession->open();
    }
}

TCPClientProducer::~TCPClientProducer()
{
}

void TCPClientProducer::init()
{
}

void TCPClientProducer::deinit() throw ()
{
}

void TCPClientProducer::start()
{
    // connect to server

    // if we did not find one, use IPv4 localhost
    if ( m_ipAddress.isEmpty() )
    {

        //m_ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
        error( "No ip address given" );
        return;
    }

    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

    QHostAddress address( m_ipAddress );

    // if this fails, error is automatically called
    // by signal slots connection
    m_tcpSocket->connectToHost( address, m_port );
}

void TCPClientProducer::stop()
{
    // disconnect from server
    m_tcpSocket->disconnect();
}

bool TCPClientProducer::readyToProduce() const
{
    // check if there is data available

    return false;
}

void TCPClientProducer::produce()
{
    // get data from queue
}

void TCPClientProducer::readData()
{
    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_4_7);

    if( m_blockSize == 0 )
    {
        if( m_tcpSocket->bytesAvailable() < (int)sizeof(quint32) )
            return;
        in >> m_blockSize;
    }

    if( m_tcpSocket->bytesAvailable() < m_blockSize )
        return;

    if( !m_configured )
    {
        // send configuration request
    }
    else
    {
        quint32 opcode;
        in >> opcode;

        switch( opcode )
        {
        case PROTO_INIT:
            m_configured = parseConfig(in);
            break;
        case PROTO_FRAME:
            if( m_configured )
                parseFrame(in);
            else
            {
                qDebug() << "Frame received while not configured.";
            }
            break;
        default:
            // error, invalid opcode
            qDebug() << "Invalid opcode, skipping message";

            // skip this message
            in.skipRawData( m_blockSize - sizeof(quint32) );
        }
    }
}

bool TCPClientProducer::parseConfig( QDataStream& in )
{

    qint32 numargs;

    in >> numargs;

    if( numargs < 1 )
    {
        // no arguments sent with header, skip this frame
        return false;
    }

    qDebug() << "Parsing argument list of CONFIG message";
    for( int i=0; i < numargs; ++i )
    {
        QVariant::Type type;
        in >> type;

        QString name = QMetaType::typeName(type);
        if( name != 0 )
        {
            qDebug() << "Found type " << type << " with name " << name;
            m_types.append( type );
        }
        else
        {
            qDebug() << "Invalid type found with value " << type;
            return false;
        }
    }
    return true;
}

bool TCPClientProducer::parseFrame( QDataStream& in )
{
    QVariantList frame;

    qint32 numargs;
    quint32 serial;

    in >> numargs;
    in >> serial;

    if( numargs < 1 )
    {
        // no arguments sent with header, skip this frame
        return false;
    }

    // parse all arguments using QVariant
    qDebug() << "TCPClientProducer received input";
    for( int i=0; i < numargs; ++i )
    {
        QVariant v;
        in >> v;

        if( !v.isValid() )
        {
            qDebug() << "Frame is not correct, invalid variant.";
            return false;
        }

        qDebug() << "Variant parsed: " << v.type();
        if( v.type() != m_types[i] )
        {
            // frame is not according to specifications
            qDebug() << "Frame is not correct, variant of unexpected type.";
            return false;
        }
        frame.append(v);
    }
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

void TCPClientProducer::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        error(tr("The host was not found. Please check the "
                 "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        error( tr("The connection was refused by the peer. "
                  "Make sure the fortune server is running, "
                  "and check that the host name and port "
                  "settings are correct."));
        break;
    default:
       error( tr("The following error occurred: %1.")
              .arg(m_tcpSocket->errorString()));
    }
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


