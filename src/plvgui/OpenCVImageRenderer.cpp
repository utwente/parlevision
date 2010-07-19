#include "OpenCVImageRenderer.h"

#include <QDebug>
#include <QPixmap>
#include <QLabel>
#include <QVBoxLayout>
#include <QImage>
#include <QtConcurrentRun>
#include <QFutureWatcher>
#include <QMutexLocker>

#include "Types.h"
#include "OpenCVImage.h"
#include "QtImage.h"
#include "RefPtr.h"
#include "ImageConverter.h"

using namespace plvgui;
using namespace plv;

OpenCVImageRenderer::OpenCVImageRenderer(QWidget* parent)
    : DataRenderer(parent),
        m_busy(false)

{
    m_layout      = new QVBoxLayout;
    m_imagelabel  = new ImageLabel(this);
    QImage image  = QImage(100,100,QImage::Format_RGB32);

    m_layout->addWidget( m_imagelabel );

    for (int x = 0; x < 100; x ++)
        for (int y =0; y < 100; y++)
            image.setPixel( x, y, qRgb(x, y, y) );

    m_imagelabel->setPixmap( QPixmap::fromImage( image ) );
    setLayout( m_layout );
    m_layout->setAlignment(Qt::AlignCenter);

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHeightForWidth(true);
    m_imagelabel->setSizePolicy(sizePolicy);

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

    if(m_imagelabel->sizeHint() != this->sizeHint())
    {
        this->sizeHint() = m_imagelabel->sizeHint();
        updateGeometry();
    }

    m_busy = false;
}

QSize OpenCVImageRenderer::sizeHint() const
{
    return m_imagelabel->sizeHint();
}

int OpenCVImageRenderer::heightForWidth(int w) const
{
    return ((double)m_imagelabel->sizeHint().height()/(double)m_imagelabel->sizeHint().width()) * w;
}


ImageLabel::ImageLabel(QWidget *parent) :
        QLabel(parent)
{
    setScaledContents(true);
}

int ImageLabel::heightForWidth(int w) const
{
//    int h = ((double)sizeHint().height()/(double)sizeHint().width()) * w;
//    return h;
    return 100;
}

QSize ImageLabel::sizeHint() const
{
    return QSize(0,0);
}

QSize ImageLabel::minimumSizeHint() const
{
    return QSize(0,0);
}
