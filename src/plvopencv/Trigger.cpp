/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvopencv module of ParleVision.
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
#include "Trigger.h"

#include <QDebug>
#include <plvcore/Pin.h>
#include <opencv/cv.h>

#include "OpenCVImage.h"

using namespace plv;
using namespace plvopencv;

/**
 * Constructor.
 */
Trigger::Trigger() :
        m_activate(false),
        m_continuous(false)
{
    m_inputPin  = createInputPin<plv::Data>("input", this, IInputPin::INPUT_REQUIRED );
    m_outputPin = createOutputPin<PlvBoolean>( "trigger", this );
}

/**
 * Destructor.
 */
Trigger::~Trigger()
{
}

/** Mandatory methods. Has nothing to do here. Yet? */
void Trigger::init()  {}
void Trigger::deinit() throw(){}
void Trigger::start()  {}
void Trigger::stop()  {}

/**
 * Check if the image isn't NULL. Get a new activation trigger data, with
 * the same value as the activation property. If continuous isn't true then
 * the activation property is automatically set to false.
 */
void Trigger::process()
{
    assert(m_inputPin != 0);

    // do this to decrease queue at input pin
    m_inputPin->get();

    //send a trigger to the output with the value of activate
    m_outputPin->put( new PlvBoolean(true) );

    //check if it is continuous or not
    if(m_activate && !m_continuous) setActivate(false);
}
