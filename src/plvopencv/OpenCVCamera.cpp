/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvcore module of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

#include "OpenCVCamera.h"

#include <opencv/highgui.h>
#include <opencv/cv.h>

#include <QDebug>

using namespace plv;
using namespace plvopencv;

OpenCVCamera::OpenCVCamera(QObject* parent) :
    QThread(parent),
    m_id( 0 ),
    m_state( CAM_UNINITIALIZED ),
    m_captureDevice( 0 ),
    m_imgCount( 0 )
{
}

OpenCVCamera::~OpenCVCamera()
{
    releaseCapture();
}

bool OpenCVCamera::init( int id )
{
    QMutexLocker lock( &m_opencv_mutex );
    m_id = id;

    qDebug() << "Trying to initialize OpenCV camera with id " << m_id;

    m_captureDevice = cvCreateCameraCapture( m_id );
    if( m_captureDevice == 0 )
    {
        qDebug() << "Initialization of camera failed";
        return false;
    }

    setFPS(30);

    m_state = CAM_INITIALIZED;
    qDebug() << "OpenCV camera initialised with initial resolution of "
             << width() << "," << height() << " and fps " << FPS();
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

int OpenCVCamera::setFPS(int value)
{
    assert(value >= 0);
    return cvSetCaptureProperty( m_captureDevice, CV_CAP_PROP_FPS, value);
}

int OpenCVCamera::FPS() const
{
    return (int) cvGetCaptureProperty( m_captureDevice, CV_CAP_PROP_FPS);
}

void OpenCVCamera::run()
{
    assert( m_state != CAM_UNINITIALIZED );
    assert( m_captureDevice != 0 );

    while( m_state == CAM_RUNNING || m_state == CAM_PAUSED )
    {
        // get a frame, blocking call
        CvMatData frame;
        if( !getFrame(frame) )
        {
            qWarning() << "Camera failed to grab frame. Exiting camera loop.";
            return;
        }

        // send a signal to subscribers
        if( frame.isValid() )
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

    cvSetCaptureProperty( m_captureDevice, CV_CAP_PROP_FRAME_WIDTH, dw );

    // check if properties did change
    int nwidth = width();
    int nheight = height();
    if( nwidth != w || nheight != h )
    {
        qDebug()<< "Camera resolution failed to change to " << w << "x" << h
                << " and is at " << nwidth << "x" << nheight;
        return false;
    }

    qDebug() << "Camera resolution changed to " << nwidth << "x" << nheight;
    return true;
}

bool OpenCVCamera::getFrame( CvMatData& mat )
{
    QMutexLocker lock( &m_opencv_mutex );

    if( m_state == CAM_UNINITIALIZED )
        return 0;

    // first grab a frame, this is a fast function
    int status = cvGrabFrame( m_captureDevice );

    if( !status )
        return false;

    // now do post processing such as decompression
    const IplImage* image = cvRetrieveFrame( m_captureDevice );

    if( image == 0 )
        return false;

    // add one to image count
    ++m_imgCount;

    qDebug() << tr("image %1").arg(m_imgCount);

    // copy the image, since the pointer becomes invalid on another
    // call to cvGrabFrame() and this pointer is passed to the pipeline
    // get a reused buffer
    mat = image;
    return true;
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
