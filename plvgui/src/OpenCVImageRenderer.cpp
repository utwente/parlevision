#include "OpenCVImageRenderer.h"

#include <QDebug>
#include <QtGui>

#include "Types.h"
#include "OpenCVImage.h"
#include "QtImage.h"
#include "RefPtr.h"
#include "ImageConverter.h"
#include "ImageWidget.h"

using namespace plvgui;
using namespace plv;

OpenCVImageRenderer::OpenCVImageRenderer(QWidget* parent) :
    DataRenderer(parent),
    m_busy(false)

{
    m_layout      = new QVBoxLayout(this);
    m_imageWidget = new ImageWidget;

    m_layout->addWidget( m_imageWidget );
//    m_layout->setSizeConstraint(QLayout::SetDefaultConstraint);
//    m_layout->addStretch(0);

    QImage image = QImage(320,240,QImage::Format_RGB32);
    image.fill( qRgb(0,0,0) );
    m_imageWidget->setMinimumSize( 320, 240 );
    m_imageWidget->setImage( image );

    this->setLayout( m_layout );

//    QSizePolicy labelPolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//    labelPolicy.setHeightForWidth(true);
//    m_imagelabel->setSizePolicy(labelPolicy);

//    QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
//    sizePolicy.setHeightForWidth(true);
//    this->setSizePolicy(sizePolicy);

    m_converter = new ImageConverter();
}

void OpenCVImageRenderer::showEvent(QShowEvent* event)
{
    qDebug() << "connecting renderer";

    connect(m_converter.getPtr(), SIGNAL( converted(RefPtr<QtImage>) ),
             this,                 SLOT( updateImage(RefPtr<QtImage>) ),
             Qt::UniqueConnection);

    QWidget::showEvent(event);
}

void OpenCVImageRenderer::hideEvent(QHideEvent* event)
{
    qDebug() << "disconnecting renderer";
    this->disconnect();
    QWidget::hideEvent(event);
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
    QImage qImg = image->getImage();
    m_imageWidget->setImage( qImg );
    m_busy = false;
}
