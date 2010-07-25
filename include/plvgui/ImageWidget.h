#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>

class QSize;
class QImage;

namespace plvgui
{
    class ImageWidget : public QWidget
    {
        Q_OBJECT
    public:
        explicit ImageWidget( QWidget *parent = 0 );
        void setImage( const QImage &image );
        QImage getImage();
        float getZoomFactor();
        void setZoomFactor( float zoomFactor );
        void setZoomToFit( bool zoomToFit );

    protected:
        QImage m_image;
        float  m_zoomFactor;
        float  m_aspectRatio;
        bool   m_zoomToFit;

        /** Custom paint method inherited from QWidget */
        void paintEvent( QPaintEvent * );

        /** Iherited from QWidget. Returns our preferred size */
        QSize sizeHint() const;

        void computeZoomFactorToFitImage();
    };
}

#endif // IMAGEWIDGET_H
