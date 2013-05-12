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

#ifndef SAVEIMAGETOFILE_H
#define SAVEIMAGETOFILE_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/InputPin.h>
#include <plvcore/Enum.h>
#include <plvcore/CvMatData.h>
#include <QList>

namespace plv
{
    class OpenCVImage;
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    /**
        This proccessor allows one or multiple CvMatData based images to be saved
        to the filesystem.

        The user can enter a directory in the configuration panel or connect a processor to the directory pin.

        The filename is equal to the filename on the filename pin or if that is
        not connected the filename will be the serial number of the input image.
        If there are multiple images the filename will have an extra number attached to it.

        For instance:
        - no filename connected, single image %serial%.%extension%
        - no filename connected, multiple images %serial%_%number%.%extension%

        The extension is dependend on the output format selected
        OpenCV provides 6 different file formats to save the image to:
        - BMP
        - JPG
        - PNG
        - PBM
        - SR
        - TIFF
        The default is set to BMP because it is lossless and is fast.
     */
    class SaveImageToFile : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( SaveImageToFile )

        Q_CLASSINFO("author", "Richard Loos")
        Q_CLASSINFO("name", "Save Image")
        Q_CLASSINFO("description", "A processor that takes an input and saves it.")

        Q_PROPERTY( QString directory READ getDirectory WRITE setDirectory NOTIFY directoryChanged )
        Q_PROPERTY( plv::Enum fileFormat READ getFileFormat WRITE setFileFormat NOTIFY fileFormatChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_PROCESSOR

    public:
        /** Constructor/Destructor */
        SaveImageToFile();
        virtual ~SaveImageToFile();

        /** property methods */
        QString getDirectory() const;
        plv::Enum getFileFormat() const;

        /** checks if directory exists */
        virtual bool init();

    signals:
        void directoryChanged(QString newValue);
        void fileFormatChanged(plv::Enum newValue);

    public slots:
        void setDirectory(QString s);
        void setFileFormat(plv::Enum e);

    private:
        plv::InputPin< plv::CvMatData>*         m_inputImage;
        plv::InputPin< QList<plv::CvMatData> >* m_inputImages;
        plv::InputPin<bool>* m_inputTrigger;
        plv::InputPin<QString>* m_inputFilename;
        plv::InputPin<QString>* m_inputPath;

        QString   m_directory;  /** directory to store the image. Default is empty */
        plv::Enum m_fileFormat; /** The file extention and thus the format in which the data is stored. */

        /** Additional properties */
        QString m_fileExt;   /** The filename extension selected through m_fileFormat. */
    };
}

#endif // SAVEIMAGETOFILE_H
