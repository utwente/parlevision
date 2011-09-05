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

#include "ServerConnection.h"
#include "Proto.h"

#include <QtNetwork>
#include <assert.h>

ServerConnection::ServerConnection(int socketDescriptor, bool lossless, int maxFrameQueue, int maxFramesInFlight) :
    QObject(0),
    m_tcpSocket(0),
    m_socketDescriptor(socketDescriptor),
    m_errorString("no error"),
    m_waiting(false),
    m_lossless(lossless),
    m_maxFramesInQueue(maxFrameQueue),
    m_maxFramesInFlight(maxFramesInFlight),
    m_blockSize(0)
{
    connect( this, SIGNAL( scheduleSend()), this, SLOT(sendData()), Qt::QueuedConnection );
}

ServerConnection::~ServerConnection()
{
    if( m_tcpSocket != 0 )
    {
        delete m_tcpSocket;
		m_tcpSocket = 0;
    }
}

void ServerConnection::queueFrame(quint32 frameNumber, QByteArray frameData)
{
    if( m_tcpSocket == 0 )
        return;

    Frame f(frameNumber, frameData);

    // append to queue of this connection
    m_frameQueue.append(f);

    if( m_frameNumbersInFlight.size() < m_maxFramesInFlight )
    {
        sendData();
    }

    // we start dropping frames when the queue is at m_maxFramesInQueue
    // unless we are set to lossless, then we
    // give the server a signal to throttle
    
	// check if they are too old
	/*if(m_frameQueue.size() > m_maxFramesInQueue)
    {
		while( m_frameQueue.size() > 0 )
		{
			const Frame& f = m_frameQueue.first();
			
			if( f.time.elapsed() < 5000 )
				break;

			qWarning() << tr("Frame %1 timed out after %2 msecs.").arg(f.serial).arg(f.time.elapsed());
			m_frameQueue.removeFirst();
		}
	}*/

	if(m_frameQueue.size() > m_maxFramesInQueue)
	{
        if( m_lossless )
        {
            if( !m_waiting )
            {
                m_waiting = true;
                emit waitingOnClient(this, m_waiting);
            }
        }
        else
        {
            // drop the oldest frame
            Frame f = m_frameQueue.takeFirst();
            qDebug() << "Connection " << m_tcpSocket->peerAddress().toString() << ":"
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

    if( m_waiting )
        return;

//    if( m_tcpSocket->bytesToWrite() > MAX_BYTES_TO_WRITE )
//    {
//        m_waiting = true;
//        emit waitingOnClient(this, m_waiting);
//        return;
//    }

    bool overflow = false;
    while(!( m_frameNumbersInFlight.size() >= m_maxFramesInFlight ||
             m_frameQueue.isEmpty() ||
             overflow ))
    {
        Frame& f = m_frameQueue.first();
        if( f.sent == 0 )
        {
            qint64 bytesSent = m_tcpSocket->write(f.bytes);
            if( bytesSent < f.bytes.size() )
            {
                if( bytesSent != -1 )
                    f.sent = bytesSent;
                else
                    overflow = true;
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
                if( bytesSent != -1 )
                    f.sent = f.sent + bytesSent;
                else
                    overflow = true;
            }
            else
            {
                m_frameNumbersInFlight.append(f.serial);
                m_frameQueue.removeFirst();
            }
        }

        if( overflow )
        {
            m_waiting = true;
            emit waitingOnClient(this, m_waiting);
        }
    }
}

void ServerConnection::ackFrame(quint32 serial)
{
    if( m_frameNumbersInFlight.isEmpty() )
    {
        qWarning() << "Ignoring ack received for frame " << serial << " while no ack was expected.";
        return;
    }
    quint32 expectedSerial = m_frameNumbersInFlight.first();
    if( expectedSerial == serial )
    {
        m_frameNumbersInFlight.removeFirst();
    }
    else
    {
        qWarning() << "Ignoring ack received for frame " << serial << " while expected frame " << expectedSerial << ".";
    }

    if( m_lossless && m_waiting )
    {
        m_waiting = false;
        emit waitingOnClient(this, m_waiting);
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
            data = false;
        }

        if( m_tcpSocket->bytesAvailable() < m_blockSize )
            data = false;
    }
}

void ServerConnection::bytesWritten(qint64 bytes)
{
	Q_UNUSED(bytes)
//    if( m_waiting && m_tcpSocket->bytesToWrite() < MAX_BYTES_TO_WRITE )
//    {
//        m_waiting = false;
//        emit waitingOnClient(this, m_waiting);
//    }
}

void ServerConnection::start()
{
    assert( m_tcpSocket == 0 );

    m_tcpSocket = new QTcpSocket();

    // Try to optimize the socket for low latency.
    // For a QTcpSocket this would set the TCP_NODELAY option
    // and disable Nagle's algorithm.
    //m_tcpSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

    // connect signals of tcp socket
    connect( m_tcpSocket, SIGNAL(connected()), this, SLOT(connected()) );
    connect( m_tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()) );
    connect( m_tcpSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    connect( m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()) );
    connect( m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error(QAbstractSocket::SocketError)));

    if( !m_tcpSocket->setSocketDescriptor(m_socketDescriptor) )
    {
        emit onError( PlvPipelineRuntimeError, m_tcpSocket->errorString() );
        emit finished();
        return;
    }
}

void ServerConnection::stop()
{
    m_tcpSocket->disconnectFromHost();
}

void ServerConnection::connected()
{
    qDebug() << "Server socket connected with address "
             << m_tcpSocket->peerAddress().toString()
             << " and port " << m_tcpSocket->peerPort();
}

void ServerConnection::disconnected()
{
    // schedule for deletion according to documentation of disconnected:
    // Warning: If you need to delete the sender() of
    // this signal in a slot connected to it, use the deleteLater() function.
    m_tcpSocket->disconnect();
    m_tcpSocket->deleteLater();
    m_tcpSocket = 0;
    emit finished();
}

void ServerConnection::setMaxFrameQueue(int max)
{
    m_maxFramesInQueue = max;
}

void ServerConnection::setMaxFramesInFlight(int max)
{
    m_maxFramesInFlight = max;
}

void ServerConnection::setLossless(bool lossless)
{
    m_lossless = lossless;
}

void ServerConnection::error(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
    case QAbstractSocket::RemoteHostClosedError:
        emit onError(PlvNonFatalError, tr("The remote host closed the connection."));
        break;
    default:
        emit onError(PlvNonFatalError, tr("The following error occurred: %1.")
                     .arg(m_tcpSocket->errorString()));
    }
}
