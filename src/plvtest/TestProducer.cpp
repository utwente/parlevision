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

bool TestProducer::init()
{
    return true;
}

bool TestProducer::deinit() throw ()
{
    return true;
}

bool TestProducer::start()
{
    return true;
}

bool TestProducer::stop()
{
    return true;
}

bool TestProducer::readyToProduce() const
{
    return true;
}

bool TestProducer::produce()
{
    unsigned int serial = getProcessingSerial();

    m_intOut->put(int(serial));
    m_stringOut->put(QString("%1").arg(serial));
    m_floatOut->put(float(serial));
    m_doubleOut->put(double(serial));

    return true;
}
