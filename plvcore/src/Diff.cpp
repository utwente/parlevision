#include <QDebug>

#include "Diff.h"
#include "Pin.h"
#include "OpenCVImage.h"
#include <opencv/cv.h>

using namespace plv;

#define INPUT_PIN_NAME1 "input 1"
#define INPUT_PIN_NAME2 "input 2"
#define OUTPUT_PIN_NAME "output"

Diff::Diff()
{
    m_inputPin1 = createInputPin<OpenCVImage>( INPUT_PIN_NAME1, this );
    m_inputPin2 = createInputPin<OpenCVImage>( INPUT_PIN_NAME2, this );
    m_outputPin = createOutputPin<OpenCVImage>( OUTPUT_PIN_NAME, this );
}

Diff::~Diff()
{
}

void Diff::init() throw (PipelineException)
{
}

bool Diff::isReadyForProcessing() const
{
    return (m_inputPin1->hasData() && m_inputPin2->hasData());
}

void Diff::process()
{
    assert(m_inputPin1 != 0);
    assert(m_inputPin2 != 0);
    assert(m_outputPin != 0);

    RefPtr<OpenCVImage> img1 = m_inputPin1->get();
    RefPtr<OpenCVImage> img2 = m_inputPin2->get();
    // open input images for reading
    const IplImage* iplImgIn1 = img1->getImage();
    const IplImage* iplImgIn2 = img2->getImage();
    CvSize size1=cvGetSize(iplImgIn1);
    CvSize size2=cvGetSize(iplImgIn2);
    //check format of images?
    if(   img1->getDepth() != img2->getDepth()
       ||
          img1->getNumChannels() != img2->getNumChannels()
        ||
           size1.height != size2.height
        ||
           size1.width != size2.width
       )
    {
        //TODO: we could use some modifications when the images do not match -- e.g., copy one of the mismatching images into a duplicate that DOES match (stretch? shrink? Diff depth?)
        throw std::runtime_error("The two images need to be the same in depth, size and nr of channels");
    }


    //get a new output image of same depth and size as input image
    RefPtr<OpenCVImage> imgOut = OpenCVImageFactory::instance()->get(
            img1->getWidth(), img1->getHeight(), img1->getDepth(), img1->getNumChannels() );

    // open output image for writing
    IplImage* iplImgOut = imgOut->getImageForWriting();

    cvAbsDiff(iplImgIn1,iplImgIn2,iplImgOut);

    // publish the new image
    m_outputPin->put( imgOut.getPtr() );
}

