#include "ImageWidget.h"

#include <QImage>
#include <QPainter>
#include <QSize>

using namespace plvgui;

ImageWidget::ImageWidget(QWidget *parent) :
        QWidget(parent)
{
    m_zoomToFit = true;
}

void ImageWidget::setImage(const QImage &img)
{
    m_image = img;
    m_aspectRatio = img.width() / (float)img.height();

    if( m_zoomToFit )
    {
        computeZoomFactorToFitImage();
    }
    else
    {
        m_zoomFactor = 1.0;
    }

    // resize and update widget
    resize( sizeHint() );
    update();
}

void ImageWidget::paintEvent(QPaintEvent *)
{
    QRect rect = this->rect();
    int x,y,w,h;

    w = m_image.width() * m_zoomFactor;
    h = m_image.height()* m_zoomFactor;
    x = (rect.width() - w ) / 2;
    y = (rect.height()- h ) / 2;
    x = qMax(x,0);
    y = qMax(y,0);

    QRectF target(x,y,w,h);
    QPainter p(this);
    p.drawImage(target, m_image);
}

QImage ImageWidget::getImage()
{
    return m_image;
}

void ImageWidget::setZoomFactor( float zf )
{
    m_zoomFactor = zf;
    //updateGeometry();
    update();
    adjustSize();
}

QSize ImageWidget::sizeHint() const
{
    if( !m_image.isNull() )
    {
        QSize hint = m_image.size();
        hint = m_zoomFactor * hint;
        return hint;
    }
    return minimumSize();
}

void ImageWidget::setZoomToFit( bool zoomToFit )
{
    m_zoomToFit = zoomToFit;
    if( zoomToFit)
    {
        computeZoomFactorToFitImage();
    }
    else
    {
        m_zoomFactor = 1.0;
    }

    // resize and update widget
    resize( sizeHint() );
    update();
}

void ImageWidget::computeZoomFactorToFitImage()
{
    QRect rect = this->rect();
    float ratio = rect.width() / (float)rect.height();

    if( ratio > m_aspectRatio )
    {
        m_zoomFactor= rect.height() / (float)m_image.height();
    }
    else
    {
        m_zoomFactor = rect.width() / (float)m_image.width();
    }
}
