#include "ImageConverter.h"

#include <QImage>
#include <QtConcurrentRun>

#include "OpenCVImage.h"
#include "QtImage.h"

using namespace plvgui;
using namespace plv;

ImageConverter* ImageConverter::m_instance = 0;

ImageConverter* ImageConverter::getInstance()
{
    if(m_instance == 0)
    {
        m_instance = new ImageConverter();
    }
    return m_instance;
}

void ImageConverter::convert_OpenCVImage(RefPtr<OpenCVImage> img)
{
    QtConcurrent::run(this, &ImageConverter::convert, img);
}

void ImageConverter::convert(RefPtr<OpenCVImage> imgdata)
{
    IplImage* image = imgdata->getImage();
    RefPtr<QtImage> q_img = new QtImage(
                                            new QImage(image->width,
                                                          image->height,
                                                          QImage::Format_RGB32));


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
