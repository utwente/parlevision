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
    //definePin( OUTPUT_PIN_NAME, OpenCVImage::getType() );
    addOutputPin( new TypedPin<OpenCVImage>(OUTPUT_PIN_NAME, this ) );
}

CameraProducer::~CameraProducer()
{
    connect( m_camera.getPtr(), SIGNAL( newFrame( OpenCVImage* ) ),
             this, SLOT( newFrame( OpenCVImage*) ) );
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
    return PROCESSOR_OK;
}

PlvPipelineElementState CameraProducer::checkConfig()
{
    return PROCESSOR_OK;
}

