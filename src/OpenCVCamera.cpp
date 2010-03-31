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
    if( m_state != CAM_STARTING )
    {
        return;
    }

    m_state = CAM_RUNNING;

    while( m_state != CAM_STOPPING )
    {
        // get a frame, blocking call
        const IplImage* frame = getFrame();

        // send a signal to subscribers
        emit newFrame( frame );

        while(m_state == CAM_PAUSED)
        {
            QThread::msleep( (unsigned long)( 1000 / 30.0 ) );
        }
    }

    m_state = CAM_STOPPED;
}

void OpenCVCamera::start()
{
    if( m_state > CAM_UNITIALIZED )
    {
        m_state = CAM_STARTING;
    }

    QThread::start();
}

OpenCVCamera::CameraState OpenCVCamera::getState()
{
    return m_state;
}

void OpenCVCamera::stop()
{
    if( m_state == CAM_RUNNING ||
        m_state == CAM_PAUSED )
    {
        m_state = CAM_STOPPING;
    }
}

void OpenCVCamera::pause()
{
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

