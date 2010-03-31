#ifndef CAMERAWINDOW_H_
#define CAMERAWINDOW_H_

#include <QWidget>
#include <QVBoxLayout>

#include "QOpenCVWidget.h"

class OpenCVCamera;

#include <opencv/cv.h>

class CameraWindow : public QWidget
{
    Q_OBJECT
    private:
        QOpenCVWidget* m_cvWidget;
        OpenCVCamera*  m_camera;
        
    public:
        CameraWindow(OpenCVCamera* cam, QWidget* parent=0);

    public slots:
        void setFrame( const IplImage* frame );
         
//    protected:
//        void timerEvent(QTimerEvent*);
};


#endif /* CAMERAWINDOW_H_*/
