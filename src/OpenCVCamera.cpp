#include "OpenCVCamera.h"

#include <opencv/highgui.h>
#include <opencv/cv.h>

OpenCVCamera::OpenCVCamera( int id ) :
    m_id( id ),
    m_state( CAM_UNITIALIZED ),
    m_width( 0 ),
    m_height( 0 ),
    m_captureDevice( 0 )
{
}

OpenCVCamera::~OpenCVCamera()
{
    if( m_captureDevice != 0 )
        cvReleaseCapture( &m_captureDevice );
}

int OpenCVCamera::init()
{
    m_captureDevice = cvCreateCameraCapture(0);
    if( m_captureDevice == 0 )
    {
        return -1;
    }

    // get width and height
    m_width  = (int) cvGetCaptureProperty( m_captureDevice, CV_CAP_PROP_FRAME_WIDTH );
    m_height = (int) cvGetCaptureProperty( m_captureDevice, CV_CAP_PROP_FRAME_HEIGHT );

    m_state = CAM_STOPPED;
    return 0;
}

void OpenCVCamera::run()
{
    assert( m_state != CAM_UNITIALIZED );
    assert( m_captureDevice != 0 );

    while( m_state != CAM_STOPPING )
    {
        // get a frame, blocking call
        const IplImage* frame = getFrame();

        // send a signal to subscribers
        if( frame != 0 )
        {
            emit newFrame( frame );
        }

        if( m_state == CAM_PAUSED )
        {
            m_mutex.lock();

            // this will let this thread wait on the event
            // and unlocks the mutex so no deadlock is created
            m_condition.wait(&m_mutex);

            // we have woken up
            // set cam state back to running again
            // if there has not been a stop request
            if( m_state != CAM_STOPPING )
            {
                m_state = CAM_RUNNING;
            }

            // mutex was relocked by the condition
            // unlock it here
            m_mutex.unlock();
        }
    }

    m_state = CAM_STOPPED;
}

void OpenCVCamera::start()
{
    QMutexLocker lock( &m_mutex );

    if( m_state > CAM_UNITIALIZED )
    {
        m_state = CAM_STARTING;
    }

    QThread::start();
}

OpenCVCamera::CameraState OpenCVCamera::getState() const
{
    return m_state;
}

void OpenCVCamera::stop()
{
    QMutexLocker lock( &m_mutex );

    if( m_state == CAM_RUNNING )
    {
        m_state = CAM_STOPPING;
    }
    else if( m_state == CAM_PAUSED )
    {
        // this will wake the thread after this method has exited
        m_condition.wakeOne();
    }
}

void OpenCVCamera::pause()
{
    QMutexLocker lock( &m_mutex );

    if( m_state == CAM_RUNNING )
    {
        m_state = CAM_PAUSED;
    }
}

const IplImage* OpenCVCamera::getFrame()
{
    if( m_state == CAM_UNITIALIZED )
        return 0;

    // first grab a frame, this is a fast function
    int status = cvGrabFrame( m_captureDevice );

    // now do post processing such as decompression
    if( status )
    {
        const IplImage* image = cvRetrieveFrame( m_captureDevice );
        return image;
    }

    return 0;
}

