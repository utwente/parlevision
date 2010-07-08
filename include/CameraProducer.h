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

        /** inherited by PipelineProducer */
        virtual void start();
        virtual void stop();

        /** inherited by PipelineElement */
        virtual bool init();
        virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

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
