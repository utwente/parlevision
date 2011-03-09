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

#ifndef TCPCLIENTPRODUCER_H
#define TCPCLIENTPRODUCER_H

#include <plvcore/PipelineProducer.h>
#include <plvcore/Pin.h>
#include <plvcore/Types.h>
#include <plvcore/CvMatDataPin.h>

#include <QAbstractSocket>

QT_FORWARD_DECLARE_CLASS( QTcpSocket )
QT_FORWARD_DECLARE_CLASS( QNetworkSession )

class TCPClientProducer : public plv::PipelineProducer
{
    Q_OBJECT
    PLV_PIPELINE_PRODUCER

    Q_DISABLE_COPY( TCPClientProducer )

    Q_CLASSINFO("author", "Richard Loos")
    Q_CLASSINFO("name", "TCP Client")
    Q_CLASSINFO("description", "TCP client")

    Q_PROPERTY( int port READ getPort WRITE setPort NOTIFY portChanged  )
    Q_PROPERTY( QString serverIP READ getServerIP WRITE setServerIP NOTIFY serverIPChanged )

public:
    TCPClientProducer();
    virtual ~TCPClientProducer();

    virtual bool init();
    virtual bool deinit() throw();
    virtual bool start();
    virtual bool stop();

    bool parseFrame(QDataStream& in);
    //bool parseConfig(QDataStream& in);

    int getPort() const;
    QString getServerIP() const;

signals:
    void portChanged(int port);
    void serverIPChanged(QString ip);

public slots:
    void setPort(int port, bool doEmit=false );
    void setServerIP( const QString& ip, bool doEmit=false );
    void readData();
    void sessionOpened();
    void displayError(QAbstractSocket::SocketError socketError, bool signal=true);
    void connected();
    void disconnected();

private:
    void ackFrame(quint32 frameNumber);

    QTcpSocket* m_tcpSocket;
    QString m_ipAddress;
    int m_port;
    int m_blockSize;
    QNetworkSession* m_networkSession;
    bool m_configured;
    QVector<QVariant::Type> m_types;
    QList<QVariantList> m_frameList;
    QMutex m_frameListMutex;

    plv::OutputPin<int>* m_intOut;
    plv::OutputPin<QString>* m_stringOut;
    plv::OutputPin<float>* m_floatOut;
    plv::OutputPin<double>* m_doubleOut;
    plv::OutputPin<cv::Scalar>* m_cvScalarOut;
    plv::CvMatDataOutputPin* m_imageOut;

};

#endif // TCPCLIENTPRODUCER_H
