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
#include "Add.h"

#include <QDebug>
#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

Add::Add() :
    m_alpha(0.5),
    m_beta(0.5),
    m_gamma(0)
{
    m_inputPin1 = createCvMatDataInputPin( "input A", this );
    m_inputPin2 = createCvMatDataInputPin( "input B", this );
    m_outputPin = createCvMatDataOutputPin( "output", this );

    m_inputPin1->addAllChannels();
    m_inputPin1->addAllDepths();

    m_inputPin2->addAllChannels();
    m_inputPin2->addAllDepths();

    m_outputPin->addAllChannels();
    m_outputPin->addAllDepths();
}

Add::~Add() {}

bool Add::process()
{
    // get the inputs
    CvMatData in1 = m_inputPin1->get();
    CvMatData in2 = m_inputPin2->get();

    //check format of images?
    if( in1.channels() != in2.channels() )
    {
        QString msg = tr("Images do not have same number of channels.");
        setError(PlvFatalError, msg);
        return false;
    }

    if( in1.depth() != in2.depth() )
    {
        QString msg = tr("Input images are not of the same depth. "
                      "Input 1 has depth %1 and input 2 has depth %2. " )
                .arg(CvMatData::depthToString(in1.depth()))
                .arg(CvMatData::depthToString(in2.depth()));
        setError(PlvFatalError, msg);
        return false;
    }

    CvMatData out = CvMatData::create(in1.properties());

    const cv::Mat& src1 = in1;
    const cv::Mat& src2 = in2;
    cv::Mat& dst = out;

    // does a weighted add
    cv::addWeighted( src1, m_alpha, src2, m_beta, m_gamma, dst );

    // publish the new image
    m_outputPin->put( out );

    return true;
}

double Add::getAlpha() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_alpha;
}

void Add::setAlpha( double a, bool doEmit )
{
    QMutexLocker lock( m_propertyMutex );
    m_alpha = a;
    if( doEmit ) emit( alphaChanged(a) );
}

double Add::getBeta() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_beta;
}

void Add::setBeta( double b, bool doEmit )
{
    QMutexLocker lock( m_propertyMutex );
    m_beta = b;
    if( doEmit ) emit( betaChanged(b) );
}

double Add::getGamma() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_gamma;
}

void Add::setGamma( double g, bool doEmit )
{
    QMutexLocker lock( m_propertyMutex );
    m_gamma = g;
    if( doEmit ) emit( gammaChanged(g) );
}




