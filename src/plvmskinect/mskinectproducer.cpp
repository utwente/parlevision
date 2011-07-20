#include "mskinectproducer.h"
#include "mskinectdevice.h"
#include <plvcore/CvMatDataPin.h>

using namespace plvmskinect;

MSKinectProducer::MSKinectProducer()
{
    // we have one output pin
    m_outputPinDepth = plv::createCvMatDataOutputPin( "depth", this );
    m_outputPinVideo = plv::createCvMatDataOutputPin( "camera", this );
    m_outputPinSkeleton = plv::createOutputPin<SkeletonFrame>( "skeletons", this );

    // supports all types of images
    m_outputPinVideo->addAllChannels();
    m_outputPinVideo->addAllDepths();

    m_outputPinDepth->addAllChannels();
    m_outputPinDepth->addAllDepths();

    m_kinect = new KinectDevice(this);

    connect( m_kinect, SIGNAL( newDepthFrame( plv::CvMatData ) ),
             this,     SLOT( newDepthFrame( plv::CvMatData ) ) );

    connect( m_kinect, SIGNAL( newVideoFrame( plv::CvMatData ) ),
            this,     SLOT( newVideoFrame( plv::CvMatData ) ) );

    connect( m_kinect, SIGNAL( newSkeletonFrame( plvmskinect::SkeletonFrame ) ),
             this,     SLOT( newSkeletonFrame( plvmskinect::SkeletonFrame ) ) );

    connect( m_kinect, SIGNAL( finished()), this, SLOT(kinectFinished()));
}

MSKinectProducer::~MSKinectProducer()
{
}

bool MSKinectProducer::init()
{
    if( !m_kinect->init() )
    {
        setError( PlvPipelineInitError, tr("Kinect with id %1 failed to initialise").arg(m_kinect->getId()) );
        return false;
    }
    return true;
}

bool MSKinectProducer::deinit() throw()
{
    m_kinect->deinit();
    return true;
}

bool MSKinectProducer::start()
{
    m_kinect->start();
    return true;
}

bool MSKinectProducer::stop()
{
    m_kinect->stop();
    return true;
}

bool MSKinectProducer::produce()
{
    QMutexLocker lock( &m_kinectProducerMutex );
    m_outputPinDepth->put( m_depthFrame );
    m_outputPinVideo->put( m_videoFrame );

    m_depthFrame = cv::Mat();
    m_videoFrame = cv::Mat();

    if( m_skeletonFrame.isValid() )
    {
        m_outputPinSkeleton->put( m_skeletonFrame );
        m_skeletonFrame = SkeletonFrame();
    }
    return true;
}

bool MSKinectProducer::readyToProduce() const
{
    return m_depthFrame.isValid() && m_videoFrame.isValid();
}

void MSKinectProducer::newDepthFrame( plv::CvMatData depth )
{
    QMutexLocker lock( &m_kinectProducerMutex );
    m_depthFrame = depth;
}

void MSKinectProducer::newVideoFrame( plv::CvMatData video )
{
    QMutexLocker lock( &m_kinectProducerMutex );
    m_videoFrame = video;
}

void MSKinectProducer::newSkeletonFrame( plvmskinect::SkeletonFrame frame )
{
    QMutexLocker lock( &m_kinectProducerMutex );
    m_skeletonFrame = frame;
}

void MSKinectProducer::kinectFinished()
{
    if( getState() > INITIALIZED )
    {
        setError( PlvNonFatalError, tr("Kinect quit unexpectedly.") );
        emit onError( PlvNonFatalError, this );
    }
}
