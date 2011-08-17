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
        void newDepthFrame( int deviceIndex, plv::CvMatData depth );
        void newVideoFrame( int deviceIndex, plv::CvMatData video );
        void newSkeletonFrame( int deviceIndex, plvmskinect::SkeletonFrame frame );
        void kinectFinished( int deviceIndex );

    private:
        int m_deviceCount;

        QVector<plv::CvMatDataOutputPin*> m_outputPinsVideo;
        QVector<plv::CvMatDataOutputPin*> m_outputPinsDepth;
        QVector<plv::OutputPin<SkeletonFrame>*> m_outputPinsSkeleton;

        mutable QMutex m_kinectProducerMutex;
        QVector< KinectDevice* > m_kinects;

        QVector<plv::CvMatData> m_videoFrames;
        QVector<plv::CvMatData> m_depthFrames;
        QVector<SkeletonFrame> m_skeletonFrames;
    };
}

#endif // MSKINECTPRODUCER_H
