#include "ImageConverter.h"

#include <QImage>
#include <QtConcurrentRun>

#include "OpenCVImage.h"
#include "QtImage.h"

using namespace plvgui;
using namespace plv;

ImageConverter* ImageConverter::m_instance = 0;

void ImageConverter::convert_OpenCVImage( RefPtr<OpenCVImage> img )
{
    QtConcurrent::run(this, &ImageConverter::convert, img);
}

void ImageConverter::convert( RefPtr<OpenCVImage> imgdata )
{
    IplImage* image = imgdata->getImage();
	
    QImage* qImage = iplImageToQImageFast( image );
    //QImage* qImage = iplImageToQImageSafe( image );

    if( qImage != 0 )
    {
        RefPtr<QtImage> qtimg = new QtImage( qImage );
        emit( converted( qtimg ) );
    }
    else
    {
        qDebug( "Failed to convert image" );
    }
}

QImage* ImageConverter::iplImageToQImageSafe( const IplImage* img )
{
    const uchar* imgData = reinterpret_cast<const uchar*>( img->imageData );

    QImage* qimg = new QImage( img->width,
                           img->height,
                           QImage::Format_RGB32 );

    // switch between bit depths
    int cvLineStart = 0;
    int cvIndex = 0;
    switch( img->depth )
    {
        case IPL_DEPTH_8U:
            switch( img->nChannels )
            {
                case 3:
                    for (int y = 0; y < img->height; y++)
                    {
                        cvIndex = cvLineStart;
                        QRgb* scanline = reinterpret_cast<QRgb*>( qimg->scanLine( y ) );

                        for (int x = 0; x < img->width; x++)
                        {
                            // here we convert OpenCV's BGR to Qt's RGB
                            const unsigned char& blue  = imgData[cvIndex+0];
                            const unsigned char& green = imgData[cvIndex+1];
                            const unsigned char& red   = imgData[cvIndex+2];

                            scanline[x] = qRgb( red, green, blue );
                            cvIndex += 3;
                        }
                        cvLineStart += img->widthStep;
                    }
                    break;
                default:
                    qDebug() << "This number of channels is not supported\n";
                    delete qimg;
                    return 0;
            }
            break;
        default:
            qDebug() << "This type of IplImage is not implemented\n";
            delete qimg;
            return 0;
    }

    return qimg;
}

QImage* ImageConverter::iplImageToQImageFast( const IplImage* img )
{
    const uchar* imgData = reinterpret_cast<const uchar*>( img->imageData );

    QImage* qimg = new QImage( img->width,
                               img->height,
                               QImage::Format_RGB32 );

    uchar* qImageData = qimg->bits();

    // switch between bit depths
    int cvLineStart = 0;
    int qtLineStart = 0;
    int cvIndex = 0;
    int qtIndex = 0;
    switch( img->depth )
    {
        case IPL_DEPTH_8U:
            switch( img->nChannels )
            {
                case 3:
                    for (int y = 0; y < img->height; ++y )
                    {
                        cvIndex = cvLineStart;
                        qtIndex = qtLineStart;
												
						for (int x = 0; x < img->width; ++x )
                        {
							qImageData[qtIndex]   = imgData[cvIndex];
							qImageData[qtIndex+1] = imgData[cvIndex+1];
							qImageData[qtIndex+2] = imgData[cvIndex+2];
							qImageData[qtIndex+3] = 0;
                            
                            cvIndex += 3;
                            qtIndex += 4;
                        }
                        
                        cvLineStart += img->widthStep;
                        qtLineStart += qimg->bytesPerLine();
                    }

                    break;
                default:
                    qDebug() << "This number of channels is not supported\n";
                    delete qimg;
                    return 0;
            }
            break;
        default:
            qDebug() << "This type of IplImage is not implemented\n";
            delete qimg;
            return 0;
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

    //case IPL_DEPTH_16U:
    //    if (iplImage->nChannels == 1)
    //    {
    //        // OpenCV image is stored with 2 bytes grey pixel.
    //        // We convert it to an 8 bit depth QImage.
    //        qImageBuffer = (uchar *)malloc(width * height * sizeof(uchar));
    //        uchar* qImagePtr = qImageBuffer;
    //        const uint16_t *iplImagePtr = (const uint16_t *)iplImage->imageData;

    //        for (int y = 0; y < height; ++y)
    //        {
    //            for (int x = 0; x < width; ++x)
    //            {
    //                // We take only the highest part of the 16 bit value.
    //                // It is similar to dividing by 256.
    //                *qImagePtr++ = ((*iplImagePtr++) >> 8);
    //            }
    //            iplImagePtr += widthStep / sizeof(uint16_t) - width;
    //        }
    //    }
    //    else
    //    {
    //        qDebug("IplImageToQImage: image format is not supported : depth=16U and %d channels\n",
    //               iplImage->nChannels);
    //    }
    //    break;

    //case IPL_DEPTH_32F:
    //    if (iplImage->nChannels == 1)
    //    {
    //        // OpenCV image is stored with float (4 bytes) grey pixel.
    //        // Convert it to an 8 bit depth QImage.
    //        qImageBuffer = (uchar *)malloc(width * height * sizeof(uchar));
    //        uchar* qImagePtr = qImageBuffer;
    //        const float *iplImagePtr = (const float *)iplImage->imageData;

    //        for (int y = 0; y < height; ++y)
    //        {
    //            for (int x = 0; x < width; ++x)
    //            {
    //                *qImagePtr++ = (uchar)(255 * ((*iplImagePtr++)));
    //            }
    //            iplImagePtr += widthStep / sizeof(float) - width;
    //        }
    //    }
    //    else
    //    {
    //        qDebug("IplImageToQImage: image format is not supported : depth=32F and %d channels\n",
    //               iplImage->nChannels);
    //    }

    //    break;

    //case IPL_DEPTH_64F:
    //    if (iplImage->nChannels == 1)
    //    {
    //        // OpenCV image is stored with double (8 bytes) grey pixel.
    //        // Convert it to an 8 bit depth QImage.
    //        qImageBuffer = (uchar *) malloc(width * height * sizeof(uchar));
    //        uchar *qImagePtr = qImageBuffer;
    //        const double *iplImagePtr = (const double *) iplImage->imageData;

    //        for (int y = 0; y < height; ++y)
    //        {
    //            for (int x = 0; x < width; ++x)
    //            {
    //                *qImagePtr++ = (uchar)(255 * ((*iplImagePtr++)));
    //            }
    //            iplImagePtr += widthStep / sizeof(double) - width;
    //        }
    //    }
    //    else
    //    {
    //        qDebug("IplImageToQImage: image format is not supported : depth=64F and %d channels\n",
    //               iplImage->nChannels);
    //    }
    //    break;

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

