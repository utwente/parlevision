#include "CameraProducer.h"

#include <QMutexLocker>
#include <QDebug>

#include "OpenCVCamera.h"
#include "Pin.h"

using namespace plv;

#define OUTPUT_PIN_NAME "output"

CameraProducer::CameraProducer() :
        m_camera(new OpenCVCamera()),
        m_lastProcessedId( 0 )
{
    // we have one output pin
    m_outputPin = new OutputPin<OpenCVImage>(OUTPUT_PIN_NAME, this );
    addOutputPin( m_outputPin.getPtr() );

    // connect the camera to this camera producer using Qt's signals
    // and slots mechanism.
    connect( m_camera.getPtr(),
             SIGNAL( newFrame( RefPtr<Data> ) ),
             this,
             SLOT( newFrame( RefPtr<Data> ) ) );
}

CameraProducer::~CameraProducer()
{
    m_camera->release();
}

void CameraProducer::process()
{
    QMutexLocker lock(&m_frameMutex);

    if( m_lastFrame.isNotNull() )
    {
        m_outputPin->put( m_lastFrame.getPtr() );
        //m_lastProcessedId = m_lastFrame->getId();
    }
    else
    {
        qDebug() << "WARNING: CameraProducer::process() called too early";
    }

//    qDebug() << "Pin type: " << m_outputPin->getTypeInfo().name();
}

void CameraProducer::newFrame( RefPtr<Data> frame )
{
    QMutexLocker lock(&m_frameMutex);
    m_lastFrame = ref_ptr_dynamic_cast<OpenCVImage>( frame );
    m_frameReady.wakeAll();
}

PlvPipelineElementState CameraProducer::init()
{
    if( m_camera->init() != 0 )
    {
        return PLV_PLE_STATE_NOT_READY;
    }

    m_camera->setDimensions( 640, 480 );

    m_frameMutex.lock();

    m_camera->start();

    // Block until we receive the first frame from the camera,
    // indicating it has finished starting up.
    // this will let this thread wait on the event
    // and unlocks the mutex so no deadlock is created
    m_frameReady.wait(&m_frameMutex);

    // we have woken up
    // mutex was relocked by the condition
    // unlock it here
    m_frameMutex.unlock();

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

