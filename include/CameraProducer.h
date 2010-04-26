#ifndef CAMERAPRODUCER_H
#define CAMERAPRODUCER_H

#include <QMutex>

#include "PipelineProducer.h"
#include "Pin.h"
#include "OpenCVImage.h"

namespace plv {
    class OpenCVCamera;

    class CameraProducer : public PipelineProducer
    {
        Q_OBJECT

    public:
        CameraProducer();
        virtual ~CameraProducer();

        virtual void produce();

        virtual PlvPipelineElementState init();
        virtual PlvPipelineElementState checkConfig();

    protected:

        RefPtr<OpenCVCamera> m_camera;
        RefPtr<OpenCVImage> m_lastFrame;
        RefPtr< TypedOutputPin<OpenCVImage> > m_outputPin;

        int m_lastProcessedId;

        QMutex m_frameMutex;

    public slots:
        void newFrame( RefPtr<Data> frame );
    };

}

#endif // CAMERAPRODUCER_H
