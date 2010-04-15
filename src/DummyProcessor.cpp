#include "DummyProcessor.h"
#include "Pin.h"

const char* INPUT_PIN_NAME = "input image";
const char* OUTPUT_PIN_NAME = "output image";

using namespace plv;

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
