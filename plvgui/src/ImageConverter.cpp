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
#include "OpenCVImage.h"

#include <QImage>
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
        qDebug() << "Failed to convert image with error: " << e.what() << "\n";
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
                    const unsigned char& blue  = cvImgData[cvIndex+0];
                    const unsigned char& green = cvImgData[cvIndex+1];
                    const unsigned char& red   = cvImgData[cvIndex+2];

                    scanline[x] = qRgb( red, green, blue );
                    cvIndex += 3;
                }
                cvLineStart += img->widthStep;
            }
        }
        else
        {
            errStr = "Conversion not supported: depth IPL_DEPTH_8U and number of channels is ";
            errStr += img->nChannels + " \n";
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

            for (int y = 0; y < img->height; ++y )
            {
                cvIndex = cvLineStart;
                uchar* scanline = qimg.scanLine( y );

                for (int x = 0; x < img->width; ++x )
                {
                    scanline[x] = (cvImgData[cvIndex] >> 8); //divide by 256
                    ++cvIndex;
                }
                cvLineStart += img->widthStep / sizeof(uint16_t);
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

            for (int y = 0; y < img->height; ++y )
            {
                cvIndex = cvLineStart;
                uchar* sl = qimg.scanLine( y );
                QRgb* scanline = reinterpret_cast<QRgb*>( sl );

                for( int x = 0; x < img->width; ++x )
                {
                    // here we convert OpenCV's BGR to Qt's RGB
                    const unsigned int& blue  = cvImgData[cvIndex+0];
                    const unsigned int& green = cvImgData[cvIndex+1];
                    const unsigned int& red   = cvImgData[cvIndex+2];

                    scanline[x] = qRgb( red, green, blue );
                    cvIndex += 3;
                }
                cvLineStart += img->widthStep / sizeof(uint16_t);
            }
        }
        else
        {
            errStr = "Conversion not supported: depth IPL_DEPTH_16U and number of channels is ";
            errStr += img->nChannels + " \n";
            throw ImageConversionException( errStr.toStdString() );
        }

    case IPL_DEPTH_32F:
        errStr = "Conversion not supported: depth IPL_DEPTH_32F and number of channels is ";
        errStr += img->nChannels + " \n";
        throw ImageConversionException( errStr.toStdString() );

    case IPL_DEPTH_32S:
        errStr = "Conversion not supported: depth IPL_DEPTH_32S and number of channels is ";
        errStr += img->nChannels + " \n";
        throw ImageConversionException( errStr.toStdString() );

    case IPL_DEPTH_64F:
        errStr = "Conversion not supported: depth IPL_DEPTH_64F and number of channels is ";
        errStr += img->nChannels + " \n";
        throw ImageConversionException( errStr.toStdString() );

    case IPL_DEPTH_1U:
        errStr = "Conversion not supported: depth IPL_DEPTH_1U and number of channels is ";
        errStr += img->nChannels + " \n";
        throw ImageConversionException( errStr.toStdString() );

    case IPL_DEPTH_8S:
        errStr = "Conversion not supported: depth IPL_DEPTH_8S and number of channels is ";
        errStr += img->nChannels + " \n";
        throw ImageConversionException( errStr.toStdString() );

    case IPL_DEPTH_16S:
        errStr = "Conversion not supported: depth IPL_DEPTH_16S and number of channels is ";
        errStr += img->nChannels + " \n";
        throw ImageConversionException( errStr.toStdString() );

    default:
        throw ImageConversionException( "Conversion not supported: unknown IplImage type. \n" );
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

#if 0
/** Function to convert an OpenCV images to QImage. Courtesy of user patrik08 in
  * http://www.qtcentre.org/threads/11655-OpenCV-integration.
  *
  *
  */
QImage* ImageConverter::iplImageToQImage(const IplImage* iplImage, uchar** data )
{
    QImage* qImage;
	uchar* qImageBuffer = NULL;

    // Note here that OpenCV image is stored so that each lined is 32-bits
    // aligned thus explaining the necessity to "skip" the few last bytes of
    // each line of OpenCV image buffer.
    int widthStep = iplImage->widthStep;
    int width     = iplImage->width;
    int height    = iplImage->height;

    switch( iplImage->depth )
    {
    case IPL_DEPTH_8U:
        if (iplImage->nChannels == 1)
        {
            // OpenCV image is stored with one byte grey pixel.
            // We convert it to an 8 bit depth QImage.
            qImageBuffer = (uchar *)malloc(width * height * sizeof(uchar));
            uchar* qImagePtr = qImageBuffer;
            const uchar* iplImagePtr = (const uchar*) iplImage->imageData;
            for (int y = 0; y < height; ++y)
            {
                // Copy line by line
                memcpy(qImagePtr, iplImagePtr, width);
                qImagePtr   += width;
                iplImagePtr += widthStep;
            }
        }
        else if (iplImage->nChannels == 3)
        {
            // OpenCV image is stored with 3 byte color pixels (3 channels).
            // We convert it to a 32 bit depth QImage.
			qImage = new QImage(qImageBuffer, width, height, QImage::Format_RGB32);
            //qImageBuffer = (uchar *)malloc(width * height * 4 * sizeof(uchar));
			qImageBuffer = qImage->bits();
            uchar* qImagePtr = qImageBuffer;
            const uchar *iplImagePtr = (const uchar *)iplImage->imageData;
            int step = widthStep - (3 * width);

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    // We cannot help but copy manually.
                    qImagePtr[0] = iplImagePtr[0];
                    qImagePtr[1] = iplImagePtr[1];
                    qImagePtr[2] = iplImagePtr[2];
                    qImagePtr[3] = 0;

                    qImagePtr   += 4;
                    iplImagePtr += 3;
                }
                iplImagePtr += step;
            }
        }
        else
        {
            qDebug("IplImageToQImage: image format is not supported : depth=8U and %d channels\n",
                   iplImage->nChannels);
        }
        break;

    case IPL_DEPTH_16U:
        if (iplImage->nChannels == 1)
        {
            // OpenCV image is stored with 2 bytes grey pixel.
            // We convert it to an 8 bit depth QImage.
            qImageBuffer = (uchar *)malloc(width * height * sizeof(uchar));
            uchar* qImagePtr = qImageBuffer;
            const uint16_t *iplImagePtr = (const uint16_t *)iplImage->imageData;

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    // We take only the highest part of the 16 bit value.
                    // It is similar to dividing by 256.
                    *qImagePtr++ = ((*iplImagePtr++) >> 8);
                }
                iplImagePtr += widthStep / sizeof(uint16_t) - width;
            }
        }
        else
        {
            qDebug("IplImageToQImage: image format is not supported : depth=16U and %d channels\n",
                   iplImage->nChannels);
        }
        break;

    case IPL_DEPTH_32F:
        if (iplImage->nChannels == 1)
        {
            // OpenCV image is stored with float (4 bytes) grey pixel.
            // Convert it to an 8 bit depth QImage.
            qImageBuffer = (uchar *)malloc(width * height * sizeof(uchar));
            uchar* qImagePtr = qImageBuffer;
            const float *iplImagePtr = (const float *)iplImage->imageData;

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    *qImagePtr++ = (uchar)(255 * ((*iplImagePtr++)));
                }
                iplImagePtr += widthStep / sizeof(float) - width;
            }
        }
        else
        {
            qDebug("IplImageToQImage: image format is not supported : depth=32F and %d channels\n",
                   iplImage->nChannels);
        }

        break;

    case IPL_DEPTH_64F:
        if (iplImage->nChannels == 1)
        {
            // OpenCV image is stored with double (8 bytes) grey pixel.
            // Convert it to an 8 bit depth QImage.
            qImageBuffer = (uchar *) malloc(width * height * sizeof(uchar));
            uchar *qImagePtr = qImageBuffer;
            const double *iplImagePtr = (const double *) iplImage->imageData;

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    *qImagePtr++ = (uchar)(255 * ((*iplImagePtr++)));
                }
                iplImagePtr += widthStep / sizeof(double) - width;
            }
        }
        else
        {
            qDebug("IplImageToQImage: image format is not supported : depth=64F and %d channels\n",
                   iplImage->nChannels);
        }
        break;

    default:
        qDebug("IplImageToQImage: image format is not supported : depth=%d and %d channels\n",
               iplImage->depth, iplImage->nChannels);
    }

    if (iplImage->nChannels == 1)
    {
        QVector<QRgb> colorTable;
        for (int i = 0; i < 256; i++)
        colorTable.push_back(qRgb(i, i, i));

        //qImage = new QImage(qImageBuffer, width, height, QImage::Format_Indexed8);
        qImage->setColorTable(colorTable);
    }
    else
    {
        //qImage = new QImage(qImageBuffer, width, height, QImage::Format_RGB32);
    }
    *data = qImageBuffer;
    return qImage;
}

#endif

