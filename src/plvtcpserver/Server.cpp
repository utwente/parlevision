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

#include "Server.h"
#include "ServerConnection.h"
#include "Proto.h"
#include <QImage>
#include <QImageWriter>

Server::Server(QObject *parent) : QTcpServer(parent),
    m_lossless(false),
    m_maxFramesInQueue(1),
    m_maxFramesInFlight(1)
{
    connect( this, SIGNAL( stalled(ServerConnection*) ), parent, SLOT( stalled(ServerConnection*) ) );
    connect( this, SIGNAL( unstalled(ServerConnection*) ), parent, SLOT( unstalled(ServerConnection*) ) );
}

void Server::incomingConnection(int socketDescriptor)
{
    qDebug() << "Server: incoming connection";

    QMutexLocker lock(&m_serverPropMutex);
    ServerConnection* connection = new ServerConnection(socketDescriptor, m_lossless, m_maxFramesInQueue, m_maxFramesInFlight);
    lock.unlock();

    // let errors go through the error reporting signal of this class
    connect( this, SIGNAL( onError(PlvErrorType,QString)),
             connection, SIGNAL(onError(PlvErrorType,QString)));

    // move the connection to its own thread
    QThreadEx* connectionThread = new QThreadEx();
    connection->moveToThread(connectionThread);

    // when the connection is done it is scheduled for deletion
    connect( connection, SIGNAL(finished()),
             connection, SLOT(deleteLater()));

    // when the connection is done, it stops its thread
    connect( connection, SIGNAL(finished()),
             connectionThread, SLOT(quit()));

    // start the connection when its thread is started
    connect( connectionThread, SIGNAL(started()),
             connection, SLOT(start()));

    // inform server when this serverthead is waiting on the client
    connect( connection, SIGNAL( waitingOnClient(ServerConnection*,bool)),
             this, SLOT( serverThreadStalled(ServerConnection*,bool)) );

    connect( parent(), SIGNAL( maxFrameQueueChanged(int) ),
             connection, SLOT( setMaxFrameQueue(int) ) );

    connect( parent(), SIGNAL( maxFramesInFlightChanged(int) ),
             connection, SLOT( setMaxFramesInFlight(int)) );

    connect( parent(), SIGNAL( losslessChanged(bool) ),
             connection, SLOT( setLossless(bool)) );

    // stop this connection when stopAllConnections is called
    connect( this, SIGNAL(stopAllConnections()), connection, SLOT(stop()));

    // start the connection thread and its event loop
    connectionThread->start();

    // connection receives all broadcasts and sends it to its connection
    connect( this, SIGNAL( broadcastFrame(quint32,QByteArray)),
             connection, SLOT( queueFrame(quint32,QByteArray)));
}

void Server::setMaxFramesInQueue(int max)
{
    Q_ASSERT_X(max >= 1, "Server::setMaxFramesInQueue", "max frames in queue should be larger than 0");
    QMutexLocker lock(&m_serverPropMutex);
    m_maxFramesInQueue = max;
}

void Server::setMaxFramesInFlight(int max)
{
    Q_ASSERT_X(max >= 1, "Server::setMaxFramesInFlight", "max frames in flight should be larger than 0");
    QMutexLocker lock(&m_serverPropMutex);
    m_maxFramesInFlight = max;
}

void Server::setLossless(bool lossless)
{
    QMutexLocker lock(&m_serverPropMutex);
    m_lossless = lossless;
}

int Server::getMaxFramesInFlight() const
{
    QMutexLocker lock(&m_serverPropMutex);
    return m_maxFramesInFlight;
}

int Server::getMaxFramesInQueue() const
{
    QMutexLocker lock(&m_serverPropMutex);
    return m_maxFramesInQueue;
}

bool Server::getLossless() const
{
    QMutexLocker lock(&m_serverPropMutex);
    return m_lossless;
}

void Server::sendFrame(quint32 frameNumber, const QByteArray& data)
{
    emit broadcastFrame(frameNumber, data);
}

void Server::disconnectAll()
{
    emit stopAllConnections();
}

void Server::serverThreadStalled( ServerConnection* connection, bool isStalled )
{
    if( isStalled )
        emit stalled(connection);
    else
        emit unstalled(connection);
}
