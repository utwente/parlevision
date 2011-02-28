
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

#include <QMutexLocker>
#include <QDebug>

#include "ImageDirectoryProducer.h"

#include <opencv/highgui.h>
#include <plvcore/CvMatDataPin.h>
#include <QDir>

using namespace plv;
using namespace plvopencv;

ImageDirectoryProducer::ImageDirectoryProducer() : m_idx(0)
{
    m_imgOutputPin = createCvMatDataOutputPin("image_output", this );
    m_fileNameOutputPin  = createOutputPin<QString>("file name", this );
    m_filePathOutputPin  = createOutputPin<QString>("file path", this );
}

ImageDirectoryProducer::~ImageDirectoryProducer()
{
}

QString ImageDirectoryProducer::getDirectory()
{
    QMutexLocker lock( m_propertyMutex );
    return m_directory;
}

void ImageDirectoryProducer::setDirectory(const QString& directory)
{
    //replace all '\' characters with '/' characters
    QString path = directory;
    path.replace('\\','/');

    //validate the directory
    QDir dir(directory);
    if( dir.exists() )
    {
        QMutexLocker lock( m_propertyMutex );
        m_directory = path;
        qDebug() << "New directory selected:" << m_directory;
    }
}

bool ImageDirectoryProducer::init()
{
    QDir dir(m_directory);
    if( !dir.exists() )
    {
        setError( PlvPipelineInitError, "Directory is invalid");
        return false;
    }
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Readable);
    dir.setSorting(QDir::Name);

    QStringList filters;
    filters << "*.jpg" << "*.jpeg" << "*.bmp" << "*.sr" << "*.png" << "*.pdm";
    dir.setNameFilters(filters);

    m_entryInfoList = dir.entryInfoList();
    m_idx = 0;
    return true;
}

bool ImageDirectoryProducer::deinit() throw ()
{
    m_entryInfoList.clear();
    m_idx = 0;
    return true;
}

bool ImageDirectoryProducer::readyToProduce() const
{
    return m_idx < m_entryInfoList.size();
}

bool ImageDirectoryProducer::produce()
{
    QFileInfo fileInfo = m_entryInfoList.at(m_idx);

    //load the image
    const std::string& path = fileInfo.absoluteFilePath().toStdString();
    cv::Mat image = cv::imread(path, CV_LOAD_IMAGE_UNCHANGED);
    if(image.data == 0)
    {
        setError( PlvPipelineRuntimeError, tr("Failed to load image %1.").arg(fileInfo.absolutePath()));
        return false;
    }

    m_imgOutputPin->put( CvMatData(image) );
    m_fileNameOutputPin->put( fileInfo.fileName() );
    m_filePathOutputPin->put( fileInfo.absolutePath() );
    m_idx++;

    return true;
}
