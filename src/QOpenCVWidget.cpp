#include "QOpenCVWidget.h"

#include <QPixmap>
#include <QLabel>

#include <QVBoxLayout>
#include <QImage>
#include <iostream>

using namespace plvgui;

// Constructor
QOpenCVWidget::QOpenCVWidget(QWidget *parent) : QWidget(parent)
{
    m_layout      = new QVBoxLayout;
    m_imagelabel  = new QLabel;
    m_image       = QImage(100,100,QImage::Format_RGB32);

    m_layout->addWidget( m_imagelabel );

    for (int x = 0; x < 100; x ++)
        for (int y =0; y < 100; y++)
            m_image.setPixel( x, y, qRgb(x, y, y) );

    m_imagelabel->setPixmap( QPixmap::fromImage( m_image ) );
    setLayout( m_layout );
}

QOpenCVWidget::~QOpenCVWidget(void)
{
}

void QOpenCVWidget::putImage( const IplImage *cvimage )
{
    // switch between bit depths
    int cvLineStart = 0;
    int cvIndex = 0;
    switch (cvimage->depth)
    {
        case IPL_DEPTH_8U:
            switch (cvimage->nChannels)
            {
                case 3:
                    if ( (cvimage->width  != m_image.width()) ||
                         (cvimage->height != m_image.height()) )
                    {
                        m_image = QImage(cvimage->width, cvimage->height,
                                    QImage::Format_RGB32);
                    }
                    for (int y = 0; y < cvimage->height; y++)
                    {
                        cvIndex = cvLineStart;
                        for (int x = 0; x < cvimage->width; x++)
                        {
                            unsigned char red = cvimage->imageData[cvIndex+2];
                            unsigned char green = cvimage->imageData[cvIndex+1];
                            unsigned char blue  = cvimage->imageData[cvIndex+0];

                            m_image.setPixel(x,y, qRgb(red, green, blue) );
                            cvIndex += 3;
                        }
                        cvLineStart += cvimage->widthStep;
                    }
                    break;
                default:
                    std::cout << "This number of channels is not supported\n";
                    break;
            }
            break;
        default:
            std::cout << "This type of IplImage is not implemented in QOpenCVWidget\n";
            break;
    }
    m_imagelabel->setPixmap( QPixmap::fromImage( m_image ) );
}

