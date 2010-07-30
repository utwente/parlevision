#include <QDebug>

#include "ImageCanny.h"
#include "Pin.h"
#include "OpenCVImage.h"
#include <opencv/cv.h>

using namespace plv;

#define INPUT_PIN_NAME "input"
#define OUTPUT_PIN_NAME "output"

ImageCanny::ImageCanny() :
        m_apertureSize(3),
        m_thresholdLow(0.1),
        m_thresholdHigh(1.0)
{
    m_inputPin = createInputPin<OpenCVImage>( INPUT_PIN_NAME, this );
    m_outputPin = createOutputPin<OpenCVImage>( OUTPUT_PIN_NAME, this );
}

ImageCanny::~ImageCanny()
{
}

void ImageCanny::init() throw (PipelineException)
{
}

bool ImageCanny::isReadyForProcessing() const
{
    return m_inputPin->hasData();
}

void ImageCanny::process()
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
            img->getWidth(), img->getHeight(), IPL_DEPTH_8U , 1 );

    RefPtr<OpenCVImage> img2 = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), img->getDepth(), img->getNumChannels() );


    // open for reading
    const IplImage* iplImg1 = img->getImage();

    // open image for writing
    IplImage* iplImg2 = img2->getImageForWriting();
    IplImage* iplTmp = tmp->getImageForWriting();

    // INPUT REQUIRED TO BE GRAYSCALED! take the first channel as grayscale image
    cvSplit(iplImg1,iplTmp,NULL,NULL,NULL);

    // do a canny edge detection operator of the image
    cvCanny( iplTmp, iplTmp, m_thresholdLow, m_thresholdHigh, m_apertureSize);

    // convert the image back to 8bit depth
//    cvConvertScale(iplTmp, iplImg2, 1, 0);
    cvMerge( iplTmp, iplTmp, iplTmp, NULL, iplImg2 );

    // publish the new image
    m_outputPin->put( img2.getPtr() );
}
