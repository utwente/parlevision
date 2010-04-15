#include "DummyProcessor.h"
#include "Pin.h"

using namespace plv;

#define INPUT_PIN_NAME "input image"
#define OUTPUT_PIN_NAME "output image"

DummyProcessor::DummyProcessor( Pipeline* parent ) :
        PipelineProcessor( parent )
{
}

DummyProcessor::~DummyProcessor()
{
}

PlvPipelineElementState DummyProcessor::init()
{
    return PROCESSOR_OK;
}

PlvPipelineElementState DummyProcessor::checkConfig()
{
    return PROCESSOR_OK;
}

void DummyProcessor::process()
{
    RefPtr<const Pin> in  = getInputPin( INPUT_PIN_NAME );
    RefPtr<const Pin> out = getOutputPin( OUTPUT_PIN_NAME );

    if( in.isValid() )
    {

    }

    if( out.isValid() )
    {

    }
}
