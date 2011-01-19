#include "TestProducer.h"

#include <QDebug>
#include <plvcore/Pin.h>

using namespace plv;

TestProducer::TestProducer()
{
    m_intOut = createOutputPin<int>("int", this);
    m_stringOut = createOutputPin<QString>("QString", this);
    m_floatOut = createOutputPin<float>("float", this);
    m_doubleOut = createOutputPin<double>("double", this);
}

TestProducer::~TestProducer()
{
}

void TestProducer::init()
{
}

void TestProducer::deinit() throw ()
{
}

void TestProducer::start()
{
}

void TestProducer::stop()
{
}

bool TestProducer::readyToProduce() const
{
    return true;
}

void TestProducer::produce()
{
    unsigned int serial = getProcessingSerial();

    m_intOut->put(int(serial));
    m_stringOut->put(QString(serial));
    m_floatOut->put(float(serial));
    m_doubleOut->put(double(serial));

}
