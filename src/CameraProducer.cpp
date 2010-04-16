#include "CameraProducer.h"

#include <QMutexLocker>
#include <QDebug>

#include "OpenCVCamera.h"
#include "Pin.h"

using namespace plv;

#define OUTPUT_PIN_NAME "output"

CameraProducer::CameraProducer( Pipeline* parent ) :
        PipelineProducer( parent ),
        m_camera(new OpenCVCamera())
{
    // we have one output pin
    addOutputPin( new TypedPin<OpenCVImage>(OUTPUT_PIN_NAME, this ) );

    // connect the camera to this camera producer using Qt's signals
    // and slots mechanism.
    connect( m_camera.getPtr(), SIGNAL( newFrame( OpenCVImage* ) ),
             this, SLOT( newFrame( OpenCVImage*) ) );
}

CameraProducer::~CameraProducer()
{

}

void CameraProducer::produce()
{
    QMutexLocker lock(&m_frameMutex);

    RefPtr<const Pin> output = getOutputPin( OUTPUT_PIN_NAME );
    if( output.isValid() )
    {
        output->getPinConnection();//...
        qDebug() << "Pin type: " << output->getType().name();
    }
}

void CameraProducer::newFrame(OpenCVImage* frame)
{
    QMutexLocker lock(&m_frameMutex);
    m_lastFrame = frame;
}

PlvPipelineElementState CameraProducer::init()
{
    if( !m_camera->init() )
    {
        return PLV_PLE_STATE_NOT_READY;
    }

    m_camera->start();

    return PLV_PLE_STATE_READY;
}

PlvPipelineElementState CameraProducer::checkConfig()
{
    if( m_camera->getState() != OpenCVCamera::CAM_RUNNING )
    {
        return PLV_PLE_STATE_NOT_READY;
    }
    return PLV_PLE_STATE_READY;
}

