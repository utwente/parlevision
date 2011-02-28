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

#include "ImageProducer.h"

#include <QMutexLocker>
#include <QDebug>

#include <opencv/highgui.h>
#include <string>
#include <plvcore/CvMatDataPin.h>
#include <QDir>

using namespace plv;
using namespace plvopencv;

ImageProducer::ImageProducer()
{
    //create the output pin
    m_outputPin = createCvMatDataOutputPin("image_output", this );
    m_outputPin->addAllChannels();
    m_outputPin->addAllDepths();
}

ImageProducer::~ImageProducer()
{
}

void ImageProducer::setFilename(const QString& filename)
{
    //Validate the filename
    if( filename.length() < 4 || !ImageProducer::validateExtension(filename) )
    {
        qDebug() << tr("Invalid filename %1.").arg(filename);
        return;
    }

    QMutexLocker lock( m_propertyMutex );
    m_filename = filename;
    qDebug() << "New filename selected: " << m_filename;
    return;
}

QString ImageProducer::getFilename()
{
    QMutexLocker lock( m_propertyMutex );
    return m_filename;
}

void ImageProducer::setDirectory(const QString& directory)
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

QString ImageProducer::getDirectory()
{
    QMutexLocker lock( m_propertyMutex );
    return m_directory;
}

bool ImageProducer::validateExtension(const QString& filename)
{
    QStringRef ref = filename.midRef(filename.length()-3,3);

    if(ref.compare(".sr",Qt::CaseInsensitive) == 0)
    {
        return true;
    }

    //if filename is smaller than 5 it can not be valid anymore
    if(filename.length() < 5) return false;

    ref = filename.midRef(filename.length()-4,4);

    if( ref.compare(".bmp",Qt::CaseInsensitive) == 0 ||
        ref.compare(".dib",Qt::CaseInsensitive) == 0 ||
        ref.compare(".jpg",Qt::CaseInsensitive) == 0 ||
        ref.compare(".jpe",Qt::CaseInsensitive) == 0 ||
        ref.compare(".png",Qt::CaseInsensitive) == 0 ||
        ref.compare(".pbm",Qt::CaseInsensitive) == 0 ||
        ref.compare(".pgm",Qt::CaseInsensitive) == 0 ||
        ref.compare(".ppm",Qt::CaseInsensitive) == 0 ||
        ref.compare(".ras",Qt::CaseInsensitive) == 0 ||
        ref.compare(".tif",Qt::CaseInsensitive) == 0 )
    {
        return true;
    }

    //if filename is smaller than 6 it can not be valid anymore
    if(filename.length() < 6) return false;

    ref = filename.midRef(filename.length()-5,5);
    if( ref.compare(".jpeg",Qt::CaseInsensitive) == 0 ||
        ref.compare(".tiff",Qt::CaseInsensitive) == 0 )
    {
        return true;
    }

    //none of the cases matched.
    return false;
}

bool ImageProducer::init()
{
    //build the path
    QString path =  m_directory;
    if(!path.endsWith('/')) path.append('/');
    path.append(m_filename);

    //load the image
    cv::Mat image = cv::imread(path.toStdString(), CV_LOAD_IMAGE_UNCHANGED);
    if(image.data == 0)
    {
        setError( PlvPipelineInitError, tr("Failed to load image %1.").arg(path));
        return false;
    }
    m_loadedImage = image;
    return true;
}

bool ImageProducer::deinit() throw()
{
    m_loadedImage = CvMatData();
    return true;
}

bool ImageProducer::readyToProduce() const
{
    return m_loadedImage.isValid();
}

bool ImageProducer::produce()
{
    m_outputPin->put( m_loadedImage );
    return true;
}
