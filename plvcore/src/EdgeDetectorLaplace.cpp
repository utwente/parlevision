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

    // temporary image with extra room (depth), see e.g. http://www.emgu.com/wiki/files/1.5.0.0/Help/html/8b5dffff-5fa5-f3f1-acb4-9adbc60dd7fd.htm
    RefPtr<OpenCVImage> tmp = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), IPL_DEPTH_16S , img->getNumChannels() );

    RefPtr<OpenCVImage> img2 = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), img->getDepth(), img->getNumChannels() );


    // open for reading
    const IplImage* iplImg1 = img->getImage();

    // perform laplace filter
    IplImage* tmpImg = tmp->getImageForWriting();

    //FIXME: it seems that this is a openCV2.0 function; the 2.1 function looks different
    //see http://opencv.willowgarage.com/documentation/cpp/image_filtering.html
    cvLaplace( iplImg1, tmpImg, this->m_apertureSize);

    // scale back to output format
    IplImage* iplImg2 = img2->getImageForWriting();
    cvConvertScale( tmpImg, iplImg2, 1, 0 );

    // publish the new image
    m_outputPin->put( img2.getPtr() );
}

void EdgeDetectorLaplace::setApertureSize(int i)
{
    //aperture size must be odd and positive, max 31 (but that is already way too much for sensible results)
    if (i < 1) i = 1;
    if (i > 31) i = 31;
    if (i%2 == 0)
    {   //even: determine appropriate new odd value
        if (i > m_apertureSize) i++; //we were increasing -- increase to next odd value
        else i--;                    //we were decreasing -- decrease to next odd value
    }
    m_apertureSize = i;
    emit(apertureSizeChanged(m_apertureSize));
}
