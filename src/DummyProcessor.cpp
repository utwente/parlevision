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
    m_inputPin = new TypedInputPin<OpenCVImage>( INPUT_PIN_NAME, this );
    addInputPin( m_inputPin );

    m_outputPin = new TypedOutputPin<OpenCVImage>( OUTPUT_PIN_NAME, this );
    addOutputPin( m_outputPin );
}

DummyProcessor::~DummyProcessor()
{
    delete m_inputPin;
    delete m_outputPin;
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

    assert(m_inputPin != 0);
    assert(m_outputPin != 0);


    RefPtr<OpenCVImage> img = m_inputPin->get();

    if(img.isNotNull())
    {
        RefPtr<OpenCVImage> img2 =
                OpenCVImageFactory::instance()->get( img->getWidth(), img->getHeight(),
                                                     img->getDepth(), img->getNumChannels() );
        // open image for writing
        OpenCVImageWriter writer( img2 );
        IplImage* iplImg2 = writer.get();

        // open for reading
        const IplImage* iplImg1 = img->getImage();

        // do a flip of the image
        cvFlip( iplImg1, iplImg2, 1);

        // publish the new image
        RefPtr<Data> outData = ref_ptr_dynamic_cast<Data>(img2);
        m_outputPin->put(outData);
    }
    else
    {
//        qDebug() << "no data to process";
    }

}
