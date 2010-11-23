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

#include "ImageLoader.h"

#include <plvcore/OpenCVImage.h>

#include <QMutexLocker>
#include <QDebug>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <string>

#include <plvcore/Pin.h>

using namespace plv;
using namespace plvopencv;

//--- Constructor & Destructor ------------------------------------------------
ImageLoader::ImageLoader()
{
    //create the output pin
    m_outputPin = new OutputPin<OpenCVImage>("image_output", this );
    addOutputPin( m_outputPin.getPtr() );

    m_directory = "c:/";
    m_filename = "img1001.bmp";
    m_isReady = false;

    //Attempt to load the image
    //m_isReady = ObtainImage(m_directory, m_filename);
}

ImageLoader::~ImageLoader()
{
}

//--- ImageLoader methods -----------------------------------------------------
void ImageLoader::setFilename(QString filename)
{
    m_filename.clear();
    m_filename = filename;

    qDebug() << "New filename selected:" << m_filename;

    //Validate the filename
    if(m_filename.length() >= 4 && validateExtension(m_filename))
    {
        m_isReady = ObtainImage(m_directory, m_filename);
    }
    else
    {
        qDebug() << "New filename not valid!";
        m_isReady = false;
    }


    emit( filenameChanged(m_filename) );
}

void ImageLoader::setDirectory(QString directory)
{
    m_directory.clear();
    //replace all '\' characters with '/' characters
    m_directory = directory.replace('\\','/');

    qDebug() << "New directory selected:" << m_directory;

    //validate the directory
    if(m_directory.length() >= 3){
        m_isReady = ObtainImage(m_directory, m_filename);
    }
    else{
        qDebug() << "New directory not a valid directory!";
        m_isReady = false;
    }

    emit( directoryChanged(m_directory));
}

bool ImageLoader::validateExtension(QString filename) const
{
    /**
      * Assumption OS does not allow an empty filename (excluding exten-
      * sion) to be used for files, thus is it pointless to provide only
      * the extension to the file.
      */

    QStringRef ref = filename.midRef(filename.length()-3,3);

    if(ref.compare(".sr",Qt::CaseInsensitive) == 0)
    {
        return true;
    }

    //if filename is smaller than 5 it can not be valid anymore
    if(filename.length() < 5) return false;

    ref.clear();
    ref = filename.midRef(filename.length()-4,4);
    if(ref.compare(".bmp",Qt::CaseInsensitive) == 0 ||
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
    if(filename.length() < 6)
        return false;

    ref.clear();
    ref = filename.midRef(filename.length()-5,5);
    if( ref.compare(".jpeg",Qt::CaseInsensitive) == 0 ||
        ref.compare(".tiff",Qt::CaseInsensitive) == 0 )
    {
        return true;
    }

    //none of the cases matched.
    return false;
}

bool ImageLoader::ObtainImage(QString directory, QString filename){
    QMutexLocker lock(&m_processMutex);

    //build the path
    QString path = directory;
    if(!path.endsWith('/')) path.append('/');
    path.append(filename);
    std::string c_path = path.toStdString();

    m_loadedImage = 0;

    //load the image
    const IplImage* image = cvLoadImage(c_path.c_str(), CV_LOAD_IMAGE_UNCHANGED );
    if(image != 0)
    {
        m_loadedImage = OpenCVImageFactory::instance()->getFromBuffer( image );

        //check if the newly loaded image is null or not.
        if(!m_loadedImage->isNull())
        {
            qDebug() << "New image is loaded from path: " << path;
            return true;
        }
    }

    qDebug() << "New image could not be loaded from path: " << path;
    return false;
}

//--- Inherited methods -------------------------------------------------------

void ImageLoader::process()
{
    QMutexLocker lock(&m_processMutex);

    //If a propper image has been loaded send it over the pipe.
    if(m_isReady)
    {
        m_outputPin->put( m_loadedImage.getPtr() );
    }

}

void ImageLoader::init()
{
    ///
    m_isReady = ObtainImage(m_directory, m_filename);
}

void ImageLoader::deinit() throw() {}
void ImageLoader::start() {}
void ImageLoader::stop() {}

bool ImageLoader::isReadyForProcessing() const
{
    return( m_loadedImage.isNotNull() );
}
