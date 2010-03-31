#ifndef QOPENCVWIDGET_H
#define QOPENCVWIDGET_H

#include <opencv/cv.h>

class QLabel;
class QPixmap;
class QLabel;
class QVBoxLayout;
class QImage;

#include <QWidget>

class QOpenCVWidget : public QWidget {
    private:
        QLabel*         m_imagelabel;
        QVBoxLayout*    m_layout;
        QImage          m_image;
        
    public:
        QOpenCVWidget( QWidget *parent );
        ~QOpenCVWidget();

        void putImage( const IplImage* );
}; 

#endif
