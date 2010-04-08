#ifndef CAMERAWINDOW_H_
#define CAMERAWINDOW_H_

#include <QWidget>
#include <QVBoxLayout>

#include "FrameWidget.h"

namespace plv {
    class OpenCVCamera;
}

#include <opencv/cv.h>

namespace plvgui {

class CameraWindow : public QWidget
{
    Q_OBJECT
    private:
        FrameWidget* m_cvWidget;
        plv::OpenCVCamera*  m_camera;
        
    public:
        CameraWindow( plv::OpenCVCamera* cam, QWidget* parent=0);

    public slots:
        void setFrame( const IplImage* frame );
};

}

#endif /* CAMERAWINDOW_H_*/
