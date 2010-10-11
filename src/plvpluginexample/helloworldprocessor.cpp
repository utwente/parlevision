#include "helloworldprocessor.h"

#include <QDebug>

#include "HelloWorldProcessor.h"
#include <plvcore/Pin.h>
#include <plvcore/OpenCVImage.h>
#include <opencv/cv.h>

using namespace plv;

HelloWorldProcessor::HelloWorldProcessor() :
        m_someInt(1337),
        m_someDouble(1.23456),
        m_someBool(true),
        m_someString("hello")
{
    m_inputPin = new InputPin<OpenCVImage>("input", this);
    addInputPin( m_inputPin );

    m_outputPin = new OutputPin<OpenCVImage>("output", this);
    addOutputPin( m_outputPin );
}

HelloWorldProcessor::~HelloWorldProcessor()
{
}

void HelloWorldProcessor::init()
{
}

void HelloWorldProcessor::deinit() throw ()
{
}

void HelloWorldProcessor::start()
{
}

void HelloWorldProcessor::stop()
{
}

void HelloWorldProcessor::process()
{
    assert(m_inputPin != 0);
    assert(m_outputPin != 0);

    RefPtr<OpenCVImage> img = m_inputPin->get();
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
