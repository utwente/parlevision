/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvcore module of ParleVision.
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

#include "ExampleProcessor.h"

#include <plvcore/CvMatDataPin.h>
#include <plvcore/CvMatData.h>

#include <QDebug>
#include <QTime>

using namespace plv;
using namespace plvopencv;

/** This processor is mainly for showing the code with which one creates
    pins and properties and such */
ExampleProcessor::ExampleProcessor() :
        m_someInt(1337),
        m_someDouble(1.23456),
        m_someBool(true),
        m_someString("hello")
{
    // this pin needs to be connected, or process() will not be called
    m_inputPin = createCvMatDataInputPin( "input", this, IInputPin::CONNECTION_REQUIRED );

    // this pin does not have to be connected for process() to be called
    // we have to check it manually
    // if input is ready on this pin, we are required to remove it
    m_inputPinOptional = createCvMatDataInputPin( "input2", this, IInputPin::CONNECTION_OPTIONAL );

    // here we can publish the ouput of this processor
    // we do not have to publish output, if no output is published
    // a NULL message is sent to the connected processors
    m_outputPin = createCvMatDataOutputPin( "output", this );

    // the pins get added to the PipelineElement base class. This class makes sure
    // they are deleted on destruction of this class. We do not have to do this ourselves.
    // Also, the instance variables can be simple pointers, no smart pointers are
    // necessary.
    // The instance variables for the pins are only for convenience. We could also
    // get them like this:
    // IOutputPin* example = this->getOutputPin("output");

    // we fill our enum, first item is the default
    // items have and index, an QString id, and a value
    m_customEnum.add( "Very Low" ); // implicit value = 0
    m_customEnum.add( "Low", 1 ); // explicit value = 1
    m_customEnum.add( "High" ); // implicit value = 2
    m_customEnum.add( "Very High", 5 ); // explicit value = 5
    m_customEnum.add( "High" ); // does NOT replace first High. Implicit value = 4

    // we can change the default by calling
    m_customEnum.setSelected( "Low" );

    m_inputPin->addAllChannels();
    m_inputPin->addAllDepths();

    m_inputPinOptional->addAllChannels();
    m_inputPinOptional->addAllDepths();

    m_outputPin->addAllChannels();
    m_outputPin->addAllDepths();

    // random seed
    QTime midnight(0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));
}

ExampleProcessor::~ExampleProcessor()
{
    // pins are automatically destructed
}

bool ExampleProcessor::init()
{
    return true;
}

bool ExampleProcessor::deinit() throw()
{
    return true;
}

bool ExampleProcessor::start()
{
    return true;
}

bool ExampleProcessor::stop()
{
    return true;
}

bool ExampleProcessor::process()
{
    assert(m_inputPin != 0);
    assert(m_outputPin != 0);

    CvMatData src = m_inputPin->get();

    // allocate a target buffer
    CvMatData target = CvMatData::create( src.width(), src.height(), src.type() );

    // do a flip of the image
    const cv::Mat in = src;
    cv::Mat out = target;
    cv::flip( in, out, (int)m_someBool);

    // publish the new image
    m_outputPin->put( target );

    // update our "frame counter"
    // the property mutex has been locked for us in the
    // __process method. It is recursive, so we can call
    // setters here.
    // we need to broadcast our change, so we can
    // choose ...
    int choice = qrand() % 4; // choice = [0,4)
    switch(choice)
    {
    case 0:
        // update already emits
        updateSomeInt(m_someInt+1);
        break;
    // or
    case 1:
        // no emit, do it manually
        ++m_someInt;
        emit someIntChanged(m_someInt);
        break;
    // or
    case 2:
        // setSomeInt won't emit()
        setSomeInt(getSomeInt()+1);
        emit someIntChanged( getSomeInt() );
        break;
    // or (slow!)
    case 3:
        // setProperty won't emit()
        this->setProperty("someInt", m_someInt + 1 );
        emit someIntChanged( m_someInt );
        break;
    }

    return true;
}

void ExampleProcessor::setSomeString(QString s)
{
    QMutexLocker lock( m_propertyMutex );
    m_someString = s;
}

QString ExampleProcessor::getSomeString()
{
    QMutexLocker lock( m_propertyMutex );
    return m_someString;
}

int ExampleProcessor::getSomeInt()
{
    QMutexLocker lock( m_propertyMutex );
    return m_someInt;
}

double ExampleProcessor::getSomeDouble()
{
    QMutexLocker lock( m_propertyMutex );
    return m_someDouble;
}

bool ExampleProcessor::getSomeBool()
{
    QMutexLocker lock( m_propertyMutex );
    return m_someBool;
}

plv::Enum ExampleProcessor::getCustomEnum() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_customEnum;
}

void ExampleProcessor::setSomeInt(int i)
{
    QMutexLocker lock( m_propertyMutex );
    m_someInt = i;
}

void ExampleProcessor::setSomeDouble(double d)
{
    QMutexLocker lock( m_propertyMutex );
    m_someDouble = d;
}

void ExampleProcessor::setSomeBool(bool b)
{
    QMutexLocker lock( m_propertyMutex );
    m_someBool = b;
}

void ExampleProcessor::setCustomEnum( plv::Enum c )
{
    QMutexLocker lock( m_propertyMutex );
    m_customEnum = c;
}
