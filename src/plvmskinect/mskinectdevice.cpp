/**
  * Copyright (C)2011 by Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvmskinect module of ParleVision.
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

#include "mskinectdevice.h"
#include <QDebug>
#include <QMutexLocker>

using namespace plv;
using namespace plvmskinect;

KinectDevice::KinectDevice(int id, QObject* parent) :
    QThread(parent)
{
    zero();
    m_id = id;
    connect( this, SIGNAL( finished()), this, SLOT( threadFinished()) );
}

KinectDevice::~KinectDevice()
{
}

void KinectDevice::zero()
{
    m_state = KINECT_UNINITIALIZED;

    m_hNextDepthFrameEvent = NULL;
    m_hNextVideoFrameEvent = NULL;
    m_hNextSkeletonEvent   = NULL;
    m_pDepthStreamHandle   = NULL;
    m_pVideoStreamHandle   = NULL;
    m_hThNuiProcess        = NULL;
    m_hEvNuiProcessStop    = NULL;

    //ZeroMemory(m_Pen, sizeof(m_Pen));
    //m_SkeletonDC           = NULL;
    //m_SkeletonBMP          = NULL;
    //m_SkeletonOldObj       = NULL;
    //m_PensTotal            = 6;
    //ZeroMemory(m_Points,sizeof(m_Points));

    //m_LastSkeletonFoundTime = -1;
    //m_bScreenBlanked        = false;
    //m_FramesTotal           = 0;
    //m_LastFPStime           = -1;
    //m_LastFramesTotal       = 0;
}

bool KinectDevice::init()
{
    // kinect must not already be initialized
    assert( getState() == KINECT_UNINITIALIZED );
    
    QMutexLocker lock( &m_deviceMutex );
    
    // initialize all variables except ID to default values
    zero();

    HRESULT hr;

    hr = MSR_NuiCreateInstanceByIndex( m_id, &m_nuiInstance );
    if( FAILED( hr ) )
    {
        qDebug() << tr("Kinect device with id %1 failed MSR_NuiCreateInstanceByIndex.").arg(m_id);
        return false;
    }

    m_hNextDepthFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    m_hNextVideoFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    m_hNextSkeletonEvent   = CreateEvent( NULL, TRUE, FALSE, NULL );
    m_hEvNuiProcessStop    = CreateEvent( NULL, FALSE,FALSE, NULL);

//    hr = NuiInitialize( NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX |
//                        NUI_INITIALIZE_FLAG_USES_SKELETON |
//                        NUI_INITIALIZE_FLAG_USES_COLOR );

    hr = m_nuiInstance->NuiInitialize( NUI_INITIALIZE_FLAG_USES_DEPTH
                        // | NUI_INITIALIZE_FLAG_USES_SKELETON
                         | NUI_INITIALIZE_FLAG_USES_COLOR
                        );

    if( FAILED( hr ) )
    {
        //MessageBoxResource(m_hWnd,IDS_ERROR_NUIINIT,MB_OK | MB_ICONHAND);
        qDebug() << tr("Kinect device with id %1 failed NuiInitialize with return handle %2.").arg(m_id).arg(hr);
        return false;
    }

    hr = m_nuiInstance->NuiSkeletonTrackingEnable( m_hNextSkeletonEvent, 0 );
    if( FAILED( hr ) )
    {
        //MessageBoxResource(m_hWnd,IDS_ERROR_SKELETONTRACKING,MB_OK | MB_ICONHAND);
        qDebug() << tr("Failed to open skeleton stream on Kinect with id %1 and with return handle %2.").arg(m_id).arg(hr);
        return false;
    }

    hr = m_nuiInstance->NuiImageStreamOpen(
        NUI_IMAGE_TYPE_COLOR,
        NUI_IMAGE_RESOLUTION_640x480,
        0,
        2,
        m_hNextVideoFrameEvent,
        &m_pVideoStreamHandle );
    if( FAILED( hr ) )
    {
        //MessageBoxResource(m_hWnd,IDS_ERROR_VIDEOSTREAM,MB_OK | MB_ICONHAND);
        qDebug() << tr("Failed to open video stream on Kinect with id %1.").arg(m_id);
        return false;
    }

//    hr = NuiImageStreamOpen(
//        NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
//        NUI_IMAGE_RESOLUTION_320x240,
//        0,
//        2,
//        m_hNextDepthFrameEvent,
//        &m_pDepthStreamHandle );
    hr = m_nuiInstance->NuiImageStreamOpen(
        NUI_IMAGE_TYPE_DEPTH,
        NUI_IMAGE_RESOLUTION_640x480,
        0,
        2,
        m_hNextDepthFrameEvent,
        &m_pDepthStreamHandle );
    if( FAILED( hr ) )
    {
        //MessageBoxResource(m_hWnd,IDS_ERROR_DEPTHSTREAM,MB_OK | MB_ICONHAND);
        qDebug() << tr("Failed to open depth stream on Kinect with id %1.").arg(m_id);
        return false;
    }

    setState( KINECT_INITIALIZED );

    return true;
}

bool KinectDevice::deinit()
{
    m_nuiInstance->NuiShutdown( );
    MSR_NuiDestroyInstance( m_nuiInstance );
    if( m_hNextSkeletonEvent && ( m_hNextSkeletonEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_hNextSkeletonEvent );
        m_hNextSkeletonEvent = NULL;
    }
    if( m_hNextDepthFrameEvent && ( m_hNextDepthFrameEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_hNextDepthFrameEvent );
        m_hNextDepthFrameEvent = NULL;
    }
    if( m_hNextVideoFrameEvent && ( m_hNextVideoFrameEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_hNextVideoFrameEvent );
        m_hNextVideoFrameEvent = NULL;
    }
    setState( KINECT_UNINITIALIZED );

    return true;
}

int KinectDevice::getId() const
{
    return m_id;
}

int KinectDevice::width() const
{
    return 0;
}

int KinectDevice::height() const
{
    return 0;
}

void KinectDevice::start()
{
    qDebug() << "Starting kinect";
    
    QMutexLocker lock( &m_deviceMutex );

    switch( getState() )
    {
    case KINECT_UNINITIALIZED:
        // TODO throw exception?
        break;
    case KINECT_INITIALIZED:
        // Start thread
        QThread::start();
        break;
    case KINECT_RUNNING:
    default:
        // Do nothing
        return;
    }
}

void KinectDevice::stop()
{
    QMutexLocker lock( &m_deviceMutex );

    switch( getState() )
    {
    case KINECT_RUNNING:
        // Stop the Nui processing thread
        // Signal the thread
        setState( KINECT_STOP_REQUESTED );
        // switching m_state to KINECT_STOP_REQUESTED
        // will cause the run loop to exit eventually.
        // wait for that here.
        QThread::wait();
        //fallthrough
    case KINECT_INITIALIZED:
        // the thread is not running, so it's safe to release the capture device.
        // fallthrough
    case KINECT_UNINITIALIZED:
    case KINECT_STOP_REQUESTED:
        return;
    }
}

void KinectDevice::run()
{
    setState(KINECT_RUNNING);
    
    // Configure events to be listened on
    HANDLE hEvents[3];
    hEvents[0] = this->m_hNextDepthFrameEvent;
    hEvents[1] = this->m_hNextVideoFrameEvent;
    hEvents[2] = this->m_hNextSkeletonEvent;

    // Main thread loop
    while(true)
    {
        // Wait for an event to be signalled
        int nEventIdx = WaitForMultipleObjects(sizeof(hEvents)/sizeof(hEvents[0]),hEvents,FALSE,100);

        // If the stop event, stop looping and exit
        if( getState() == KINECT_STOP_REQUESTED )
            break;

        // Perform FPS processing
//        t = timeGetTime( );
//        if( this->m_LastFPStime == -1 )
//        {
//            this->m_LastFPStime = t;
//            this->m_LastFramesTotal = this->m_FramesTotal;
//        }
//        dt = t - this->m_LastFPStime;
//        if( dt > 1000 )
//        {
//            this->m_LastFPStime = t;
//            int FrameDelta = this->m_FramesTotal - this->m_LastFramesTotal;
//            this->m_LastFramesTotal = this->m_FramesTotal;
//            SetDlgItemInt( this->m_hWnd, IDC_FPS, FrameDelta,FALSE );
//        }

        // Perform skeletal panel blanking
//        if( this->m_LastSkeletonFoundTime == -1 )
//            this->m_LastSkeletonFoundTime = t;
//        dt = t - this->m_LastSkeletonFoundTime;
//        if( dt > 250 )
//        {
//            if( !this->m_bScreenBlanked )
//            {
//                this->Nui_BlankSkeletonScreen( GetDlgItem( this->m_hWnd, IDC_SKELETALVIEW ) );
//                this->m_bScreenBlanked = true;
//            }
//        }

        // Process signal events
        switch(nEventIdx)
        {
            case 0:
                this->Nui_GotDepthAlert();
                //this->m_FramesTotal++;
                break;

            case 1:
                this->Nui_GotVideoAlert();
                break;

            case 2:
                this->Nui_GotSkeletonAlert();
                break;
        }
    }
    setState( KINECT_INITIALIZED );
}

void KinectDevice::Nui_GotDepthAlert()
{
    const NUI_IMAGE_FRAME * pImageFrame = NULL;

    HRESULT hr = NuiImageStreamGetNextFrame(m_pDepthStreamHandle, 0, &pImageFrame);
    if( FAILED( hr ) )
    {
        return;
    }

    int width;
    int height;
    if( pImageFrame->eResolution == NUI_IMAGE_RESOLUTION_320x240 )
    {
        width = 320;
        height = 240;
    }
    else
    {
        width = 640;
        height = 480;
    }

    NuiImageBuffer * pTexture = pImageFrame->pFrameTexture;
    KINECT_LOCKED_RECT LockedRect;
    pTexture->LockRect( 0, &LockedRect, NULL, 0 );
    BYTE* pBuffer = 0;
    if( LockedRect.Pitch != 0 )
    {
        pBuffer = (BYTE*) LockedRect.pBits;
    }
    else
    {
        OutputDebugString( L"Buffer length of received texture is bogus\r\n" );
    }


    CvMatData img;
    if( pImageFrame->eImageType == NUI_IMAGE_TYPE_DEPTH )
    {
        img = CvMatData::create(width, height, CV_8U, 1);

        // draw the bits to the bitmap
        USHORT* pBufferRun = (USHORT*) pBuffer;
        cv::Mat& mat = img;

        // todo should be faster with memcpy
        for( int y = 0 ; y < height ; y++ )
        {
            for( int x = 0 ; x < width ; x++ )
            {
                // from 12-bit to 16-bit
                USHORT RealDepth = *pBufferRun;

                // transform 13-bit depth information into an 8-bit intensity appropriate
                // for display (we disregard information in most significant bit)
                BYTE l = 255 - (BYTE)(256*RealDepth/0x0fff);
                mat.at<BYTE>(y,x) = l;
                pBufferRun++;
            }
        }
    }
    else if( pImageFrame->eImageType == NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX )
    {
        img = CvMatData::create(width, height, CV_8U, 3);

        // draw the bits to the bitmap
        USHORT* pBufferRun = (USHORT*) pBuffer;
        cv::Mat& mat = img;

        for( int y = 0 ; y < height ; y++ )
        {
            for( int x = 0 ; x < width ; x++ )
            {
                RGBQUAD quad = Nui_ShortToQuad_DepthAndPlayerIndex( *pBufferRun );

                pBufferRun++;
                mat.at<cv::Vec3b>(y,x)[0] = quad.rgbBlue;
                mat.at<cv::Vec3b>(y,x)[1] = quad.rgbGreen;
                mat.at<cv::Vec3b>(y,x)[2] = quad.rgbRed;
            }
        }
    }
    emit newDepthFrame( m_id, img );
    NuiImageStreamReleaseFrame( m_pDepthStreamHandle, pImageFrame );
}

void KinectDevice::Nui_GotVideoAlert()
{
    const NUI_IMAGE_FRAME * pImageFrame = NULL;

    HRESULT hr = NuiImageStreamGetNextFrame( m_pVideoStreamHandle, 0, &pImageFrame );
    if( FAILED( hr ) )
    {
        return;
    }

    NuiImageBuffer* pTexture = pImageFrame->pFrameTexture;
    KINECT_LOCKED_RECT LockedRect;
    pTexture->LockRect( 0, &LockedRect, NULL, 0 );
    if( LockedRect.Pitch != 0 )
    {
        BYTE * pBuffer = (BYTE*) LockedRect.pBits;
        
        // draw the bits to the bitmap
        RGBQUAD* pBufferRun = (RGBQUAD*) pBuffer;
        plv::CvMatData img = plv::CvMatData::create( 640, 480, CV_8UC3 );
        cv::Mat& mat = img;

        for( int y = 0 ; y < 480 ; y++ )
        {
            for( int x = 0 ; x < 640 ; x++ )
            {
                RGBQUAD quad = *pBufferRun;
                pBufferRun++;
                mat.at<cv::Vec3b>(y,x)[0] = quad.rgbBlue;
                mat.at<cv::Vec3b>(y,x)[1] = quad.rgbGreen;
                mat.at<cv::Vec3b>(y,x)[2] = quad.rgbRed;
            }
        }

        emit newVideoFrame( m_id, img );
    }
    else
    {
        OutputDebugString( L"Buffer length of received texture is bogus\r\n" );
    }

    NuiImageStreamReleaseFrame( m_pVideoStreamHandle, pImageFrame );
}

void KinectDevice::Nui_GotSkeletonAlert()
{
    SkeletonFrame sf;
    HRESULT hr = NuiSkeletonGetNextFrame( 0, sf.getNuiSkeletonFramePointer() );
    if( FAILED( hr ) )
    {
        return;
    }

    bool foundSkeleton = false;
    for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
    {
        if( sf.getNuiSkeletonFramePointer()->SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )
        {
            foundSkeleton = true;
        }
    }

    // no skeletons!
    if( !foundSkeleton )
    {
        return;
    }
    else
    {
        sf.setValid();
    }

    // smooth out the skeleton data
    NuiTransformSmooth( sf.getNuiSkeletonFramePointer(), NULL );

    emit newSkeletonFrame( m_id, sf );
}

KinectDevice::KinectState KinectDevice::getState() const
{
    QMutexLocker lock( &m_stateMutex );
	return m_state;
}

void KinectDevice::setState( KinectDevice::KinectState state )
{
    QMutexLocker lock( &m_stateMutex );
    m_state = state;
}

RGBQUAD KinectDevice::Nui_ShortToQuad_DepthAndPlayerIndex( USHORT s )
{
    USHORT RealDepth = (s & 0xfff8) >> 3;
    USHORT Player = s & 7;

    // transform 13-bit depth information into an 8-bit intensity appropriate
    // for display (we disregard information in most significant bit)
    BYTE l = 255 - (BYTE)(256*RealDepth/0x0fff);

    RGBQUAD q;
    q.rgbRed = q.rgbBlue = q.rgbGreen = 0;

    switch( Player )
    {
    case 0:
        q.rgbRed = l / 2;
        q.rgbBlue = l / 2;
        q.rgbGreen = l / 2;
        break;
    case 1:
        q.rgbRed = l;
        break;
    case 2:
        q.rgbGreen = l;
        break;
    case 3:
        q.rgbRed = l / 4;
        q.rgbGreen = l;
        q.rgbBlue = l;
        break;
    case 4:
        q.rgbRed = l;
        q.rgbGreen = l;
        q.rgbBlue = l / 4;
        break;
    case 5:
        q.rgbRed = l;
        q.rgbGreen = l / 4;
        q.rgbBlue = l;
        break;
    case 6:
        q.rgbRed = l / 2;
        q.rgbGreen = l / 2;
        q.rgbBlue = l;
        break;
    case 7:
        q.rgbRed = 255 - ( l / 2 );
        q.rgbGreen = 255 - ( l / 2 );
        q.rgbBlue = 255 - ( l / 2 );
    }

    return q;
}

void KinectDevice::threadFinished()
{
    emit deviceFinished( m_id );
}


