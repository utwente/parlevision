#include "OpenCVCamera.h"

#include <opencv/highgui.h>
#include <opencv/cv.h>

#include <QDebug>

using namespace plv;

OpenCVCamera::OpenCVCamera( int id ) :
    m_id( id ),
    m_state( CAM_UNINITIALIZED ),
    m_captureDevice( 0 )
{
}

OpenCVCamera::~OpenCVCamera()
{
    releaseCapture();
}

bool OpenCVCamera::init()
{
    QMutexLocker lock( &m_opencv_mutex );

    qDebug() << "Trying to initialize OpenCV camera with id " << m_id;

    m_captureDevice = cvCreateCameraCapture( m_id );
    if( m_captureDevice == 0 )
    {
        qDebug() << "Initialization of camera failed";
        return false;
    }

    m_state = CAM_INITIALIZED;
    qDebug() << "OpenCV camera initialised with initial resolution of "
            << width() << "," << height();
    return true;
}

int OpenCVCamera::width() const
{
    return (int) cvGetCaptureProperty( m_captureDevice, CV_CAP_PROP_FRAME_WIDTH );
}

int OpenCVCamera::height() const
{
    return (int) cvGetCaptureProperty( m_captureDevice, CV_CAP_PROP_FRAME_HEIGHT );
}

void OpenCVCamera::run()
{
    assert( m_state != CAM_UNINITIALIZED );
    assert( m_captureDevice != 0 );

    while( m_state == CAM_RUNNING || m_state == CAM_PAUSED )
    {
        // get a frame, blocking call
        RefPtr<Data> frame = getFrame();

        // send a signal to subscribers
        if( frame.isNotNull() )
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
            // mutex was relocked by the condition
            // unlock it here
            m_mutex.unlock();
        }
    }
}

void OpenCVCamera::start()
{
    qDebug() << "starting camera";
    QMutexLocker lock( &m_mutex );

    switch(m_state)
    {
    case CAM_UNINITIALIZED:
        // TODO throw exception?
        return;
    case CAM_INITIALIZED:
        // Start thread
        m_state = CAM_RUNNING;
        QThread::start();
    case CAM_RUNNING:
        // Do nothing
        return;
    case CAM_PAUSED:
        // Resume
        m_state = CAM_RUNNING;
        m_condition.wakeOne();
    }

}

OpenCVCamera::CameraState OpenCVCamera::getState() const
{
    return m_state;
}


void OpenCVCamera::pause()
{
    QMutexLocker lock( &m_mutex );

    if( m_state == CAM_RUNNING )
    {
        m_state = CAM_PAUSED;
    }
}

//void OpenCVCamera::stop()
//{
//    QMutexLocker lock( &m_mutex );
//
//    if( m_state == CAM_RUNNING )
//    {
//        m_state = CAM_STOPPING;
//    }
//    else if( m_state == CAM_PAUSED )
//    {
//        // this will wake the thread after this method has exited
//        m_condition.wakeOne();
//    }
//}

void OpenCVCamera::releaseCapture()
{
    QMutexLocker lock( &m_opencv_mutex );
    if(m_captureDevice !=0)
    {
        cvReleaseCapture(&m_captureDevice);
        m_captureDevice = 0;
    }
}

void OpenCVCamera::release()
{
    QMutexLocker lock( &m_mutex );

    CameraState current_state = m_state;
    m_state = CAM_UNINITIALIZED;

    switch(current_state)
    {
    case CAM_PAUSED:
        // this will wake the thread after m_mutex is released, unpausing it
        m_condition.wakeOne();
        m_mutex.unlock();
        m_mutex.lock();
        //fallthrough
    case CAM_RUNNING:
        // switching m_state to CAM_UNINITIALIZED
        // will cause the run loop to exit eventually.
        // wait for that here.
        QThread::wait();
        //fallthrough
    case CAM_INITIALIZED:
        // the thread is not running, so it's safe to release the capture device.
        releaseCapture();
        // fallthrough
    case CAM_UNINITIALIZED:
        return;
    }
}

bool OpenCVCamera::setDimensions( int w, int h )
{
    assert( w > 0 );
    assert( h > 0 );

    // we to convert to double for opencv
    double dw = (double)w;
    // check for 4:3 dimensions
    // OpenCV only support 4:3 camera resolutions right now
    if(((int)(dw * (3.0/4.0) )) != h)
    {
        qDebug()<< "Camera resolution failed to change to " << w << "x" << h
                << " and is at " << width() << "x" << height() << " because OpenCV "
                "only supports a 4:3 aspect ratio.";
        return false;
    }
    else
    {
        cvSetCaptureProperty( m_captureDevice, CV_CAP_PROP_FRAME_WIDTH, dw );

        int nwidth = width();
        int nheight = height();
        if( nwidth == w && nheight == h )
        {
            qDebug() << "Camera resolution changed to " << nwidth << "x" << nheight;
            return true;
        }

        qDebug()<< "Camera resolution failed to change to " << w << "x" << h
                << " and is at " << nwidth << "x" << nheight;

        return false;
    }
}

OpenCVImage* OpenCVCamera::getFrame()
{
    QMutexLocker lock( &m_opencv_mutex );

    if( m_state == CAM_UNINITIALIZED )
        return 0;

    // first grab a frame, this is a fast function
    int status = cvGrabFrame( m_captureDevice );

    if( !status )
    {
        return 0;
    }

    // now do post processing such as decompression
    const IplImage* image = cvRetrieveFrame( m_captureDevice );

    // copy the image, since the pointer becomes invalid on another
    // call to cvGrabFrame() and this pointer is passed to the pipeline
    // get a reused buffer
    OpenCVImage* ocvimg = OpenCVImageFactory::instance()->getFromBuffer( image );
    return ocvimg;
}

/*
int OpenCVCamera::getNumberOfAvailableCameras()
{
    int num = 0;
    bool finished = false;

    while( !finished )
    {
        CvCapture* capture = cvCreateCameraCapture( num );
        if( !capture )
        {
            finished = true;
        }
        else
        {
            num++;
            cvReleaseCapture( &capture );
        }
    }
    return num;
}
*/
