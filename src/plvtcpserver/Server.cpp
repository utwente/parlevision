/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "Server.h"
#include "ServerConnection.h"

//#include <stdlib.h>

Server::Server(QObject *parent) : QTcpServer(parent)
{
    connect( this, SIGNAL( stalled() ), parent, SLOT( stalled() ) );
    connect( this, SIGNAL( unstalled() ), parent, SLOT( unstalled() ) );
}

void Server::incomingConnection(int socketDescriptor)
{
    ServerConnection* connection = new ServerConnection(socketDescriptor);

    // let errors go through the error reporting signal of this class
    this->connect( connection,
                   SIGNAL(error(QString)),
                   SIGNAL(error(QString)));

    // move the connection to its own thread
    QThreadEx* connectionThread = new QThreadEx();
    connection->moveToThread(connectionThread);

    // when the connection is done, it stops its thread
    connect( connection, SIGNAL(finished()),
             connectionThread, SLOT(quit()));

    // when the connection is done it is scheduled for deletion
    connect( connection, SIGNAL(finished()),
             connection, SLOT(deleteLater()));

    // start the connection when its thread is started
    connect( connectionThread, SIGNAL(started()),
             connection, SLOT(start()));

    // inform server when this serverthead is waiting on the client
    connect( connection, SIGNAL( waitingOnClient(ServerConnection*,bool)),
             this, SLOT( serverThreadStalled(ServerConnection*,bool)) );

    // stop this connection when stopAllConnections is called
    connect( this, SIGNAL(stopAllConnections()), connection, SLOT(stop()));

    // start the connection thread and its event loop
    connectionThread->start();

    // connection receives all broadcasts and sends it to its connection
    connect( this, SIGNAL(broadcast(const QByteArray&)),
             connection, SLOT(writeBytes(const QByteArray&)));
}

 void Server::serverThreadStalled( ServerConnection* connection, bool isStalled )
 {
     if( isStalled ) emit stalled();
     else emit unstalled();
 }
