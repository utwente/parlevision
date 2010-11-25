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

#include "ImageConverter.h"

#include <plvcore/OpenCVImage.h>

#include <QImage>
#include <QPainter>
#include <QString>
#include <QtConcurrentRun>

using namespace plvgui;
using namespace plv;

ImageConverter* ImageConverter::m_instance = 0;

void ImageConverter::convert_OpenCVImage( RefPtr<OpenCVImage> img )
{
    QtConcurrent::run(this, &ImageConverter::convert, img);
}

void ImageConverter::convert( RefPtr<OpenCVImage> imgdata )
{
    try
    {
        const IplImage* image = imgdata->getImage();
        QImage qimage = iplImageToQImage( image );
        emit( converted( qimage ) );
    }
    catch( ImageConversionException& e )
    {
        QString msg = QString("IplImageConverter failed to convert image with error: %1")
                      .arg( e.what() );

        QSize size( imgdata->getWidth(), imgdata->getHeight() );
        QImage qimage( size, QImage::Format_ARGB32_Premultiplied );
        qimage.fill( 0 );
        QPainter painter(&qimage);
        painter.fillRect( qimage.rect(), QColor( Qt::white ));
        painter.setPen( Qt::black );
        QFont font = painter.font();
        font.setPointSize( 16 );
        painter.setFont( font );
        painter.drawText( qimage.rect(), Qt::AlignCenter | Qt::TextWordWrap, msg);
        painter.end();
        emit( converted( qimage ) );
    }
}

QImage ImageConverter::iplImageToQImage( const IplImage* img )
        throw( ImageConversionException )
{
    QImage qimg;
    QString errStr;

    switch( img->depth )
    {
    case IPL_DEPTH_8U:
        if( img->nChannels == 1 )
        {
            // OpenCV image is stored with one byte grey pixel.
            // Convert it to an 8 bit indexed QImage.
            // We add the index at the function exit
            qimg = QImage( img->width, img->height, QImage::Format_Indexed8 );
            const uchar* cvImgData = reinterpret_cast<const uchar*>( img->imageData );

            for( int y = 0; y < img->height; ++y )
            {
                // Copy line by line
                uchar* scanline = qimg.scanLine( y );
                memcpy( scanline, cvImgData, img->width );
                cvImgData += img->widthStep;
            }
        }
        else if( img->nChannels == 3 )
        {
            // image is stored with 3 channels and one byte per pixel
            // per channel. Convert to RGB32 which uses 4 bytes per pixel
            int cvLineStart = 0;
            int cvIndex = 0;

            qimg = QImage( img->width, img->height, QImage::Format_RGB32 );
            const uchar* cvImgData = reinterpret_cast<const uchar*>( img->imageData );

            for (int y = 0; y < img->height; ++y )
            {
                cvIndex = cvLineStart;
                uchar* sl = qimg.scanLine( y );
                QRgb* scanline = reinterpret_cast<QRgb*>( sl );

                for (int x = 0; x < img->width; ++x )
                {
                    // here we convert OpenCV's BGR to Qt's RGB
                    const unsigned char blue  = cvImgData[cvIndex+0];
                    const unsigned char green = cvImgData[cvIndex+1];
                    const unsigned char red   = cvImgData[cvIndex+2];

                    scanline[x] = qRgb( red, green, blue );
                    cvIndex += 3;
                }
                cvLineStart += img->widthStep;
            }
        }
        else
        {
            errStr = QString("Conversion not supported: depth IPL_DEPTH_8U "
                             "and number of channels is %1").arg(img->nChannels);
            throw ImageConversionException( errStr.toStdString() );
        }
        break;

    case IPL_DEPTH_16U:
        if( img->nChannels == 1 )
        {
            // OpenCV image is stored with 2 bytes grey pixel.
            // Convert it to an 8 bit indexed QImage.
            // We add the index at the function exit
            int cvLineStart = 0;
            int cvIndex = 0;
            qimg = QImage( img->width, img->height, QImage::Format_Indexed8 );
            const uint16_t* cvImgData = reinterpret_cast<const uint16_t*>( img->imageData );

            int step = img->widthStep / sizeof(uint16_t);
            for (int y = 0; y < img->height; ++y )
            {
                cvIndex = cvLineStart;
                uchar* scanline = qimg.scanLine( y );

                for (int x = 0; x < img->width; ++x )
                {
                    scanline[x] = (cvImgData[cvIndex] >> 8); //divide by 256
                    ++cvIndex;
                }
                cvLineStart += step;
            }
        }
        else if( img->nChannels == 3 )
        {
            // image is stored with 3 channels and 2 bytes per pixel
            // per channel. Convert to RGB32 which uses 4 bytes per pixel
            int cvLineStart = 0;
            int cvIndex = 0;

            qimg = QImage( img->width, img->height, QImage::Format_RGB32 );
            const uint16_t* cvImgData = reinterpret_cast<const uint16_t*>( img->imageData );

            int step = img->widthStep / sizeof(uint16_t);
            for (int y = 0; y < img->height; ++y )
            {
                cvIndex = cvLineStart;
                QRgb* scanline = reinterpret_cast<QRgb*>( qimg.scanLine( y ) );

                for( int x = 0; x < img->width; ++x )
                {
                    // here we convert OpenCV's BGR to Qt's RGB
                    const unsigned int blue  = cvImgData[cvIndex+0];
                    const unsigned int green = cvImgData[cvIndex+1];
                    const unsigned int red   = cvImgData[cvIndex+2];

                    scanline[x] = qRgb( red, green, blue );
                    cvIndex += 3;
                }
                cvLineStart += step;
            }
        }
        else
        {
            errStr = QString("Conversion not supported: depth IPL_DEPTH_16U "
                             "and number of channels is %1").arg(img->nChannels);
            throw ImageConversionException( errStr.toStdString() );
        }
        break;

    case IPL_DEPTH_16S:
        if( img->nChannels == 1 )
        {
            // OpenCV image is stored with 2 bytes grey pixel.
            // Convert it to an 8 bit indexed QImage.
            // We add the index at the function exit
            int cvLineStart = 0;
            int cvIndex = 0;
            qimg = QImage( img->width, img->height, QImage::Format_Indexed8 );
            const uint16_t* cvImgData = reinterpret_cast<const uint16_t*>( img->imageData );

            int step = img->widthStep / sizeof(uint16_t);
            for (int y = 0; y < img->height; ++y )
            {
                cvIndex = cvLineStart;
                uchar* scanline = qimg.scanLine( y );

                for (int x = 0; x < img->width; ++x )
                {
                    scanline[x] = (cvImgData[cvIndex] >> 8); //divide by 256
                    ++cvIndex;
                }
                cvLineStart += step;
            }
        }
        else if( img->nChannels == 3 )
        {
            // image is stored with 3 channels and 2 bytes per pixel
            // per channel. Convert to RGB32 which uses 4 bytes per pixel
            int cvLineStart = 0;
            int cvIndex = 0;

            qimg = QImage( img->width, img->height, QImage::Format_RGB32 );
            const int16_t* cvImgData = reinterpret_cast<const int16_t*>( img->imageData );

            int step = img->widthStep / sizeof(uint16_t);
            for (int y = 0; y < img->height; ++y )
            {
                cvIndex = cvLineStart;
                QRgb* scanline = reinterpret_cast<QRgb*>( qimg.scanLine( y ) );

                for( int x = 0; x < img->width; ++x )
                {
                    // here we convert OpenCV's BGR to Qt's RGB
                    const unsigned int blue  = cvImgData[cvIndex+0];
                    const unsigned int green = cvImgData[cvIndex+1];
                    const unsigned int red   = cvImgData[cvIndex+2];

                    scanline[x] = qRgb( red, green, blue );
                    cvIndex += 3;
                }
                cvLineStart += step;
            }
        }
        else
        {
            errStr = QString("Conversion not supported: depth IPL_DEPTH_16S "
                             "and number of channels is %1").arg(img->nChannels);
            throw ImageConversionException( errStr.toStdString() );
        }
        break;

    case IPL_DEPTH_32F:
        if( img->nChannels == 1 )
        {
            // OpenCV image is stored with 2 bytes grey pixel.
            // Convert it to an 8-bit QImage.
            int cvLineStart = 0;
            int cvIndex = 0;
            qimg = QImage( img->width, img->height, QImage::Format_Indexed8);
            const float* cvImgData = reinterpret_cast<const float*>( img->imageData );

            int step = img->widthStep / sizeof(float);
            for (int y = 0; y < img->height; ++y )
            {
                cvIndex = cvLineStart;
                uchar* scanline = qimg.scanLine( y );

                for (int x = 0; x < img->width; ++x )
                {
                    // float values between 0 and 1.0 so upscale
                    scanline[x] = (uchar)((int)(cvImgData[cvIndex] * 255.0)); //mult by 255
                    ++cvIndex;
                }
                cvLineStart += step;
            }
        }
        else if( img->nChannels == 3 )
        {
            // image is stored with 3 channels and 4 bytes per pixel
            // per channel. Convert to RGB32 which uses 4 bytes per pixel
            int cvLineStart = 0;
            int cvIndex = 0;

            qimg = QImage( img->width, img->height, QImage::Format_RGB32 );
            const float* cvImgData = reinterpret_cast<const float*>( img->imageData );

            int step = img->widthStep / sizeof(uint16_t);
            for (int y = 0; y < img->height; ++y )
            {
                cvIndex = cvLineStart;
                QRgb* scanline = reinterpret_cast<QRgb*>( qimg.scanLine( y ) );

                for( int x = 0; x < img->width; ++x )
                {
                    // here we convert OpenCV's BGR to Qt's RGB
                    const unsigned int blue  = (int)(cvImgData[cvIndex+0]*255.0);
                    const unsigned int green = (int)(cvImgData[cvIndex+1]*255.0);
                    const unsigned int red   = (int)(cvImgData[cvIndex+2]*255.0);

                    scanline[x] = qRgb( red, green, blue );
                    cvIndex += 3;
                }
                cvLineStart += step;
            }
        }
        else
        {
            errStr = QString("Conversion not supported: depth IPL_DEPTH_32F "
                             "and number of channels is %1").arg(img->nChannels);
            throw ImageConversionException( errStr.toStdString() );
        }
        break;
    default:
        errStr = QString("Conversion not supported: depth %1 "
                         "and number of channels is %2")
                .arg(OpenCVImage::depthToString(img->depth))
                .arg(img->nChannels);
        throw ImageConversionException( errStr.toStdString() );
    }

    if( img->nChannels == 1 )
    {
        QVector<QRgb> colorTable;
        for (int i = 0; i < 256; i++)
        {
            colorTable.push_back(qRgb(i, i, i));
        }
        qimg.setColorTable(colorTable);
    }

    return qimg;
}

