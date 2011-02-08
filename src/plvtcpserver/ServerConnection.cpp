#include "ServerConnection.h"
#include "Proto.h"

#include <QtNetwork>
#include <assert.h>

#define MAX_FRAME_QUEUE 10
#define MAX_BYTES_TO_WRITE 512*1024

ServerConnection::ServerConnection(int socketDescriptor) :
    QObject(0),
    m_tcpSocket(0),
    m_socketDescriptor(socketDescriptor),
    m_errorString("No error string set"),
    m_waiting(false),
    m_lossless(false),
    m_blockSize(0)
{
    connect( this, SIGNAL( scheduleSend()), this, SLOT(sendData()), Qt::QueuedConnection );
}

ServerConnection::~ServerConnection()
{
    if( m_tcpSocket != 0 )
    {
        delete m_tcpSocket;
    }
}

void ServerConnection::queueFrame(quint32 frameNumber, QByteArray frameData)
{
    Frame f(frameNumber, frameData);

    // append to queue of this connection
    //QMutexLocker lock(&m_scMutex);
    m_frameQueue.append(f);

    // we start dropping frames when the queue is at MAX_FRAME_QUEUE
    // unless we are set to lossless, then we
    // give the server a signal to throttle
    if(m_frameQueue.size() >= MAX_FRAME_QUEUE)
    {
        if( m_lossless )
        {
            m_waiting = true;
            emit(waitingOnClient(this, m_waiting));
        }
        else
        {
            // drop the oldest frame
            Frame f = m_frameQueue.takeFirst();
            qDebug() << "Connection " << m_tcpSocket->peerAddress() << ":"
                     << m_tcpSocket->peerPort() << " dropped frame " << f.serial;
        }
    }
    emit scheduleSend();
}

void ServerConnection::sendData()
{
    if( m_tcpSocket == 0 ||
        m_tcpSocket->state() != QAbstractSocket::ConnectedState )
    {
        return;
    }

    if( m_tcpSocket->state() != QAbstractSocket::ConnectedState )
    {
        return;
    }

    if( m_waiting )
        return;

    if( m_tcpSocket->bytesToWrite() > MAX_BYTES_TO_WRITE )
    {
        m_waiting = true;
        emit(waitingOnClient(this, m_waiting));
        return;
    }

    //QMutexLocker lock(&m_scMutex);
    if( m_frameQueue.isEmpty() )
        return;

    Frame& f = m_frameQueue.first();
    if( f.sent == 0 )
    {
        qint64 bytesSent = m_tcpSocket->write(f.bytes);
        if( bytesSent < f.bytes.size() )
        {
            if( bytesSent == -1 )
                emit errorOccurred(PlvDebug, m_tcpSocket->errorString());
            else
                f.sent = bytesSent;
        }
        else
        {
            m_frameNumbersInFlight.append(f.serial);
            m_frameQueue.removeFirst();
        }
    }
    else
    {
        const char* ptr = f.bytes.data();
        ptr += f.sent;
        qint64 len = f.bytes.size() - f.sent;

        qint64 bytesSent = m_tcpSocket->write(ptr, len);

        if( bytesSent < len )
        {
            if( bytesSent == -1 )
                emit errorOccurred(PlvDebug, m_tcpSocket->errorString());
            else
                f.sent = f.sent + bytesSent;
        }
        else
        {
            m_frameNumbersInFlight.append(f.serial);
            m_frameQueue.removeFirst();
        }
    }
}

void ServerConnection::ackFrame(quint32 serial)
{
    //QMutexLocker lock(&m_scMutex);
    quint32 expectedSerial = m_frameNumbersInFlight.takeFirst();
    if( expectedSerial == serial )
    {
        if( m_waiting )
        {
            m_waiting = false;
            emit(waitingOnClient(this, m_waiting));
        }
    }
    else
    {
        qWarning() << "Ack received for frame " << serial << " while expected frame " << expectedSerial << ".";
    }
}

void ServerConnection::readyRead()
{
    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

    if( m_blockSize == 0 )
    {
        if( m_tcpSocket->bytesAvailable() < (int)sizeof(quint32) )
            return;
        in >> m_blockSize;
    }

    if( m_tcpSocket->bytesAvailable() < m_blockSize )
        return;

    bool data = true;
    int frames = 0;
    while( data )
    {
        quint32 opcode;
        in >> opcode;

        switch( opcode )
        {
        case PROTO_ACK:
            quint32 serial;
            in >> serial;
            ackFrame(serial);
            break;

        case PROTO_INIT:
        case PROTO_FRAME:
        default:
            // error, invalid opcode
            qWarning() << "Invalid or unexpected opcode, skipping message";

            // skip this message
            in.skipRawData(m_blockSize - sizeof(quint32));
        }

        if(m_tcpSocket->bytesAvailable() >= (int)sizeof(quint32))
        {
            in >> m_blockSize;
        }
        else
        {
            m_blockSize = 0;
        }

        if( m_tcpSocket->bytesAvailable() < m_blockSize )
            data = false;

        if( ++frames > 10 )
            data = false;
    }
}

void ServerConnection::bytesWritten(qint64 bytes)
{
    if( m_waiting && m_tcpSocket->bytesToWrite() < MAX_BYTES_TO_WRITE )
    {
        m_waiting = false;
        emit(waitingOnClient(this, m_waiting));
    }
}

void ServerConnection::start()
{
    assert( m_tcpSocket == 0 );

    m_tcpSocket = new QTcpSocket();

    // Try to optimize the socket for low latency.
    // For a QTcpSocket this would set the TCP_NODELAY option
    // and disable Nagle's algorithm.
    m_tcpSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

    // connect signals of tcp socket
    connect( m_tcpSocket, SIGNAL(connected()), this, SLOT(connected()) );
    connect( m_tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()) );
    connect( m_tcpSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    connect( m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()) );
    connect( m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error(QAbstractSocket::SocketError)));

    if( !m_tcpSocket->setSocketDescriptor(m_socketDescriptor) )
    {
        emit errorOccurred( PlvWarning, m_tcpSocket->errorString() );
        emit finished();
        return;
    }
}

void ServerConnection::stop()
{
    //QMutexLocker lock( &m_scMutex );
    m_tcpSocket->disconnectFromHost();
}

void ServerConnection::connected()
{
    qDebug() << "Server socket connected with address " <<
            m_tcpSocket->peerAddress() << " and port " <<
            m_tcpSocket->peerPort();
}

void ServerConnection::disconnected()
{
    // schedule for deletion according to documentation of disconnected:
    // Warning: If you need to delete the sender() of
    // this signal in a slot connected to it, use the deleteLater() function.
    m_tcpSocket->deleteLater();

    //QMutexLocker lock( &m_scMutex );
    m_tcpSocket = 0;
    //lock.unlock();
    emit finished();
}

void ServerConnection::error(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
    case QAbstractSocket::RemoteHostClosedError:
        emit errorOccurred( PlvNotify, tr("The remote host closed the connection.") );
        break;
    default:
        emit errorOccurred( PlvNotify, tr("The following error occurred: %1.").arg(m_tcpSocket->errorString()) );
    }
}
