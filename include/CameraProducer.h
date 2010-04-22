#ifndef CAMERAPRODUCER_H
#define CAMERAPRODUCER_H

#include <QMutex>

#include "PipelineProducer.h"
#include "Pin.h"

namespace plv {
    class OpenCVImage;
    class OpenCVCamera;

    class CameraProducer : public PipelineProducer
    {
        Q_OBJECT

    public:
        CameraProducer( Pipeline* parent );

        virtual void produce();

        virtual PlvPipelineElementState init();
        virtual PlvPipelineElementState checkConfig();

    protected:
        virtual ~CameraProducer();

        RefPtr<OpenCVCamera> m_camera;
        RefPtr<OpenCVImage> m_lastFrame;
        RefPtr< OutputPin<OpenCVImage> > m_outputPin;

        int m_lastProcessedId;

        QMutex m_frameMutex;

    public slots:
        void newFrame(OpenCVImage* frame);
    };

}

#endif // CAMERAPRODUCER_H
