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
#include <QStringBuilder>
#include <QVariant>

#include <plvcore/CvMatDataPin.h>
#include "OpenCVCamera.h"


using namespace plv;
using namespace plvopencv;

CameraProducer::CameraProducer() :
        m_camera( new OpenCVCamera() ),
        m_cameraId( 0 ),
        m_width( 640 ),
        m_height( 480 ),
        m_lastProcessedId( 0 ),
        m_maxBufferSize(3)
{
    // we have one output pin
    m_outputPin = createCvMatDataOutputPin( "output", this );

    // supports all types of images
    m_outputPin->addAllChannels();
    m_outputPin->addAllDepths();

    // connect the camera to this camera producer using Qt's signals
    // and slots mechanism.
    connect( m_camera.getPtr(),
             SIGNAL( newFrame( plv::CvMatData ) ),
             this,
             SLOT( newFrame( plv::CvMatData ) ) );
}

CameraProducer::~CameraProducer()
{
}

bool CameraProducer::produce()
{
    QMutexLocker lock(&m_frameMutex);
    m_outputPin->put( m_frames.takeFirst() );
    return true;
}

void CameraProducer::newFrame( plv::CvMatData frame )
{
    QMutexLocker lock(&m_frameMutex);
    m_frames.append(frame);
    if( m_frames.size() > m_maxBufferSize )
        m_frames.removeFirst();

}

bool CameraProducer::init()
{
    if( !m_camera->init(m_cameraId) )
    {
        setError( PlvInitError, tr("Camera with id %1 failed to initialise").arg(m_cameraId) );
        return false;
    }
    m_camera->setDimensions(m_width, m_height);
    return true;
}

bool CameraProducer::deinit() throw()
{
    m_camera->release();
    return true;
}

bool CameraProducer::start()
{
    m_camera->start();
    return true;
}

bool CameraProducer::stop()
{
    m_camera->pause();
    return true;
}

bool CameraProducer::readyToProduce() const
{
    QMutexLocker lock(&m_frameMutex);
    return( !m_frames.isEmpty() );
}
