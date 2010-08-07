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

#include "CameraProducer.h"

#include <QMutexLocker>
#include <QDebug>

#include "OpenCVCamera.h"
#include "Pin.h"

using namespace plv;

#define OUTPUT_PIN_NAME "output"

CameraProducer::CameraProducer() :
        m_camera( new OpenCVCamera() ),
        m_lastProcessedId( 0 )
{
    // we have one output pin
    m_outputPin = new OutputPin<OpenCVImage>(OUTPUT_PIN_NAME, this );
    addOutputPin( m_outputPin.getPtr() );

    // connect the camera to this camera producer using Qt's signals
    // and slots mechanism.
    connect( m_camera.getPtr(),
             SIGNAL( newFrame( RefPtr<Data> ) ),
             this,
             SLOT( newFrame( RefPtr<Data> ) ) );
}

CameraProducer::~CameraProducer()
{
    m_camera->release();
}

CameraProducer::CameraProducer(const CameraProducer& other):
        PipelineProducer(other),
        m_camera(other.m_camera),
        m_lastProcessedId( other.m_lastProcessedId )
{
    // we have one output pin
    m_outputPin = new OutputPin<OpenCVImage>(OUTPUT_PIN_NAME, this );
    addOutputPin( m_outputPin.getPtr() );

    // connect the camera to this camera producer using Qt's signals
    // and slots mechanism.
    connect( m_camera.getPtr(),
             SIGNAL( newFrame( RefPtr<Data> ) ),
             this,
             SLOT( newFrame( RefPtr<Data> ) ) );
}

void CameraProducer::process()
{
    QMutexLocker lock(&m_frameMutex);
    assert( m_lastFrame.isNotNull() );
    m_outputPin->put( m_lastFrame.getPtr() );

    // clear last frame so we do not process this image twice
    m_lastFrame.set( 0 );
}

void CameraProducer::newFrame( RefPtr<Data> frame )
{
    QMutexLocker lock(&m_frameMutex);
    m_lastFrame = ref_ptr_dynamic_cast<OpenCVImage>( frame );
    //m_frameReady.wakeAll();
}

void CameraProducer::init() throw (PipelineException)
{
}

void CameraProducer::start() throw (PipelineException)
{
    if( !m_camera->init() )
    {
        throw PipelineException("Camera failed to initialise");
    }

    m_camera->setDimensions( 640, 480 );

    m_frameMutex.lock();

    m_camera->start();

    // Block until we receive the first frame from the camera,
    // indicating it has finished starting up.
    // this will let this thread wait on the event
    // and unlocks the mutex so no deadlock is created
    //m_frameReady.wait(&m_frameMutex);

    // we have woken up
    // mutex was relocked by the condition
    // unlock it here
    m_frameMutex.unlock();
}

void CameraProducer::stop() throw (PipelineException)
{
    m_camera->release();
}

//bool CameraProducer::isBootstrapped() const
//{
//    // TODO
//    return true;
//}

bool CameraProducer::isReadyForProcessing() const
{
    return( m_lastFrame.isNotNull() );
}
