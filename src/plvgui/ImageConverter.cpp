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

#include <QImage>
#include <QPainter>
#include <QString>
#include <QtConcurrentRun>

using namespace plvgui;
using namespace plv;

ImageConverter::ImageConverter( QObject* parent ) : QObject(parent)
{
}

ImageConverter::~ImageConverter()
{
}

void ImageConverter::convertCvMatData( const plv::CvMatData& data, int id )
{
    QtConcurrent::run(this, &ImageConverter::convert, data, id);
}

void ImageConverter::convertCvMatDataList( const QList<plv::CvMatData>& dataList, int id )
{
    QtConcurrent::run(this, &ImageConverter::convertList, dataList, id);
}

void ImageConverter::convert( const plv::CvMatData& data, int id )
{
    try
    {
        QImage qimage = cvMatToQImage( data.get() );
        emit converted( qimage, id );
    }
    catch( ImageConversionException& e )
    {
        QString msg = QString("IplImageConverter failed to convert image with error: %1")
                      .arg( e.what() );

        QSize size( data.width(), data.height() );
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
        emit converted( qimage, id );
    }
}

// TODO parallelize
void ImageConverter::convertList( const QList<plv::CvMatData>& dataList, int id )
{
    QList<QImage> qimageList;

    foreach( const plv::CvMatData& data, dataList )
    {
        QImage qimage;
        try
        {
             qimage = cvMatToQImage( data.get() );
        }
        catch( ImageConversionException& e )
        {
            QString msg = QString("IplImageConverter failed to convert image with error: %1")
                          .arg( e.what() );

            QSize size( data.width(), data.height() );
            qimage = QImage( size, QImage::Format_ARGB32_Premultiplied );
            qimage.fill( 0 );
            QPainter painter(&qimage);
            painter.fillRect( qimage.rect(), QColor( Qt::white ));
            painter.setPen( Qt::black );
            QFont font = painter.font();
            font.setPointSize( 16 );
            painter.setFont( font );
            painter.drawText( qimage.rect(), Qt::AlignCenter | Qt::TextWordWrap, msg);
            painter.end();
        }
        qimageList.append(qimage);
    }
    emit convertedList( qimageList, id );
}

QImage ImageConverter::cvMatToQImage( const cv::Mat mat )
        throw( ImageConversionException )
{
    QImage qimg;
    QString errStr;

    int depth = mat.depth();
    int channels = mat.channels();

    int type = CV_MAKETYPE(depth,channels);

    switch( type )
    {
    case CV_8UC1:
    {
        // OpenCV image is stored with one byte grey pixel.
        // Convert it to an 8 bit indexed QImage.
        // We add the index at the function exit
        qimg = QImage( mat.cols, mat.rows, QImage::Format_Indexed8 );
        const uchar* cvImgData = reinterpret_cast<const uchar*>( mat.data );

        for( int y = 0; y < mat.rows; ++y )
        {
            // Copy line by line
            uchar* scanline = qimg.scanLine( y );
            memcpy( scanline, cvImgData, mat.cols );
            cvImgData += mat.step;
        }
    }
    break;
    case CV_8UC3:
    {
        // image is stored with 3 channels and one byte per pixel
        // per channel. Convert to RGB32 which uses 4 bytes per pixel
        int cvLineStart = 0;
        int cvIndex = 0;

        qimg = QImage( mat.cols, mat.rows, QImage::Format_RGB32 );
        const uchar* cvImgData = reinterpret_cast<const uchar*>( mat.data );

        for (int y = 0; y < mat.rows; ++y )
        {
            cvIndex = cvLineStart;
            uchar* sl = qimg.scanLine( y );
            QRgb* scanline = reinterpret_cast<QRgb*>( sl );

            for (int x = 0; x < mat.cols; ++x )
            {
                // here we convert OpenCV's BGR to Qt's RGB
                const unsigned char blue  = cvImgData[cvIndex+0];
                const unsigned char green = cvImgData[cvIndex+1];
                const unsigned char red   = cvImgData[cvIndex+2];

                scanline[x] = qRgb( red, green, blue );
                cvIndex += 3;
            }
            cvLineStart += mat.step;
        }
    }
    break;
    case CV_16UC1:
    {
        // OpenCV image is stored with 2 bytes grey pixel.
        // Convert it to an 8 bit indexed QImage.
        // We add the index at the function exit
        int cvLineStart = 0;
        int cvIndex = 0;
        qimg = QImage( mat.cols, mat.rows, QImage::Format_Indexed8 );
        const uint16_t* cvImgData = reinterpret_cast<const uint16_t*>( mat.data );

        int step = mat.step / sizeof(uint16_t);
        for (int y = 0; y < mat.rows; ++y )
        {
            cvIndex = cvLineStart;
            uchar* scanline = qimg.scanLine( y );

            for (int x = 0; x < mat.cols; ++x )
            {
                scanline[x] = (cvImgData[cvIndex] >> 8); //divide by 256
                ++cvIndex;
            }
            cvLineStart += step;
        }
    }
    break;
    case CV_16UC3:
    {
        // image is stored with 3 channels and 2 bytes per pixel
        // per channel. Convert to RGB32 which uses 4 bytes per pixel
        int cvLineStart = 0;
        int cvIndex = 0;

        qimg = QImage( mat.cols, mat.rows, QImage::Format_RGB32 );
        const uint16_t* cvImgData = reinterpret_cast<const uint16_t*>( mat.data );

        int step = mat.step / sizeof(uint16_t);
        for (int y = 0; y < mat.rows; ++y )
        {
            cvIndex = cvLineStart;
            QRgb* scanline = reinterpret_cast<QRgb*>( qimg.scanLine( y ) );

            for( int x = 0; x < mat.cols; ++x )
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
    break;
    case CV_16S:
    {
        // OpenCV image is stored with 2 bytes grey pixel.
        // Convert it to an 8 bit indexed QImage.
        // We add the index at the function exit
        int cvLineStart = 0;
        int cvIndex = 0;
        qimg = QImage( mat.cols, mat.rows, QImage::Format_Indexed8 );
        const uint16_t* cvImgData = reinterpret_cast<const uint16_t*>( mat.data );

        int step = mat.step / sizeof(uint16_t);
        for (int y = 0; y < mat.rows; ++y )
        {
            cvIndex = cvLineStart;
            uchar* scanline = qimg.scanLine( y );

            for (int x = 0; x < mat.cols; ++x )
            {
                scanline[x] = (cvImgData[cvIndex] >> 8); //divide by 256
                ++cvIndex;
            }
            cvLineStart += step;
        }
    }
    case CV_16SC3:
    {
        // image is stored with 3 channels and 2 bytes per pixel
        // per channel. Convert to RGB32 which uses 4 bytes per pixel
        int cvLineStart = 0;
        int cvIndex = 0;

        qimg = QImage( mat.cols, mat.rows, QImage::Format_RGB32 );
        const int16_t* cvImgData = reinterpret_cast<const int16_t*>( mat.data );

        int step = mat.step / sizeof(uint16_t);
        for (int y = 0; y < mat.rows; ++y )
        {
            cvIndex = cvLineStart;
            QRgb* scanline = reinterpret_cast<QRgb*>( qimg.scanLine( y ) );

            for( int x = 0; x < mat.cols; ++x )
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
    break;
    case CV_32FC1:
    {
        // OpenCV image is stored with 2 bytes grey pixel.
        // Convert it to an 8-bit QImage.
        int cvLineStart = 0;
        int cvIndex = 0;
        qimg = QImage( mat.cols, mat.rows, QImage::Format_Indexed8);
        const float* cvImgData = reinterpret_cast<const float*>( mat.data );

        int step = mat.step / sizeof(float);
        for (int y = 0; y < mat.rows; ++y )
        {
            cvIndex = cvLineStart;
            uchar* scanline = qimg.scanLine( y );

            for (int x = 0; x < mat.cols; ++x )
            {
                // float values between 0 and 1.0 so upscale
                scanline[x] = (uchar)((int)(cvImgData[cvIndex] * 255.0)); //mult by 255
                ++cvIndex;
            }
            cvLineStart += step;
        }
    }
    break;
    case CV_32FC3:
    {
        // image is stored with 3 channels and 4 bytes per pixel
        // per channel. Convert to RGB32 which uses 4 bytes per pixel
        int cvLineStart = 0;
        int cvIndex = 0;

        qimg = QImage( mat.cols, mat.rows, QImage::Format_RGB32 );
        const float* cvImgData = reinterpret_cast<const float*>( mat.data );

        int step = mat.step / sizeof(uint16_t);
        for (int y = 0; y < mat.rows; ++y )
        {
            cvIndex = cvLineStart;
            QRgb* scanline = reinterpret_cast<QRgb*>( qimg.scanLine( y ) );

            for( int x = 0; x < mat.cols; ++x )
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
    break;
    default:
        errStr = QString("Conversion not supported: depth %1 "
                         "and number of channels is %2")
                .arg(CvMatData::depthToString(mat.depth()))
                .arg(mat.channels());
        throw ImageConversionException( errStr.toStdString() );
    }
    if( channels == 1 )
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

