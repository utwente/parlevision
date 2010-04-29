#include <QDebug>

#include "DummyProcessor.h"
#include "Pin.h"
#include "OpenCVImage.h"
#include <opencv/cv.h>

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

    assert(in.isNotNull());
    assert(out.isNotNull());
    qDebug() << "DummyProcessor recieved input";


    RefPtr<Data> data = in->get();
    if(data.isNotNull())
    {
        // It's safe to do this, because the pins are guaranteed to provide this type.
        RefPtr<OpenCVImage> img = ref_ptr_dynamic_cast<OpenCVImage>(data);
        assert(img.isNotNull());
        RefPtr<OpenCVImage> img2 = img->deepcopy();

//        IplImage* img2 = cvCreateImage( CvSize( img->getImage()->width, img->getImage()->height),
//                       img->getImage()->depth, img->getImage()->nChannels );
        cvFlip(img->getImage(), img2->getImage(), 1);
//        RefPtr<OpenCVImage> img2ref = new OpenCVImage( 0, img2 );
        RefPtr<Data> outData = ref_ptr_dynamic_cast<Data>(img2);
        out->put(outData);
    }
    else
    {
        qDebug() << "no data to process";
    }

}
