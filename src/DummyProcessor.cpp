#include <QDebug>

#include "DummyProcessor.h"
#include "Pin.h"
#include "OpenCVImage.h"

using namespace plv;

#define INPUT_PIN_NAME "input image"
#define OUTPUT_PIN_NAME "output image"

DummyProcessor::DummyProcessor( Pipeline* parent ) :
        PipelineProcessor( parent )
{
    addInputPin( new TypedPin<OpenCVImage>( INPUT_PIN_NAME, this ) );
    addOutputPin( new TypedPin<OpenCVImage>( OUTPUT_PIN_NAME, this ) );
}

DummyProcessor::~DummyProcessor()
{
}

PlvPipelineElementState DummyProcessor::init()
{
    return PLV_PLE_STATE_READY;
}

PlvPipelineElementState DummyProcessor::checkConfig()
{
    return PLV_PLE_STATE_READY;
}

void DummyProcessor::process()
{
    RefPtr<const Pin> in  = getInputPin( INPUT_PIN_NAME );
    RefPtr<const Pin> out = getOutputPin( OUTPUT_PIN_NAME );

    if( in.isValid() && out.isValid() )
    {
        qDebug() << "DummyProcessor recieved input";
    }

}
