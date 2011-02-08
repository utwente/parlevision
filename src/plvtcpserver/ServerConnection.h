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

#ifndef SERVERCONNECTION_H
#define SERVERCONNECTION_H

#include <QTcpSocket>
#include <QByteArray>
#include <QMutex>

#include <plvcore/plvglobal.h>

class Frame
{
public:
    quint32 serial;
    QByteArray bytes;
    qint64 sent;

    Frame(quint32 s, QByteArray& b) : serial(s), bytes(b), sent(0) {}
};

class ServerConnection : public QObject
{
    Q_OBJECT

public:
    ServerConnection(int socketDescriptor);
    virtual ~ServerConnection();

public slots:
    void start();
    void stop();

    void connected();
    void disconnected();

    /** queues the frame in the internal frame queue. The frame will
        be dropped if the queue is at its maximum size and the connection
        is not lossless. If the connection is lossless and the queue is full
        it will emit the waitingOnClient signal */
    void queueFrame(quint32 frameNumber, QByteArray frameData);

    void sendData();

    void bytesWritten( qint64 bytes );
    void readyRead();

    void error(QAbstractSocket::SocketError socketError);

signals:
    void errorOccurred(PipelineErrorType type, const QString& msg);
    void finished();

    /** emitted when the TCP connection is waiting for the client */
    void waitingOnClient( ServerConnection* connection, bool waiting );

    void configured(bool configured);

    void scheduleSend();

private:
    void ackFrame(quint32 serial);

    QTcpSocket* m_tcpSocket;
    int m_socketDescriptor;
    QString m_errorString;
    int m_frameNumber;
    //QMutex m_scMutex;
    bool m_waiting;
    bool m_lossless;
    int m_blockSize;

    QList<quint32> m_frameNumbersInFlight; /** framenumbers which have not been ack-ed yet */
    QList<Frame> m_frameQueue;
};

#endif
