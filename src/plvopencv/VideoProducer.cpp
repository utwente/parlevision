/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvopencv module of ParleVision.
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

#include "VideoProducer.h"

#include <QMutexLocker>
#include <QDebug>

#include <opencv/highgui.h>
#include <string>
#include <plvcore/CvMatDataPin.h>
#include <QDir>

using namespace plv;
using namespace plvopencv;

VideoProducer::VideoProducer() :
    m_filename(""), m_directory(""), m_frameCount(0), m_posMillis(0), m_ratio(0), m_fps(0),
    m_capture( new cv::VideoCapture() )
{
    //create the output pin
    m_outputPin = createCvMatDataOutputPin("image_output", this );
    m_outputPin->addAllChannels();
    m_outputPin->addAllDepths();

    m_outFrameCount = createOutputPin<int>("frame count", this);
    m_outPositionMillis = createOutputPin<long>("milli seconds", this);
    m_outRatio = createOutputPin<double>("ratio", this);
    m_outFps = createOutputPin<int>("fps", this);
}

VideoProducer::~VideoProducer()
{
}

void VideoProducer::setFilename(const QString& filename)
{
    //Validate the filename
    if( filename.length() < 4 || !VideoProducer::validateExtension(filename) )
    {
        qDebug() << tr("Invalid filename %1.").arg(filename);
        return;
    }

    QMutexLocker lock( m_propertyMutex );
    m_filename = filename;
    qDebug() << "New filename selected: " << m_filename;
    return;
}

QString VideoProducer::getFilename()
{
    QMutexLocker lock( m_propertyMutex );
    return m_filename;
}

void VideoProducer::setDirectory(const QString& directory)
{
    //replace all '\' characters with '/' characters
    QString directoryCopy = directory;
    directoryCopy.replace('\\','/');

    //validate the directory
    QDir dir(directory);
    if( dir.exists() )
    {
        QMutexLocker lock( m_propertyMutex );
        m_directory = directoryCopy;
        qDebug() << "New directory selected:" << m_directory;
    }
}

QString VideoProducer::getDirectory()
{
    QMutexLocker lock( m_propertyMutex );
    return m_directory;
}

bool VideoProducer::validateExtension(const QString& filename)
{
    // TODO
    return true;
}

bool VideoProducer::init()
{
    //build the path
    QString path =  m_directory;
    if(!path.endsWith('/')) path.append('/');
    path.append(m_filename);

    if(!m_capture->open(path.toStdString()))
    {
        setError(PlvPipelineInitError, tr("Failed to open video %1.").arg(path));
        return false;
    }

    m_frameCount = (int)m_capture->get(CV_CAP_PROP_FRAME_COUNT);
    m_posMillis = (long)m_capture->get(CV_CAP_PROP_POS_MSEC);
    m_ratio = m_capture->get(CV_CAP_PROP_POS_AVI_RATIO);
    m_fps = (int)m_capture->get(CV_CAP_PROP_FPS);
    return true;
}

bool VideoProducer::deinit() throw()
{
    m_capture->release();
    return true;
}

bool VideoProducer::readyToProduce() const
{
    return m_capture->grab();
}

bool VideoProducer::produce()
{
    if( !m_capture->retrieve(m_frame, 0) )
    {
        setError(PlvPipelineInitError, tr("Failed to grab frame"));
        return false;
    }
    CvMatData d = CvMatData::create(m_frame.properties());
    m_frame.copyTo(d);
    m_outputPin->put(d);

    m_frameCount = (long)m_capture->get(CV_CAP_PROP_FRAME_COUNT);
    m_posMillis = (long)m_capture->get(CV_CAP_PROP_POS_MSEC);
    m_ratio = m_capture->get(CV_CAP_PROP_POS_AVI_RATIO);
    m_fps = (int)m_capture->get(CV_CAP_PROP_FPS);

    m_outFrameCount->put(m_frameCount);
    m_outPositionMillis->put(m_posMillis);
    m_outRatio->put(m_ratio);
    m_outFps->put(m_fps);

    return true;
}
