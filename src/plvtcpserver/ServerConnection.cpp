#include "ServerConnection.h"

#include <QtNetwork>
#include <assert.h>

ServerConnection::ServerConnection(int socketDescriptor) :
    QObject(0),
    m_tcpSocket( new QTcpSocket() ),
    m_socketDescriptor(socketDescriptor),
    m_errorString("No error string set")
{
}

ServerConnection::~ServerConnection()
{
    assert( m_tcpSocket == 0 );
}

void ServerConnection::writeBytes( const QByteArray& bytes )
{
    assert( m_tcpSocket != 0 );
    if( m_tcpSocket->state() != QAbstractSocket::ConnectedState )
    {
        emit( error( "Not connected" ) );
        return;
    }

    // signal if we have to wait for data to be sent
    // this will tell the server to throttle the pipeline
    bool waiting = false;
    if( m_tcpSocket->bytesToWrite() > 1024*1024 )
    {
        emit( waitingOnClient(this,true) );
        waiting = true;
    }

    while( m_tcpSocket->bytesToWrite() > 1024*1024 )
    {
        m_tcpSocket->waitForBytesWritten(10);
    }

    // tell server to continue
    if( waiting ) emit( waitingOnClient(this,false));

    if( m_tcpSocket->write(bytes) == -1 )
    {
        emit error( m_tcpSocket->errorString() );
    }
}

void ServerConnection::bytesWritten( qint64 bytes )
{
    //qDebug() << "Written " << bytes << " bytes.";
}

void ServerConnection::start()
{
    assert( m_tcpSocket != 0 );

    // connect signals of tcp socket
    connect( m_tcpSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));

    if( !m_tcpSocket->setSocketDescriptor(m_socketDescriptor) )
    {
        emit error( m_tcpSocket->errorString() );
        return;
    }
}

void ServerConnection::stop()
{
    m_tcpSocket->disconnectFromHost();
    m_tcpSocket->waitForDisconnected();

    delete m_tcpSocket;
    m_tcpSocket = 0;

    emit( finished() );
}
