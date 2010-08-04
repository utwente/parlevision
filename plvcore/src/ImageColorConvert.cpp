#include <QDebug>

#include "ImageColorConvert.h"
#include "Pin.h"
#include "OpenCVImage.h"
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>

using namespace plv;

/* Constants for color conversion
CV_BGR2BGRA
CV_RGB2RGBA
CV_BGRA2BGR
CV_RGBA2RGB

CV_BGR2RGBA
CV_RGB2BGRA

CV_RGBA2BGR
CV_BGRA2RGB

CV_BGR2RGB
CV_RGB2BGR

CV_BGRA2RGBA
CV_RGBA2BGRA

CV_BGR2GRAY
CV_RGB2GRAY
CV_GRAY2BGR
CV_GRAY2RGB
CV_GRAY2BGRA
CV_GRAY2RGBA
CV_BGRA2GRAY
CV_RGBA2GRAY

CV_BGR2BGR565
CV_RGB2BGR565
CV_BGR5652BGR
CV_BGR5652RGB
CV_BGRA2BGR565
CV_RGBA2BGR565
CV_BGR5652BGRA
CV_BGR5652RGBA

CV_GRAY2BGR565
CV_BGR5652GRAY

CV_BGR2BGR555
CV_RGB2BGR555
CV_BGR5552BGR
CV_BGR5552RGB
CV_BGRA2BGR555
CV_RGBA2BGR555
CV_BGR5552BGRA
CV_BGR5552RGBA

CV_GRAY2BGR555
CV_BGR5552GRAY

CV_BGR2XYZ
CV_RGB2XYZ
CV_XYZ2BGR
CV_XYZ2RGB

CV_BGR2YCrCb
CV_RGB2YCrCb
CV_YCrCb2BGR
CV_YCrCb2RGB

CV_BGR2HSV
CV_RGB2HSV

CV_BGR2Lab
CV_RGB2Lab

CV_BayerBG2BGR
CV_BayerGB2BGR
CV_BayerRG2BGR
CV_BayerGR2BGR

CV_BayerBG2RGB
CV_BayerGB2RGB
CV_BayerRG2RGB
CV_BayerGR2RGB

CV_BGR2Luv
CV_RGB2Luv
CV_BGR2HLS
CV_RGB2HLS

CV_HSV2BGR
CV_HSV2RGB

CV_Lab2BGR
CV_Lab2RGB
CV_Luv2BGR
CV_Luv2RGB
CV_HLS2BGR
CV_HLS2RGB
*/

#define INPUT_PIN_NAME "input"
#define OUTPUT_PIN_NAME "output"

#define PLV_ENUM_ADD( plvEnum, type ) plvEnum.add( #type, type )

ImageColorConvert::ImageColorConvert()
{
    m_inputPin = createInputPin<OpenCVImage>( INPUT_PIN_NAME, this );
    m_outputPin = createOutputPin<OpenCVImage>( OUTPUT_PIN_NAME, this );

    // first one added is default
    PLV_ENUM_ADD( m_conversionType, CV_RGB2GRAY );
    PLV_ENUM_ADD( m_conversionType, CV_RGB2RGBA );
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

    cvCvtColor(iplImg1, iplTmp, m_conversionType.getSelectedValue());

    // merge the temporary image in the three channels of the output image
    cvMerge(iplTmp, iplTmp, iplTmp, NULL, iplImg2);

    // publish the new image
    m_outputPin->put( img2.getPtr() );
}
