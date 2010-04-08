#ifndef FrameWidget_H
#define FrameWidget_H

#include <opencv/cv.h>

class QLabel;
class QPixmap;
class QLabel;
class QVBoxLayout;
class QImage;

#include <QWidget>
#include "OpenCVCamera.h"
#include "OpenCVImage.h"

namespace plvgui {

class FrameWidget : public QWidget {
    Q_OBJECT

    private:
        QLabel*         m_imagelabel;
        QVBoxLayout*    m_layout;
        QImage          m_image;
        
    public:
        FrameWidget( QWidget *parent );
        ~FrameWidget();

        void putImage( plv::OpenCVImage* );

        /**
         * Connect this viewer to the given source
         */
        void setSource(plv::OpenCVCamera* cam);

    public slots:
        void setFrame( plv::OpenCVImage* frame );
}; 

}

#endif
