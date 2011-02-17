#include "TCPServerProcessor.h"
#include "Proto.h"

#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <QNetworkInterface>

using namespace plv;

TCPServerProcessor::TCPServerProcessor() : m_port(1337), m_waiting(false)
{
    m_server = new Server(this);
    connect(this, SIGNAL( sendFrame(quint32,QVariantList)),
            m_server, SLOT( sendFrame(quint32,QVariantList)) );

    m_inputPinCvMatData = createCvMatDataInputPin( "image", this, IInputPin::CONNECTION_OPTIONAL );
    m_inputPinDouble    = createInputPin<double>(  "double_test", this, IInputPin::CONNECTION_OPTIONAL );
    m_inputPinCvScalar  = createInputPin<cv::Scalar>( "cv::Scalar", this, IInputPin::CONNECTION_OPTIONAL );
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
        setError(PlvFatalError, msg);
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

bool TCPServerProcessor::process()
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
    quint32 frameNumber = (quint32)getProcessingSerial();
    emit sendFrame(frameNumber, frameData);
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

void TCPServerProcessor::serverError(PlvErrorType type, const QString& msg)
{
    // propagate to pipeline element error handling
    setError(type,msg);
    emit onError(type,this);
}

