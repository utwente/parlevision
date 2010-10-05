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

#include <plvcore/Pin.h>

#include "OpenCVCamera.h"

using namespace plv;
using namespace plvopencv;

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
             SIGNAL( newFrame( plv::RefPtr<plv::Data> ) ),
             this,
             SLOT( newFrame( plv::RefPtr<plv::Data> ) ) );
}

CameraProducer::~CameraProducer()
{

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

void CameraProducer::newFrame( plv::RefPtr<plv::Data> frame )
{
    QMutexLocker lock(&m_frameMutex);
    m_lastFrame = ref_ptr_dynamic_cast<OpenCVImage>( frame );
    //m_frameReady.wakeAll();
}

void CameraProducer::init()
{
    if( !m_camera->init() )
    {
        throw PlvException("Camera failed to initialise");
    }
    m_camera->setDimensions( 640, 480 );
}

void CameraProducer::deinit() throw()
{
    m_camera->release();
}

void CameraProducer::start()
{
    m_camera->start();
}

void CameraProducer::stop()
{
    m_camera->pause();
}

bool CameraProducer::isReadyForProcessing() const
{
    return( m_lastFrame.isNotNull() );
}
