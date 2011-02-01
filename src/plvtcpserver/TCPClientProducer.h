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
    void displayError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket* m_tcpSocket;
    QString m_ipAddress;
    int m_port;
    int m_blockSize;
    QNetworkSession* m_networkSession;
    bool m_configured;
    QVector<QVariant::Type> m_types;
    QList<QVariantList> m_frameList;

    plv::OutputPin<int>* m_intOut;
    plv::OutputPin<QString>* m_stringOut;
    plv::OutputPin<float>* m_floatOut;
    plv::OutputPin<double>* m_doubleOut;
    plv::OutputPin<cv::Scalar>* m_cvScalarOut;
    plv::CvMatDataOutputPin* m_imageOut;

};

#endif // TCPCLIENTPRODUCER_H
