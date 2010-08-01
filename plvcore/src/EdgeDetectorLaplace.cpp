#include <QDebug>

#include "EdgeDetectorLaplace.h"
#include "Pin.h"
#include "OpenCVImage.h"
#include <opencv/cv.h>

using namespace plv;

#define INPUT_PIN_NAME "input"
#define OUTPUT_PIN_NAME "output"

EdgeDetectorLaplace::EdgeDetectorLaplace() :
        m_apertureSize(3)
{
    m_inputPin = createInputPin<OpenCVImage>( INPUT_PIN_NAME, this );
    m_outputPin = createOutputPin<OpenCVImage>( OUTPUT_PIN_NAME, this );
}

EdgeDetectorLaplace::~EdgeDetectorLaplace()
{
}

void EdgeDetectorLaplace::init() throw (PipelineException)
{
}

bool EdgeDetectorLaplace::isReadyForProcessing() const
{
    return m_inputPin->hasData();
}

void EdgeDetectorLaplace::process()
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

    // perform laplace filter
    IplImage* tmpImg = tmp->getImageForWriting();
    cvLaplace( iplImg1, tmpImg, nearestOdd(this->m_apertureSize));

    // scale back to output format
    IplImage* iplImg2 = img2->getImageForWriting();
    cvConvertScale( tmpImg, iplImg2, 1, 0 );

    // publish the new image
    m_outputPin->put( img2.getPtr() );
}

void EdgeDetectorLaplace::setApertureSize(int i)
{
    m_apertureSize = i;
    emit(apertureSizeChanged(i));
}

int EdgeDetectorLaplace::nearestOdd(int i)
{
    return ( i%2 == 0 ? ++i : i );
}
