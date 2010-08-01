#include <QDebug>

#include "ImageFlip.h"
#include "Pin.h"
#include "OpenCVImage.h"
#include <opencv/cv.h>

using namespace plv;

#define INPUT_PIN_NAME "input"
#define OUTPUT_PIN_NAME "output"

ImageFlip::ImageFlip() :
        m_apertureSize(3),
        m_someInt( 0 ),
        m_someBool( false )
{
    m_inputPin = createInputPin<OpenCVImage>( INPUT_PIN_NAME, this );
    m_outputPin = createOutputPin<OpenCVImage>( OUTPUT_PIN_NAME, this );
}

ImageFlip::~ImageFlip()
{
}

void ImageFlip::init() throw (PipelineException)
{
}

bool ImageFlip::isReadyForProcessing() const
{
    return m_inputPin->hasData();
}

void ImageFlip::process()
{
    assert(m_inputPin != 0);
    assert(m_outputPin != 0);

    RefPtr<OpenCVImage> img = m_inputPin->get();
    if(img->getDepth() != IPL_DEPTH_8U)
    {
        throw std::runtime_error("format not yet supported");
    }

    // temporary image with extra room (depth)
    //TODO: [DR]: why do we need the extra depth here?
    RefPtr<OpenCVImage> tmp = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), IPL_DEPTH_16S , img->getNumChannels() );

    RefPtr<OpenCVImage> img2 = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), img->getDepth(), img->getNumChannels() );


    // open for reading
    const IplImage* iplImg1 = img->getImage();

    // open image for writing
    IplImage* iplImg2 = img2->getImageForWriting();

    // do a flip of the image
    cvFlip( iplImg1, iplImg2, (int)m_someBool);

    // publish the new image
    m_outputPin->put( img2.getPtr() );

    this->setSomeInt(this->getSomeInt()+1);

}

void ImageFlip::setApertureSize(int i)
{
    m_apertureSize = i;
    emit(apertureSizeChanged(i));
}

int ImageFlip::nearestOdd(int i)
{
    return ( i%2 == 0 ? ++i : i );
}
