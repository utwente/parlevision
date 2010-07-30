#include <QDebug>

#include "ImageCornerHarris.h"
#include "Pin.h"
#include "OpenCVImage.h"
#include <opencv/cv.h>

using namespace plv;

#define INPUT_PIN_NAME "input"
#define OUTPUT_PIN_NAME "output"

ImageCornerHarris::ImageCornerHarris() :
        m_apertureSize(3),
        m_blockSize(3),
        m_k(0.04)
{
    m_inputPin = new InputPin<OpenCVImage>( INPUT_PIN_NAME, this );
    addInputPin( m_inputPin );

    m_outputPin = new OutputPin<OpenCVImage>( OUTPUT_PIN_NAME, this );
    addOutputPin( m_outputPin );
}

ImageCornerHarris::~ImageCornerHarris()
{
}

void ImageCornerHarris::init() throw (PipelineException)
{
}

bool ImageCornerHarris::isReadyForProcessing() const
{
    return m_inputPin->hasData();
}

void ImageCornerHarris::process()
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
    RefPtr<OpenCVImage> tmp2 = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), IPL_DEPTH_32F , 1 );

    RefPtr<OpenCVImage> img2 = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), IPL_DEPTH_32F, img->getNumChannels() );
    RefPtr<OpenCVImage> img3 = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), img->getDepth(), img->getNumChannels() );


    // open for reading
    const IplImage* iplImg1 = img->getImage();

    // open image for writing, lots of unused images
    IplImage* iplImg2 = img2->getImageForWriting();
    IplImage* iplImg3 = img3->getImageForWriting();
    IplImage* iplTmp = tmp->getImageForWriting();
    IplImage* iplTmp2 = tmp2->getImageForWriting();

    // INPUT REQUIRED TO BE GRAYSCALED! take the first channel as grayscale image
    cvSplit(iplImg1,iplTmp,NULL,NULL,NULL);

    // do a canny edge detection operator of the image
    cvCornerHarris( iplTmp, iplTmp2, m_blockSize, m_apertureSize, m_k);

    // convert the image back to 8bit depth
    cvMerge( iplTmp2, iplTmp2, iplTmp2, NULL, iplImg2 );
    cvConvertScale(iplImg2, iplImg3, 1, 0);

    // publish the new image
    m_outputPin->put( img3.getPtr() );
}
