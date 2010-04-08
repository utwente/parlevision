#include "CameraWindow.h"
#include "OpenCVCamera.h"

using namespace plvgui;

CameraWindow::CameraWindow( plv::OpenCVCamera* cam, QWidget *parent) :
    QWidget(parent)
{
    m_camera = cam;
    m_cvWidget = new FrameWidget(this);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget( m_cvWidget );

    this->setLayout( layout );
    this->resize( m_camera->getWidth(), m_camera->getHeight() );

    connect( m_camera, SIGNAL( newFrame( const IplImage* ) ),
             m_cvWidget, SLOT( setFrame(const IplImage*) ) );

}
