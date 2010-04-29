#include "ImageConverter.h"

#include <QImage>
#include <QtConcurrentRun>

#include "OpenCVImage.h"
#include "QtImage.h"

using namespace plvgui;
using namespace plv;

ImageConverter* ImageConverter::m_instance = 0;

void ImageConverter::convert_OpenCVImage(RefPtr<OpenCVImage> img)
{
    QtConcurrent::run(this, &ImageConverter::convert, img);
}

void ImageConverter::convert(RefPtr<OpenCVImage> imgdata)
{
    IplImage* image = imgdata->getImage();
    
    RefPtr<QtImage> q_img = new QtImage(
            new QImage( image->width,
                        image->height,
                        QImage::Format_RGB32 ));

    // switch between bit depths
    int cvLineStart = 0;
    int cvIndex = 0;
    switch (image->depth)
    {
        case IPL_DEPTH_8U:
            switch (image->nChannels)
            {
                case 3:

                    for (int y = 0; y < image->height; y++)
                    {
                        cvIndex = cvLineStart;
                        for (int x = 0; x < image->width; x++)
                        {
                            unsigned char red = image->imageData[cvIndex+2];
                            unsigned char green = image->imageData[cvIndex+1];
                            unsigned char blue  = image->imageData[cvIndex+0];

                            q_img->getImage()->setPixel(x,y, qRgb(red, green, blue));
                            cvIndex += 3;
                        }
                        cvLineStart += image->widthStep;
                    }

                    break;
                default:
                    qDebug() << "This number of channels is not supported\n";
                    break;
            }
            break;
        default:
            qDebug() << "This type of IplImage is not implemented\n";
            break;
    }
    qDebug() << "emitting converted(img)";
    emit(converted(q_img));
}

/** Function to convert an OpenCV images to QImage. Courtesy of user patrik08 in
  * http://www.qtcentre.org/threads/11655-OpenCV-integration.
  *
  *
  */
QImage* ImageConverter::iplImageToQImage(const IplImage* iplImage ) //, uchar** data , bool mirroRimage = true )
{
    uchar* qImageBuffer = NULL;
    int    width        = iplImage->width;

    // Note here that OpenCV image is stored so that each lined is 32-bits aligned thus
    // explaining the necessity to "skip" the few last bytes of each line of OpenCV image buffer.
    int widthStep = iplImage->widthStep;
    int height    = iplImage->height;

    switch (iplImage->depth)
    {
    case IPL_DEPTH_8U:
        if (iplImage->nChannels == 1)
        {
            // OpenCV image is stored with one byte grey pixel.
            // We convert it to an 8 bit depth QImage.
            qImageBuffer = (uchar *)malloc(width * height * sizeof(uchar));
            uchar *QImagePtr = qImageBuffer;
            const uchar *iplImagePtr = (const uchar *)iplImage->imageData;
            for (int y = 0; y < height; ++y)
            {
                // Copy line by line
                memcpy(QImagePtr, iplImagePtr, width);
                QImagePtr += width;
                iplImagePtr += widthStep;
            }
        }
        else if (iplImage->nChannels == 3)
        {
            // OpenCV image is stored with 3 byte color pixels (3 channels).
            // We convert it to a 32 bit depth QImage.
            qImageBuffer = (uchar *)malloc(width * height * 4 * sizeof(uchar));
            uchar *QImagePtr = qImageBuffer;
            const uchar *iplImagePtr = (const uchar *)iplImage->imageData;

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    // We cannot help but copy manually.
                    QImagePtr[0] = iplImagePtr[0];
                    QImagePtr[1] = iplImagePtr[1];
                    QImagePtr[2] = iplImagePtr[2];
                    QImagePtr[3] = 0;

                    QImagePtr += 4;
                    iplImagePtr += 3;
                }
                iplImagePtr += widthStep - 3 * width;
            }
        }
        else
        {
            qDebug("IplImageToQImage: image format is not supported : depth=8U and %d channels\n", iplImage->nChannels);
        }
        break;

    case IPL_DEPTH_16U:
        if (iplImage->nChannels == 1)
        {
            // OpenCV image is stored with 2 bytes grey pixel.
            // We convert it to an 8 bit depth QImage.
            qImageBuffer = (uchar *)malloc(width * height * sizeof(uchar));
            uchar *QImagePtr = qImageBuffer;
            const uint16_t *iplImagePtr = (const uint16_t *)iplImage->imageData;

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    // We take only the highest part of the 16 bit value. It is similar to dividing by 256.
                    *QImagePtr++ = ((*iplImagePtr++) >> 8);
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
            uchar *QImagePtr = qImageBuffer;
            const float *iplImagePtr = (const float *)iplImage->imageData;

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    *QImagePtr++ = (uchar)(255 * ((*iplImagePtr++)));
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
            uchar *QImagePtr = qImageBuffer;
            const double *iplImagePtr = (const double *) iplImage->imageData;

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    *QImagePtr++ = (uchar)(255 * ((*iplImagePtr++)));
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

    QImage* qImage;
    if (iplImage->nChannels == 1)
    {
        QVector<QRgb> colorTable;
        for (int i = 0; i < 256; i++)
        colorTable.push_back(qRgb(i, i, i));

        qImage = new QImage(qImageBuffer, width, height, QImage::Format_Indexed8);
        qImage->setColorTable(colorTable);
    }
    else
    {
        qImage = new QImage(qImageBuffer, width, height, QImage::Format_RGB32);
    }
    return qImage;

//    QImage gd0 = qImage->mirrored(false,mirroRimage);
//    *data = qImageBuffer;
//
//    QColor textColor = Qt::black;
//    QColor fillrectcolor = Qt::red;
//    QColor shapepicture = Qt::white;
//
//    QImage gd = gd0.scaledToWidth(350);
//
//    QDateTime now = QDateTime::currentDateTime ();
//    QString selectionText = now.toString("dd.MM.yyyy hh:mm:ss");
//    QPainter p(&gd);
//    p.setRenderHint(QPainter::Antialiasing, true);
//    QFontMetrics fm( qApp->font() );
//    int stringWidth = fm.width(selectionText);
//    int stringHeight = fm.ascent();
//    const int sx = gd.width() - stringWidth - 5;
//    QPen pen;
//    pen.setStyle( Qt::SolidLine );
//    pen.setWidth( 2 );
//    pen.setColor( textColor );
//    p.setPen( pen);
//    p.drawText(QPointF(sx - 1 ,gd.height() - 2 - stringHeight - 1),selectionText);
//    pen.setColor( fillrectcolor );
//    p.setPen( pen);
//    p.drawText(QPointF(sx,gd.height() - 2 - stringHeight),selectionText);
//
//    return gd;
}

