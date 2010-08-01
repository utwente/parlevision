#include <QDebug>

#include "ImageFlip.h"
#include "Pin.h"
#include "OpenCVImage.h"
#include <opencv/cv.h>

using namespace plv;

#define INPUT_PIN_NAME "input"
#define OUTPUT_PIN_NAME "output"

ImageFlip::ImageFlip() :
        m_flipX(true),
        m_flipY(true),
        m_method(-1)
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

    RefPtr<OpenCVImage> imgIn = m_inputPin->get();

    RefPtr<OpenCVImage> imgOut = OpenCVImageFactory::instance()->get(
            imgIn->getWidth(), imgIn->getHeight(), imgIn->getDepth(), imgIn->getNumChannels() );


    // open for reading
    const IplImage* iplImgIn = imgIn->getImage();

    // open image for writing
    IplImage* iplImgOut = imgOut->getImageForWriting();

    // do a flip of the image
    if (m_flipX || m_flipY) cvFlip( iplImgIn, iplImgOut, m_method);
    else cvCopy( iplImgIn, iplImgOut);

    // publish the new image
    m_outputPin->put( imgOut.getPtr() );

}



void ImageFlip::setFlipX(bool b)
{
    if (b)
    {
        if (m_flipY) m_method = -1;
        else m_method=0;
    }
    else
    {
        if (m_flipY) m_method = 1;
        else m_method=0;
    }


    m_flipX = b;
    emit(flipXChanged(m_flipX));
}

void ImageFlip::setFlipY(bool b)
{
    if (b)
    {
        if (m_flipX) m_method = -1;
        else m_method=1;
    }
    else
    {
        if (m_flipX) m_method = 0;
        else m_method=1;
    }


    m_flipY = b;
    emit(flipYChanged(m_flipY));
}
