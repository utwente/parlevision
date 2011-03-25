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

#include <QDebug>

#include "DilateErode.h"
#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>

using namespace plv;
using namespace plvopencv;

enum ErosionDelationOrder {
    BGDEO_ERODE_DELATE,
    BGDEO_DELATE_ERODE
};

DilateErode::DilateErode():
        m_erosionIterations(0),
        m_dilationIterations(0)
{
    m_inputPin = createCvMatDataInputPin( "input", this );
    m_inputPin->addAllChannels();
    m_inputPin->addAllDepths();

    m_outputPin = createCvMatDataOutputPin( "output", this );
    m_outputPin->addAllChannels();
    m_outputPin->addAllDepths();

    m_delationErosionOrder.add("erode then delate", BGDEO_ERODE_DELATE);
    m_delationErosionOrder.add("delate then erode", BGDEO_DELATE_ERODE);
}

DilateErode::~DilateErode()
{
}

bool DilateErode::process()
{
    CvMatData in = m_inputPin->get();
    CvMatData out = CvMatData::create(in.properties());
    CvMatData out2 = CvMatData::create(in.properties());

    cv::Mat element;
    cv::Point point(-1,-1);

    switch( m_delationErosionOrder.getSelectedValue() )
    {
    // first dilation, then erosion
    case BGDEO_ERODE_DELATE:
        // first erosion, then dilation
        cv::erode(in, out, element, point, m_erosionIterations);
        cv::dilate(out, out2, element, point, m_dilationIterations);
        break;
    case BGDEO_DELATE_ERODE:
        cv::dilate(in, out, element, point, m_dilationIterations);
        cv::erode(out, out2, element, point, m_erosionIterations);
        break;
    }
    m_outputPin->put(out2);
    return true;
}

int DilateErode::getErosionIterations() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_erosionIterations;
}

int DilateErode::getDilationIterations() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_dilationIterations;
}

plv::Enum DilateErode::getOrder() const
{
    QMutexLocker lock(m_propertyMutex);
    return m_delationErosionOrder;
}

void DilateErode::setErosionIterations(int e)
{
    QMutexLocker lock( m_propertyMutex );
    if( e > 0 )
        m_erosionIterations = e;
    emit erosionIterationsChanged(m_erosionIterations);
}

void DilateErode::setDilationIterations(int d)
{
    QMutexLocker lock( m_propertyMutex );
    if( d > 0 )
        m_dilationIterations = d;
    emit dilationIterationsChanged(m_dilationIterations);
}

void DilateErode::setOrder(plv::Enum order)
{
    QMutexLocker lock(m_propertyMutex);
    m_delationErosionOrder = order;
    emit orderChanged(order);
}
