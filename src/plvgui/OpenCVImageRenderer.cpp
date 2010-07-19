#include "OpenCVImageRenderer.h"

#include <QDebug>
#include <QtGui>

#include "Types.h"
#include "OpenCVImage.h"
#include "QtImage.h"
#include "RefPtr.h"
#include "ImageConverter.h"

using namespace plvgui;
using namespace plv;

OpenCVImageRenderer::OpenCVImageRenderer(QWidget* parent)
    : DataRenderer(parent),
        m_constraintWidget(new QWidget(this)),
        m_busy(false)

{
    QVBoxLayout* innerLayout = new QVBoxLayout(m_constraintWidget);
    m_constraintWidget->setLayout(innerLayout);

    m_layout      = new QVBoxLayout(this);
    m_imagelabel  = new ImageLabel(this);

    QImage image  = QImage(100,100,QImage::Format_RGB32);
    innerLayout->addWidget(m_imagelabel);
    innerLayout->setSizeConstraint(QLayout::SetFixedSize);

    m_layout->addStretch(0);
    m_layout->addWidget( m_constraintWidget );
    m_layout->setSizeConstraint(QLayout::SetDefaultConstraint);
    m_layout->addStretch(0);

    for (int x = 0; x < 100; x ++)
        for (int y =0; y < 100; y++)
            image.setPixel( x, y, qRgb(x, y, y) );

    m_imagelabel->setPixmap( QPixmap::fromImage( image ) );
    setLayout( m_layout );

    QSizePolicy labelPolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    labelPolicy.setHeightForWidth(true);
    m_imagelabel->setSizePolicy(labelPolicy);

    QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    sizePolicy.setHeightForWidth(true);
    this->setSizePolicy(sizePolicy);

    m_converter = new ImageConverter();
    connect(m_converter.getPtr(), SIGNAL( converted(RefPtr<QtImage>) ),
             this,                 SLOT( updateImage(RefPtr<QtImage>) ) );

}

void OpenCVImageRenderer::newData( RefPtr<Data> data )
{
    QMutexLocker lock( &m_busy_mutex );

    if(m_busy)
    {
        return;
    }
    else
    {
        RefPtr<OpenCVImage> img = ref_ptr_static_cast<OpenCVImage>(data);
        assert(img.isNotNull());
        if(img.isNotNull())
        {
            m_busy = true;
            // dispatch an asynchronous call
            m_converter->convert_OpenCVImage(img);
        }
    }
}

void OpenCVImageRenderer::updateImage( RefPtr<QtImage> image )
{
    //TODO better make this a separate mutex, but this will do for now
    QMutexLocker lock( &m_busy_mutex );
    QImage* qImg = image->getImage();
    QPixmap pixmap = QPixmap::fromImage(*qImg);
    m_imagelabel->setPixmap( pixmap );
    m_busy = false;
}

void OpenCVImageRenderer::resizeEvent(QResizeEvent * /*resizeEvent*/)
{
    fixAspectRatio();
}

void OpenCVImageRenderer::fixAspectRatio()
{
    int containerWidth =  m_imagelabel->width();
    int containerHeight = m_imagelabel->height();
    double _aspectRatio = (double)m_imagelabel->sizeHint().width()
                                    / (double)m_imagelabel->sizeHint().height();

    int contentsHeight = containerHeight ;
    int contentsWidth = containerHeight * _aspectRatio;
    if (contentsWidth > containerWidth ) {
            contentsWidth = containerWidth ;
            contentsHeight = containerWidth / _aspectRatio;
    }

    m_constraintWidget->resize(contentsWidth, contentsHeight);
}



ImageLabel::ImageLabel(QWidget *parent) :
        QLabel(parent)
{
    setScaledContents(true);
}

int ImageLabel::heightForWidth(int w) const
{
    int h = ((double)sizeHint().height()/(double)sizeHint().width()) * w;
    return h;
//    return 100;
}

QSize ImageLabel::sizeHint() const
{
    return QLabel::sizeHint();
}

QSize ImageLabel::minimumSizeHint() const
{
    return QSize(0,0);
}
