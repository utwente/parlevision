#ifndef TCPSERVERPROCESSOR_H
#define TCPSERVERPROCESSOR_H

#include <QObject>
#include <plvcore/PipelineProcessor.h>
#include <plvcore/RefPtr.h>
#include <plvcore/Pin.h>
#include <opencv/cv.h>

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

    /** required standard method declaration for plv::PipelineProcessor */
    PLV_PIPELINE_PROCESSOR

public:
    TCPServerProcessor();
    virtual ~TCPServerProcessor();

    /** propery methods */
    int getPort() const;
    virtual bool isReadyForProcessing() const;

signals:
    void portChanged(int port);
    void sendFrame(quint32 frameNumber, const QVariantList& frameData);

public slots:
    void setPort(int port, bool doEmit=false );
    void stalled();
    void unstalled();
    void serverError(PipelineErrorType type, const QString& msg);

private:
    void acceptConfigurationRequest();

    int m_port;
    Server* m_server;
    bool m_waiting;
    plv::CvMatDataInputPin* m_inputPinCvMatData;
    plv::InputPin<double>* m_inputPinDouble;
    plv::InputPin<cv::Scalar>* m_inputPinCvScalar;
};

#endif // TCPSERVERPROCESSOR_H
