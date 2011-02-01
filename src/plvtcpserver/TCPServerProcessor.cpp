#include "TCPServerProcessor.h"
#include "Proto.h"

#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <QNetworkInterface>

using namespace plv;

TCPServerProcessor::TCPServerProcessor() : m_port(1337), m_waiting(false)
{
    m_server = new Server(this);
    this->connect(this,
                  SIGNAL(sendData(const QByteArray&)),
                  m_server,
                  SLOT(sendData(const QByteArray&)) );

    m_inputPinCvMatData = createCvMatDataInputPin( "image", this, IInputPin::CONNECTION_OPTIONAL );
    m_inputPinDouble    = createInputPin<double>(  "double_test", this, IInputPin::CONNECTION_OPTIONAL );
    m_inputPinCvScalar  = createInputPin<cv::Scalar>( "cv::Scalar", this, IInputPin::CONNECTION_OPTIONAL );
}

TCPServerProcessor::~TCPServerProcessor()
{
}

void TCPServerProcessor::init()
{
    if( !m_server->listen( QHostAddress::LocalHost, m_port ) )
    {
        error(PlvFatal, tr("Unable to start the server: %1.")
               .arg(m_server->errorString()));
        return;
    }

    connect(m_server, SIGNAL( error(QString) ),
            this, SLOT( error(QString) ));

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    qDebug() << (tr("The server is running on\n\nIP: %1\nport: %2\n\n")
                 .arg(ipAddress).arg(m_server->serverPort()));
}

void TCPServerProcessor::deinit() throw ()
{
    m_server->disconnectAll();
    m_server->close();
}

void TCPServerProcessor::start()
{
}

void TCPServerProcessor::stop()
{
}

void TCPServerProcessor::process()
{
    QVariantList frameData;

    PipelineElement::InputPinMap::iterator itr = m_inputPins.begin();
    for( ; itr != m_inputPins.end(); ++itr )
    {
        if( itr->second->isConnected() )
        {
            QVariant v;
            itr->second->getVariant(v);
            frameData.append(v);
        }
    }
    sendFrame(frameData);
}

void TCPServerProcessor::sendFrame( const QVariantList& frameData )
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);

    // number of arguments in this frame
    unsigned int numargs = frameData.size();
    unsigned int frameNumber = getProcessingSerial();

    // write the header
    out << (quint32)PROTO_FRAME;
    out << (quint32)0; // reserved for number of bytes later
    out << (quint32)frameNumber;
    out << (quint32)numargs;

    // write all data to the stream
    foreach( const QVariant& v, frameData )
    {
        out << v;
    }

    // calculate size of total data and write it as first 4 bytes
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));

    // send it
    emit( sendData( block ) );
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
    if( doEmit ) emit( portChanged(port) );
}

void TCPServerProcessor::stalled()
{
    QMutexLocker lock( m_propertyMutex );
    m_waiting = true;
}

void TCPServerProcessor::unstalled()
{
    QMutexLocker lock( m_propertyMutex );
    m_waiting = false;
}

bool TCPServerProcessor::isReadyForProcessing() const
{
    QMutexLocker lock( m_propertyMutex );
    return !m_waiting;
}
