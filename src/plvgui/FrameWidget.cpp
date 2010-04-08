#include "FrameWidget.h"
#include "RefPtr.h"

#include <QPixmap>
#include <QLabel>

#include <QVBoxLayout>
#include <QImage>
#include <iostream>

using namespace plvgui;
using namespace plv;

// Constructor
FrameWidget::FrameWidget(QWidget *parent) : QWidget(parent)
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

FrameWidget::~FrameWidget(void)
{
}

void FrameWidget::putImage( OpenCVImage* cvimage )
{
    RefPtr<OpenCVImage> ocvimg = cvimage;
    IplImage* image = ocvimg->getImage();
    // switch between bit depths
    int cvLineStart = 0;
    int cvIndex = 0;
    switch (image->depth)
    {
        case IPL_DEPTH_8U:
            switch (image->nChannels)
            {
                case 3:
                    if ( (image->width  != m_image.width()) ||
                         (image->height != m_image.height()) )
                    {
                        m_image = QImage(image->width, image->height,
                                    QImage::Format_RGB32);
                    }
                    for (int y = 0; y < image->height; y++)
                    {
                        cvIndex = cvLineStart;
                        for (int x = 0; x < image->width; x++)
                        {
                            unsigned char red = image->imageData[cvIndex+2];
                            unsigned char green = image->imageData[cvIndex+1];
                            unsigned char blue  = image->imageData[cvIndex+0];

                            m_image.setPixel(x,y, qRgb(red, green, blue) );
                            cvIndex += 3;
                        }
                        cvLineStart += image->widthStep;
                    }
                    break;
                default:
                    std::cout << "This number of channels is not supported\n";
                    break;
            }
            break;
        default:
            std::cout << "This type of IplImage is not implemented in FrameWidget\n";
            break;
    }
    m_imagelabel->setPixmap( QPixmap::fromImage( m_image ) );
}

void FrameWidget::setFrame( OpenCVImage* frame )
{
    putImage( frame );
}


void FrameWidget::setSource( plv::OpenCVCamera* cam ) {
    //TODO disconnect
    connect( cam, SIGNAL( newFrame( plv::OpenCVImage* ) ),
             this, SLOT( setFrame( plv::OpenCVImage*) ) );
}
