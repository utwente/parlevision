#ifndef TCPSERVERPROCESSOR_H
#define TCPSERVERPROCESSOR_H

#include <QObject>
#include <plvcore/PipelineProcessor.h>
#include <plvcore/RefPtr.h>
#include <plvcore/Pin.h>

#include "Server.h"

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

class TCPServerProcessor : public plv::PipelineProcessor
{
    Q_OBJECT
    Q_DISABLE_COPY( TCPServerProcessor )

    Q_CLASSINFO("author", "Richard Loos")
    Q_CLASSINFO("name", "TCP Server")
    Q_CLASSINFO("description", "TCP server")

    Q_PROPERTY( int port READ getPort WRITE setPort NOTIFY portChanged  )

    /** required standard method declaration for plv::PipelineElement */
    PLV_PIPELINE_ELEMENT

public:
    TCPServerProcessor();
    virtual ~TCPServerProcessor();

    /** propery methods */
    int getPort() const;
    virtual bool isReadyForProcessing() const;

signals:
    void portChanged(int port);
    void sendData(const QByteArray&);

public slots:
    void setPort(int port, bool doEmit=false );
    void stalled();
    void unstalled();

private:
    void sendFrame( const QVariantList& frameData );

    int m_port;
    Server* m_server;
    bool m_waiting;
    plv::CvMatDataInputPin* m_inputPinCvMatData;
    plv::InputPin<double>* m_inputPinDouble;
};

#endif // TCPSERVERPROCESSOR_H
