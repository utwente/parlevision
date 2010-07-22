#include "CameraProducer.h"

#include <QMutexLocker>
#include <QDebug>

#include "OpenCVCamera.h"
#include "Pin.h"

using namespace plv;

#define OUTPUT_PIN_NAME "output"

CameraProducer::CameraProducer() :
        m_camera( new OpenCVCamera() ),
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

CameraProducer::CameraProducer(const CameraProducer& other):
        PipelineProducer(other),
        m_camera(other.m_camera),
        m_lastProcessedId( other.m_lastProcessedId )
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
    //m_frameReady.wakeAll();
}

void CameraProducer::init() throw (PipelineException)
{
}

void CameraProducer::start() throw (PipelineException)
{
    if( !m_camera->init() )
    {
        throw std::runtime_error("Camera failed to initialise");
    }

    m_camera->setDimensions( 640, 480 );

    m_frameMutex.lock();

    m_camera->start();

    // Block until we receive the first frame from the camera,
    // indicating it has finished starting up.
    // this will let this thread wait on the event
    // and unlocks the mutex so no deadlock is created
    //m_frameReady.wait(&m_frameMutex);

    // we have woken up
    // mutex was relocked by the condition
    // unlock it here
    m_frameMutex.unlock();
}

void CameraProducer::stop() throw (PipelineException)
{
    m_camera->release();
}

//bool CameraProducer::isBootstrapped() const
//{
//    // TODO
//    return true;
//}

bool CameraProducer::isReadyForProcessing() const
{
    // TODO
    return true;
}
