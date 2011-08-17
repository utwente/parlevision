#include "mskinectproducer.h"
#include "mskinectdevice.h"
#include <plvcore/CvMatDataPin.h>

using namespace plvmskinect;

MSKinectProducer::MSKinectProducer() : m_deviceCount( 0 )
{
    //NUIAPI HRESULT MSR_NUIGetDeviceCount( int * pCount ); // note capitalization
    //NUIAPI HRESULT MSR_NuiCreateInstanceByIndex( int Index, INuiInstance ** ppInstance );
    //NUIAPI void    MSR_NuiDestroyInstance( INuiInstance * pInstance );

    MSR_NUIGetDeviceCount( &m_deviceCount );
    if( m_deviceCount == 0 )
    {

    }
    else
    {
        m_depthFrames.resize( m_deviceCount );
        m_videoFrames.resize( m_deviceCount );
        m_skeletonFrames.resize( m_deviceCount );

        m_outputPinsDepth.resize( m_deviceCount );
        m_outputPinsVideo.resize( m_deviceCount );
        m_outputPinsSkeleton.resize( m_deviceCount );

        for( int i = 0; i < m_deviceCount; ++i )
        {
            KinectDevice* device = new KinectDevice(i, this);
            m_kinects.push_back(device);

            connect( device, SIGNAL( newDepthFrame( int, plv::CvMatData ) ),
                     this,     SLOT( newDepthFrame( int, plv::CvMatData ) ) );

            connect( device, SIGNAL( newVideoFrame( int, plv::CvMatData ) ),
                    this,     SLOT( newVideoFrame( int, plv::CvMatData ) ) );

            connect( device, SIGNAL( newSkeletonFrame( int, plvmskinect::SkeletonFrame ) ),
                     this,     SLOT( newSkeletonFrame( int, plvmskinect::SkeletonFrame ) ) );

            connect( device, SIGNAL( deviceFinished(int)), this, SLOT(kinectFinished(int)));

            // we have one output pin
            m_outputPinsDepth[i] = plv::createCvMatDataOutputPin( "depth", this );
            m_outputPinsVideo[i] = plv::createCvMatDataOutputPin( "camera", this );
            m_outputPinsSkeleton[i] = plv::createOutputPin<SkeletonFrame>( "skeletons", this );

            // supports all types of images
            m_outputPinsVideo[i]->addAllChannels();
            m_outputPinsVideo[i]->addAllDepths();

            m_outputPinsDepth[i]->addAllChannels();
            m_outputPinsDepth[i]->addAllDepths();
        }
    }
}

MSKinectProducer::~MSKinectProducer()
{
}

bool MSKinectProducer::init()
{
    for( int i = 0; i < m_deviceCount; ++i )
    {
        if( !m_kinects.at(i)->init() )
        {
            setError( PlvPipelineInitError, tr("Kinect with id %1 failed to initialise").arg(m_kinects.at(i)->getId()) );

            // deinit already initalized devices
            for( int j=0; j < i; ++j )
                m_kinects.at(j)->deinit();

            return false;
        }
    }
    return true;
}

bool MSKinectProducer::deinit() throw()
{
    for( int i = 0; i < m_deviceCount; ++i )
    {
        m_kinects.at(i)->deinit();
    }
    return true;
}

bool MSKinectProducer::start()
{
    for( int i = 0; i < m_deviceCount; ++i )
    {
        m_kinects.at(i)->start();
    }
    return true;
}

bool MSKinectProducer::stop()
{
    for( int i = 0; i < m_deviceCount; ++i )
    {
        m_kinects.at(i)->stop();
    }
    return true;
}

bool MSKinectProducer::produce()
{
    QMutexLocker lock( &m_kinectProducerMutex );

    for( int i = 0; i < m_deviceCount; ++i )
    {
        m_outputPinsDepth.at(i)->put( m_depthFrames.at(i) );
        m_outputPinsVideo.at(i)->put( m_videoFrames.at(i) );

        m_depthFrames[i] = cv::Mat();
        m_videoFrames[i] = cv::Mat();

        if( m_skeletonFrames.at(i).isValid() )
        {
            m_outputPinsSkeleton.at(i)->put( m_skeletonFrames.at(i) );
            m_skeletonFrames[i] = SkeletonFrame();
        }
    }
    return true;
}

bool MSKinectProducer::readyToProduce() const
{
    bool ready = true;
    for( int i=0; i < m_deviceCount && ready; ++i )
    {
        ready = m_depthFrames.at(i).isValid() && m_videoFrames.at(i).isValid();
    }
    return ready;
}

void MSKinectProducer::newDepthFrame( int deviceIndex, plv::CvMatData depth )
{
    QMutexLocker lock( &m_kinectProducerMutex );
    assert( deviceIndex > -1 && deviceIndex < m_deviceCount );
    m_depthFrames[deviceIndex] = depth;
}

void MSKinectProducer::newVideoFrame( int deviceIndex, plv::CvMatData video )
{
    QMutexLocker lock( &m_kinectProducerMutex );
    assert( deviceIndex > -1 && deviceIndex < m_deviceCount );
    m_videoFrames[deviceIndex] = video;
}

void MSKinectProducer::newSkeletonFrame( int deviceIndex, plvmskinect::SkeletonFrame frame )
{
    QMutexLocker lock( &m_kinectProducerMutex );
    assert( deviceIndex > -1 && deviceIndex < m_deviceCount );
    m_skeletonFrames[deviceIndex] = frame;
}

void MSKinectProducer::kinectFinished( int deviceIndex )
{
    if( getState() > INITIALIZED )
    {
        setError( PlvNonFatalError, tr("Kinect with ID %1 quit unexpectedly.").arg(deviceIndex) );
        emit onError( PlvNonFatalError, this );
    }
}
