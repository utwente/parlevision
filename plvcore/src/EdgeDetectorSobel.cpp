#include <QDebug>

#include "EdgeDetectorSobel.h"
#include "Pin.h"
#include "OpenCVImage.h"
#include <opencv/cv.h>

using namespace plv;

#define INPUT_PIN_NAME "input"
#define OUTPUT_PIN_NAME "output"

EdgeDetectorSobel::EdgeDetectorSobel()
{
    m_inputPin = createInputPin<OpenCVImage>( INPUT_PIN_NAME, this );
    m_outputPin = createOutputPin<OpenCVImage>( OUTPUT_PIN_NAME, this );
}

EdgeDetectorSobel::~EdgeDetectorSobel()
{
}

void EdgeDetectorSobel::init() throw (PipelineException)
{
}

bool EdgeDetectorSobel::isReadyForProcessing() const
{
    return m_inputPin->hasData();
}

void EdgeDetectorSobel::process()
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
    IplImage* iplTmp = tmp->getImageForWriting();

    // do a sobel operator of the image
    cvSobel( iplImg1, iplTmp, 1,0,3);
    // convert the image back to 8bit depth
    cvConvertScale(iplTmp, iplImg2, 1, 0);

    // publish the new image
    m_outputPin->put( img2.getPtr() );
}
