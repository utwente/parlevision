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

using namespace plv;
using namespace plvopencv;

/** default directory for saving data to */
//TODO replace with global config
#ifdef Q_OS_WIN
#   define SAVEIMAGETOFILE_DEFAULT_DIR "c:/tmp/"
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
        m_directory(""),
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
    //replace all '\' characters with '/' characters
    m_directory = s.replace('\\','/');
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
    return true;
}
