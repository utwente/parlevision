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

#ifndef PARLEVISIONSERVER_H
#define PARLEVISIONSERVER_H

#include <plvcore/plvglobal.h>
#include <QTcpServer>
#include <QThread>
#include <QVariantList>
#include <QMutex>

class ServerConnection;

class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server(QObject *parent);

public slots:
    /** sends the content of the array to all connected clients.
        Calls queueFrame slot of all server threads */
    void sendFrame(quint32 frameNumber, const QByteArray& data);

    /** stops all connection threads and disconnects connections */
    void disconnectAll();

    /** called when one of the connection threads has stalled */
    void serverThreadStalled( ServerConnection* connection, bool stalled );

    void setLossless(bool lossless);
    void setMaxFramesInQueue(int max);
    void setMaxFramesInFlight(int max);

    int getMaxFramesInFlight() const;
    int getMaxFramesInQueue() const;
    bool getLossless() const;

signals:
    /** is fired when an error has occured in this Server or in one
        of its Connection threads */
    void onError(PlvErrorType type, const QString& msg);

    /** stops all connection threads and disconnects connections */
    void stopAllConnections();

    void broadcastFrame(quint32 serial, QByteArray);

    void stalled(ServerConnection*);
    void unstalled(ServerConnection*);

private:
    void incomingConnection(int socketDescriptor);

    bool m_lossless;
    int m_maxFramesInQueue;
    int m_maxFramesInFlight;
    mutable QMutex m_serverPropMutex;
};

/** Helper class for a QThread to run its own event loop */
class QThreadEx : public QThread
{
protected:
    void run() { exec(); }
};

#endif
