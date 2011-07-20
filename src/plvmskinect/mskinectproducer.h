#ifndef MSKINECTPRODUCER_H
#define MSKINECTPRODUCER_H

#include <plvcore/PipelineProducer.h>
#include <plvcore/CvMatData.h>
#include <plvcore/Pin.h>
#include <QMutex>

#include "mskinectdatatypes.h"

namespace plv
{
    class CvMatDataOutputPin;
}

namespace plvmskinect
{
    class KinectDevice;

    class MSKinectProducer : public plv::PipelineProducer
    {
        Q_OBJECT
        Q_DISABLE_COPY( MSKinectProducer )

        Q_CLASSINFO("author", "Richard Loos")
        Q_CLASSINFO("name", "MSKinectProducer")
        Q_CLASSINFO("description", "This producer produces data from the "
                    "Microsoft Kinect using the MS Kinect SDK.")

        /** required standard method declaration for plv::PipelineProducer */
        PLV_PIPELINE_PRODUCER

    public:
        MSKinectProducer();
        virtual ~MSKinectProducer();

        virtual bool init();
        virtual bool deinit() throw();
        virtual bool start();
        virtual bool stop();

    public slots:
        void newDepthFrame( plv::CvMatData depth );
        void newVideoFrame( plv::CvMatData video );
        void newSkeletonFrame( plvmskinect::SkeletonFrame frame );
        void kinectFinished();

    private:
        plv::CvMatDataOutputPin* m_outputPinVideo;
        plv::CvMatDataOutputPin* m_outputPinDepth;
        plv::OutputPin<SkeletonFrame>* m_outputPinSkeleton;

        KinectDevice* m_kinect;
        mutable QMutex m_kinectProducerMutex;

        plv::CvMatData m_videoFrame;
        plv::CvMatData m_depthFrame;
        SkeletonFrame m_skeletonFrame;
    };
}

#endif // MSKINECTPRODUCER_H
