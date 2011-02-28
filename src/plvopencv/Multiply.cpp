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

#include <QDebug>

#include "Multiply.h"

#include <plvcore/CvMatDataPin.h>
#include <plvcore/CvMatData.h>

using namespace plv;
using namespace plvopencv;

Multiply::Multiply()
{
    m_inputPin1 = createCvMatDataInputPin( "image_input A", this );
    m_inputPin2 = createCvMatDataInputPin( "image_input B", this );
    m_outputPin = createCvMatDataOutputPin( "image_output", this );

    m_scale = 1.0/255.0;
}

Multiply::~Multiply()
{
}

double Multiply::getScale()
{
    QMutexLocker lock( m_propertyMutex );
    return m_scale;
}

void Multiply::setScale(double d)
{
    QMutexLocker lock( m_propertyMutex );
    m_scale = d;
}

bool Multiply::process()
{
    plv::CvMatData img1 = m_inputPin1->get();
    plv::CvMatData img2 = m_inputPin2->get();

    if( img1.properties() != img2.properties() )
    {
        setError(PlvPipelineRuntimeError,"Input images do not have equal in properties");
        return false;
    }

    // open input images for reading
    const cv::Mat& mat1 = img1;
    const cv::Mat& mat2 = img2;

    //get a new output image of same depth and size as input image
    plv::CvMatData imgOut = CvMatData::create( img1.properties() );

    // open output image for writing
    cv::Mat& out = imgOut;

    //Multiply both images
    cv::multiply(mat1, mat2, out, m_scale);

    // publish the new image
    m_outputPin->put( imgOut );

    return true;
}

