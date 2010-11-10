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

#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QMutex>
#include <plvcore/PipelineProducer.h>
#include <plvcore/Pin.h>
#include <plvcore/OpenCVImage.h>

namespace plvopencv
{
    class ImageLoader : public plv::PipelineProducer
    {
        Q_OBJECT
        Q_DISABLE_COPY( ImageLoader )
        Q_CLASSINFO("author", "Niek Hoeijmakers")
        Q_CLASSINFO("name", "ImageLoader")
        Q_CLASSINFO("description", "This pipeline producer loads an image with the given path to the image."
                    "And each time the process is called it checks if there has been image data loaded and"
                    "sends the image or not.");

        Q_PROPERTY( QString filename READ getFilename WRITE setFilename NOTIFY filenameChanged )
        Q_PROPERTY( QString directory READ getDirectory WRITE setDirectory NOTIFY directoryChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        ImageLoader();
        virtual ~ImageLoader();

        /** @returns true if a new frame is available */
        bool isReadyForProcessing() const;

        /** property methods **/
        QString getFilename() { return m_filename; }
        QString getDirectory() { return m_directory; }

    signals:
        void filenameChanged(QString newValue);
        void directoryChanged(QString newValue);

    protected:
        plv::RefPtr< plv::OpenCVImage > m_loadedImage;
        plv::RefPtr< plv::OutputPin< plv::OpenCVImage > > m_outputPin;

        //int m_lastProcessedId;

        QMutex m_processMutex;
        //QWaitCondition m_frameReady;

    public slots:
        /**
          * This method sets the value to a new filename and then checks if the
          * name is valid and allow this producer to transmit loaded images.
          */
        void setFilename(QString filename);
        void setDirectory(QString directory);

    private:
        //variables
        QString     m_filename;  //The filename to save the image to.
        QString     m_directory; //Directory to store the image.
        bool        m_isReady;   //Checks whether there is an image loaded and ready for sending.

        //methods
        /**
          * This method checks whether the extension of the filename is one of the
          * accepted extensions for images by OpenCV. See:
          * http://opencv.willowgarage.com/documentation/c/reading_and_writing_images_and_video.html
          * @require filename.length >= 4.
          */
        bool validateExtension(QString filename) const;
        /**
          * This method attemps to load a new image with directory and filename. It
          * returns true if it succesfully loaded a new image and false if the load-
          * ed image is NULL.
          */
        bool ObtainImage(QString directory, QString filename);

    };//class ImageLoader

}//namespace plvopencv

#endif // IMAGELOADER_H
