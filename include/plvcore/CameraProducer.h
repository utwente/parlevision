#ifndef CAMERAPRODUCER_H
#define CAMERAPRODUCER_H

#include <QMutex>
#include <QWaitCondition>

#include "PipelineProducer.h"
#include "Pin.h"
#include "OpenCVImage.h"
#include "OpenCVCamera.h"

namespace plv {

    class CameraProducer : public PipelineProducer
    {
        Q_OBJECT

    public:
        CameraProducer();
        virtual ~CameraProducer();
        CameraProducer(const CameraProducer&);

        /** inherited by PipelineProducer */
        virtual void start() throw (PipelineException);
        virtual void stop() throw (PipelineException);

        /** inherited by PipelineElement */
        virtual void init() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        RefPtr<OpenCVCamera> getCamera() const { return m_camera; }

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
