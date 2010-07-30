#include <QDebug>

#include "ImageColorConvert.h"
#include "Pin.h"
#include "OpenCVImage.h"
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>

using namespace plv;

#define INPUT_PIN_NAME "input"
#define OUTPUT_PIN_NAME "output"

ImageColorConvert::ImageColorConvert() :
        m_conversionType( CV_RGB2GRAY )
{
    m_inputPin = createInputPin<OpenCVImage>( INPUT_PIN_NAME, this );
    m_outputPin = createOutputPin<OpenCVImage>( OUTPUT_PIN_NAME, this );
}

ImageColorConvert::~ImageColorConvert()
{
}

void ImageColorConvert::init() throw (PipelineException)
{
}

bool ImageColorConvert::isReadyForProcessing() const
{
    return m_inputPin->hasData();
}

void ImageColorConvert::process()
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
            img->getWidth(), img->getHeight(), img->getDepth(), 1 );

    RefPtr<OpenCVImage> img2 = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), img->getDepth(), img->getNumChannels() );


    // open for reading
    const IplImage* iplImg1 = img->getImage();

    // open image for writing
    IplImage* iplImg2 = img2->getImageForWriting();
    IplImage* iplTmp = tmp->getImageForWriting();

    cvCvtColor(iplImg1, iplTmp, m_conversionType);

    // merge the temporary image in the three channels of the output image
    cvMerge(iplTmp, iplTmp, iplTmp, NULL, iplImg2);

    // publish the new image
    m_outputPin->put( img2.getPtr() );
}
