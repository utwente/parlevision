#ifndef CAMERAPRODUCER_H
#define CAMERAPRODUCER_H

#include <QMutex>
#include <QWaitCondition>

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
        CameraProducer(const CameraProducer&);

        virtual void process();

        virtual PlvPipelineElementState init();
        virtual PlvPipelineElementState checkConfig();

    protected:

        RefPtr<OpenCVCamera> m_camera;
        RefPtr<OpenCVImage> m_lastFrame;
        RefPtr< OutputPin<OpenCVImage> > m_outputPin;

        int m_lastProcessedId;

        QMutex m_frameMutex;
        QWaitCondition m_frameReady;

    public slots:
        void newFrame( RefPtr<Data> frame );
    };

}

#endif // CAMERAPRODUCER_H
