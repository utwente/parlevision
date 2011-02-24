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

#include <QObject>
#include <stdexcept>
#include <QImage>
#include <plvcore/CvMatData.h>

namespace plvgui
{
    class ImageConversionException : public std::runtime_error
    {
    public:
        ImageConversionException( const std::string& why ) :
                std::runtime_error( why ) {}
    };

    class ImageConverter : public QObject
    {
        Q_OBJECT

    public:
        ImageConverter( QObject* parent = 0 );
        ~ImageConverter();

        /** Starts converting this image to a QImage.
          * This call will return immediately as converting is done asynchronously.
          * The id can be used to track multiple images in flight by the
          * calling code, but nothing is done with the id here and images
          * can be converted out of order.
          * @emits converted(QImage*, id) when converting has finished
          */
        void convertCvMatData( const plv::CvMatData& data, int id=0 );
        void convertCvMatDataList( const QList<plv::CvMatData>& data, int id=0 );

    private:
        void convert( const plv::CvMatData& mat, int id );
        void convertList( const QList<plv::CvMatData>& data, int id );

        /** Converts an OpenCV iplImage to a QImage.
          * @throw ImageConversionException when conversion fails.
          */
        static QImage cvMatToQImage( const cv::Mat mat )
                throw( ImageConversionException );

    signals:
        /** Emitted when converting is done.
          * The contained image might not be valid if an error occurred
          */
        void converted( QImage img, int id );
        void convertedList( QList<QImage> img, int id );
    };
}


#endif // IMAGECONVERTER_H
