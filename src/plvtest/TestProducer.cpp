/**
  * Copyright (C)2011 by Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvtest plugin of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

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
