#include <QDebug>

#include "DummyProcessor.h"
#include "Pin.h"
#include "OpenCVImage.h"

using namespace plv;

#define INPUT_PIN_NAME "input image"
#define OUTPUT_PIN_NAME "output image"

DummyProcessor::DummyProcessor()
{
    addInputPin( new TypedInputPin<OpenCVImage>( INPUT_PIN_NAME, this ) );
    addOutputPin( new TypedOutputPin<OpenCVImage>( OUTPUT_PIN_NAME, this ) );
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
    RefPtr<InputPin> in  = getInputPin( INPUT_PIN_NAME );
    RefPtr<OutputPin> out = getOutputPin( OUTPUT_PIN_NAME );

    assert(in.isValid());
    assert(out.isValid());
    qDebug() << "DummyProcessor recieved input";


    RefPtr<Data> data = in->get();
    if(data.isValid())
    {
        // It's safe to do this, because the pins are guaranteed to provide this type.
        RefPtr<OpenCVImage> img = ref_ptr_dynamic_cast<OpenCVImage>(data);
        assert(img.isValid());
        out->put(data);
    }
    else
    {
        qDebug() << "no data to process";
    }

}
