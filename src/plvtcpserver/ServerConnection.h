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

#ifndef SERVERCONNECTION_H
#define SERVERCONNECTION_H

#include <QTcpSocket>
#include <QByteArray>
#include <QMutex>

#include <plvcore/plvglobal.h>

//#define MAX_FRAMES_IN_QUEUE 1
//#define MAX_FRAMES_IN_FLIGHT 1
//#define MAX_BYTES_TO_WRITE 64

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
    ServerConnection(int socketDescriptor, bool lossless, int maxFrameQueue, int maxFramesInFlight);
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

    void setMaxFrameQueue(int max);
    void setMaxFramesInFlight(int max);
    void setLossless(bool lossless);

signals:
    void onError(const QString& msg);
    void finished(ServerConnection* connection);

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
    bool m_waiting;
    bool m_lossless;
    int m_maxFramesInQueue;
    int m_maxFramesInFlight;
    int m_blockSize;

    QList<quint32> m_frameNumbersInFlight; /** framenumbers which have not been ack-ed yet */
    QList<Frame> m_frameQueue;
};

#endif
