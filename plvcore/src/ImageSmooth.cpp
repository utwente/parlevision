#include <QDebug>

#include "ImageSmooth.h"
#include "Pin.h"
#include "OpenCVImage.h"
#include <opencv/cv.h>

using namespace plv;

#define INPUT_PIN_NAME "input"
#define OUTPUT_PIN_NAME "output"

ImageSmooth::ImageSmooth() :
        m_one(3),
        m_two(0),
        m_three(0.0),
        m_four(0.0)
{
    m_inputPin = new InputPin<OpenCVImage>( INPUT_PIN_NAME, this );
    addInputPin( m_inputPin );

    m_outputPin = new OutputPin<OpenCVImage>( OUTPUT_PIN_NAME, this );
    addOutputPin( m_outputPin );
}

ImageSmooth::~ImageSmooth()
{
}

void ImageSmooth::init() throw (PipelineException)
{
}

bool ImageSmooth::isReadyForProcessing() const
{
    return m_inputPin->hasData();
}

void ImageSmooth::process()
{
    assert(m_inputPin != 0);
    assert(m_outputPin != 0);

    RefPtr<OpenCVImage> img = m_inputPin->get();
    if(img->getDepth() != IPL_DEPTH_8U)
    {
        throw std::runtime_error("format not yet supported");
    }

    // temporary image with extra room (depth)
    RefPtr<OpenCVImage> tmp = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), IPL_DEPTH_16S , img->getNumChannels() );

    RefPtr<OpenCVImage> img2 = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), img->getDepth(), img->getNumChannels() );


    // open for reading
    const IplImage* iplImg1 = img->getImage();

    // open image for writing
    IplImage* iplImg2 = img2->getImageForWriting();

    // do a smooth operator of the image
    cvSmooth( iplImg1, iplImg2, (int)CV_GAUSSIAN, m_one, m_two, m_three, m_four);

    // publish the new image
    m_outputPin->put( img2.getPtr() );
}
