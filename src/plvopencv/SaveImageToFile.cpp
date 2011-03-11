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

#include <QDebug>

#include "SaveImageToFile.h"
#include <plvcore/CvMatData.h>

#include <plvcore/Pin.h>
#include <plvcore/CvMatDataPin.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <QDir>

using namespace plv;
using namespace plvopencv;

/** default directory for saving data to */
//TODO replace with global config
#ifdef Q_OS_WIN
#   define SAVEIMAGETOFILE_DEFAULT_DIR "c:/Temp/"
#elif defined(Q_OS_DARWIN)
#   define SAVEIMAGETOFILE_DEFAULT_DIR "/tmp/"
#elif defined(Q_OS_UNIX)
#   define SAVEIMAGETOFILE_DEFAULT_DIR "/tmp/"
#endif

enum ImageFormat {
    BMP,
    JPG,
    PNG,
    PBM,
    SR,
    TIFF
};

/**
 * Constructor.
 */
SaveImageToFile::SaveImageToFile() :
        m_directory(SAVEIMAGETOFILE_DEFAULT_DIR),
        m_fileExt(".bmp")
{
    m_inputImage    = createInputPin<CvMatData>("image", this, IInputPin::CONNECTION_OPTIONAL );
    m_inputImages   = createInputPin< QList<CvMatData> >("image list", this, IInputPin::CONNECTION_OPTIONAL );
    m_inputFilename = createInputPin<QString>("name", this, IInputPin::CONNECTION_OPTIONAL );
    m_inputPath     = createInputPin<QString>("path", this, IInputPin::CONNECTION_OPTIONAL );

    m_fileFormat.add("Windows Bitmap - *.bmp", BMP);
    m_fileFormat.add("JPEG Files - *.jpg", JPG);
    m_fileFormat.add("Portable Network Graphics - *.png", PNG);
    m_fileFormat.add("Portable Image Format - *.pbm", PBM);
    m_fileFormat.add("Sun Rasters - *.sr", SR);
    m_fileFormat.add("TIFF Files - *.tiff", TIFF);
}

SaveImageToFile::~SaveImageToFile()
{
}

bool SaveImageToFile::init()
{
    //replace all '\' characters with '/' characters
    m_directory = m_directory.replace('\\','/');
    if(!m_directory.endsWith('/'))
    {
        m_directory.append('/');
    }
    emit directoryChanged(m_directory);

    QDir dir(m_directory);
    return dir.exists();
}

QString SaveImageToFile::getDirectory() const
{
    QMutexLocker lock(m_propertyMutex);
    return m_directory;
}

plv::Enum SaveImageToFile::getFileFormat() const
{
    QMutexLocker lock(m_propertyMutex);
    return m_fileFormat;
}

/**
 * Deals with the change of the directory property.
 */
void SaveImageToFile::setDirectory(QString s)
{
    if( getState() > UNDEFINED )
        return;

    QMutexLocker lock(m_propertyMutex);
    m_directory = s;
    emit directoryChanged(m_directory);
}

/**
 * Deals with the change of the fileFomat property.
 */
void SaveImageToFile::setFileFormat(plv::Enum e)
{
    m_fileFormat = e;

    //clear the additional properties
    switch(e.getSelectedIndex())
    {
    case JPG: //JPEG Files
        m_fileExt = ".jpg";
        break;
    case PNG: //Portable Network Graphics
        m_fileExt = ".png";
        break;
    case PBM: //Portable Image Format
        m_fileExt = ".pbm";
        break;
    case SR: //Sun Rasters
        m_fileExt = ".sr";
        break;
    case TIFF: //TIFF Files
        m_fileExt = ".tiff";
        break;
    case BMP: // Bitmap files
        m_fileExt = ".bmp";
        break;
    default: //Defaulting to Windows bitmap
        m_fileExt = ".bmp";
        break;
    }
    emit fileFormatChanged(m_fileFormat);
}

bool SaveImageToFile::process()
{
    QList<plv::CvMatData> images;

    if( m_inputImage->isConnected() )
    {
        images.append(m_inputImage->get());
    }

    if( m_inputImages->isConnected() )
    {
        images.append(m_inputImages->get());
    }

    QString path;
    if( m_inputPath->isConnected() )
    {
        path = m_inputPath->get();
        path = path.replace('\\','/');
        if(!path.endsWith('/'))
        {
            path.append('/');
        }
        QDir dir(path);
        if( !dir.exists() )
        {
            qWarning() << "plv::opencv::SaveImageToFile::process() invalid path given to processor, ignoring.";
            return true;
        }
    }
    else
    {
        path = m_directory;
    }

    QString filenameBegin;

    if( m_inputFilename->isConnected() )
    {
        filenameBegin = m_inputFilename->get();
        if( !filenameBegin.isEmpty() )
        {
            QStringList parts = filenameBegin.split(".");
            filenameBegin = parts.at(0);
        }
        else
        {
            qWarning() << "plv::opencv::SaveImageToFile::process() empty string received";
            filenameBegin = QString::number(this->getProcessingSerial());
        }
    }
    else
    {
        filenameBegin = QString::number(this->getProcessingSerial());
    }

    bool success = false;
    bool multi_image = images.size() > 1;
    for(int i=0; i<images.size(); ++i)
    {
        QString filename;
        if(multi_image)
            filename = QString("%1%2_%3%4").arg(path).arg(filenameBegin).arg(i).arg(m_fileExt);
        else
            filename = QString("%1%2.%3").arg(path).arg(filenameBegin).arg(m_fileExt);

        QFile file(filename);
        if( !file.exists() )
        {
            if( file.open(QIODevice::WriteOnly) )
            {
                file.close();
                const cv::Mat& mat = images.at(i);

                // The function imwrite saves the image to the specified file.
                // The image format is chosen based on the filename extension,
                // see imread for the list of extensions.
                // Only 8-bit (or 16-bit in the case of PNG, JPEG 2000 and TIFF) single-channel
                // or 3-channel (with ‘BGR’ channel order) images can be saved using this function.
                // If the format, depth or channel order is different, use Mat::convertTo ,
                // and cvtColor to convert it before saving, or use the universal XML I/O functions
                // to save the image to XML or YAML format.

                success = cv::imwrite(filename.toStdString(), mat);
            }
        }

        if( !success )
        {
            qWarning() << "plv::opencv::SaveImageToFile::process() failed to save image " << filename;
        }
    }
    return true;
}
