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

#include <plvcore/Pin.h>
#include <plvcore/OpenCVImage.h>

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
    m_inputPin = createInputPin<OpenCVImage>( "input", this, IInputPin::INPUT_REQUIRED );
    m_inputPinOptional = createInputPin<OpenCVImage>( "input2", this, IInputPin::INPUT_OPTIONAL );
    m_outputPin = createOutputPin<OpenCVImage>( "output", this );

    m_customEnum.add( "Very Low" );
    m_customEnum.add( "Low" );
    m_customEnum.add( "High" );
    m_customEnum.add( "Very High" );
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

    RefPtr<OpenCVImage> srcPtr = m_inputPin->get();

    RefPtr<OpenCVImage> targetPtr = OpenCVImageFactory::get( srcPtr->getProperties() );

    // open for reading
    const IplImage* src = srcPtr->getImage();

    // open image for writing
    IplImage* target = targetPtr->getImageForWriting();

    // do a flip of the image
    cvFlip( src, target, (int)m_someBool);

    // publish the new image
    m_outputPin->put( targetPtr.getPtr() );

    this->setSomeInt(this->getSomeInt()+1);
}
