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

#ifndef PARLEVISIONSERVER_H
#define PARLEVISIONSERVER_H

#include <plvcore/plvglobal.h>
#include <QTcpServer>
#include <QThread>
#include <QVariantList>

class ServerConnection;

class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server(QObject *parent);

public slots:
    /** sends the content of the array to all connected clients.
        Calls queueFrame slot of all server threads */
    void sendFrame(quint32 frameNumber, const QVariantList& frameData);

    /** stops all connection threads and disconnects connections */
    void disconnectAll();

    /** called when one of the connection threads has stalled */
    void serverThreadStalled( ServerConnection* connection, bool stalled );

signals:
    /** is fired when an error has occured in this Server or in one
        of its Connection threads */
    void error(PipelineErrorType type, const QString& msg);

    /** stops all connection threads and disconnects connections */
    void stopAllConnections();

    void broadcastFrame(quint32 serial, QByteArray);

    void stalled();
    void unstalled();

private:
    void incomingConnection(int socketDescriptor);
};

/** Helper class for a QThread to run its own event loop */
class QThreadEx : public QThread
{
protected:
    void run() { exec(); }
};

#endif
