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

#include "DummyProcessor.h"

#include <plvcore/CvMatDataPin.h>
#include <plvcore/CvMatData.h>

#include <QDebug>

using namespace plv;
using namespace plvopencv;

/**
  * This processor is mainly for showing the code with which one creates pins and properties and such */
DummyProcessor::DummyProcessor() :
        m_someInt(1337),
        m_someDouble(1.23456),
        m_someBool(true),
        m_someString("hello"),
        m_someVarWithNr1(0),
        m_someVarWithNr2(0)
{
    m_inputPin = createCvMatDataInputPin( "input", this, IInputPin::INPUT_REQUIRED );
    m_inputPinOptional = createCvMatDataInputPin( "input2", this, IInputPin::INPUT_OPTIONAL );
    m_outputPin = createCvMatDataOutputPin( "output", this );

    m_customEnum.addLast( "Very Low" );
    m_customEnum.addLast( "Low" );
    m_customEnum.addLast( "High" );
    m_customEnum.addLast( "Very High" );

    m_inputPin->addAllChannels();
    m_inputPin->addAllDepths();

    m_inputPinOptional->addAllChannels();
    m_inputPinOptional->addAllDepths();

    m_outputPin->addAllChannels();
    m_outputPin->addAllDepths();
}

DummyProcessor::~DummyProcessor()
{
}

void DummyProcessor::init()
{
}

void DummyProcessor::deinit() throw()
{
}

void DummyProcessor::start()
{
}

void DummyProcessor::stop()
{
}

void DummyProcessor::process()
{
    assert(m_inputPin != 0);
    assert(m_outputPin != 0);

    CvMatData src = m_inputPin->get();

    // allocate a target buffer
    CvMatData target = CvMatData::create( src.getWidth(), src.getHeight(), src.getType() );

    // do a flip of the image
    const cv::Mat in = src;
    cv::Mat out = target;
    cv::flip( in, out, (int)m_someBool);

    // publish the new image
    m_outputPin->put( target );

    // update our "frame counter"
    this->setSomeInt(this->getSomeInt()+1);
}
