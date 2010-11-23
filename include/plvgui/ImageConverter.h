/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvgui module of ParleVision.
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

#ifndef IMAGECONVERTER_H
#define IMAGECONVERTER_H

#include <plvcore/RefPtr.h>
#include <QObject>
#include <opencv/cv.h>
#include <stdexcept>
#include <QImage>

namespace plv
{
    class OpenCVImage;
}

namespace plvgui
{
    class ImageConversionException : public std::runtime_error
    {
    public:
        ImageConversionException( const std::string& why ) :
                std::runtime_error( why ) {}
    };

    class ImageConverter : public QObject, public plv::RefCounted
    {
        Q_OBJECT

    public:

        /** Starts converting this image to a QImage.
          * This call will return immediately
          * as converting is done asynchronously.
          * @emits converted(QImage*) when converting has finished;
          */
        void convert_OpenCVImage( plv::RefPtr<plv::OpenCVImage> img );

    private:
        static ImageConverter* m_instance;
        void convert(plv::RefPtr<plv::OpenCVImage> img);

        /** Converts an OpenCV iplImage to a QImage.
          * @throw ImageConversionException when conversion fails.
          */
        static QImage iplImageToQImage( const IplImage* img )
                throw( ImageConversionException );

    signals:
        /** Emitted when converting is done.
          * The contained image might not be valid if an error occurred
          */
        void converted( QImage img );
    };
}


#endif // IMAGECONVERTER_H
