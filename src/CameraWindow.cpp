#include "CameraWindow.h"
#include "OpenCVCamera.h"

CameraWindow::CameraWindow(OpenCVCamera* cam, QWidget *parent) : QWidget(parent)
{
    m_camera = cam;
    m_cvWidget = new QOpenCVWidget(this);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget( m_cvWidget );

    this->setLayout( layout );
    this->resize( m_camera->getWidth(), m_camera->getHeight() );

    connect( m_camera, SIGNAL( newFrame( const IplImage* ) ),
             this, SLOT( setFrame(const IplImage*) ) );

    //startTimer( (int)( 1000 / 30.0 ));
 }

//void CameraWindow::timerEvent(QTimerEvent*)
//{
//    const IplImage* image = m_camera->getFrame();
//    m_cvWidget->putImage( image );
//}

void CameraWindow::setFrame( const IplImage* frame )
{
    m_cvWidget->putImage( frame );
}

