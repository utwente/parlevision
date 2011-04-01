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

#include "ForegroundDetector.h"
#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <opencv/cv.h>
#include <opencv/cvaux.h>

using namespace plv;
using namespace plvopencv;

enum fgDetectionMethod {
    FG_ACMMM2003,
    FG_ACMMM2003_SIMPLE,
    FG_CVPR1999
};

ForegroundDetector::ForegroundDetector() : m_bgModel(0)
{
    m_inInput = createCvMatDataInputPin( "input", this, IInputPin::CONNECTION_REQUIRED, IInputPin::CONNECTION_SYNCHRONOUS );

    m_outForeground = createCvMatDataOutputPin( "foreground", this );
    m_outBackground = createCvMatDataOutputPin( "background", this );

    m_inInput->addSupportedChannels(3);
    m_inInput->addSupportedDepth(CV_8U);

    m_outForeground->addAllChannels();
    m_outForeground->addAllDepths();

    m_outBackground->addSupportedChannels(1);
    m_outBackground->addSupportedDepth(CV_8U);

    m_method.add("ACM MM2003", FG_ACMMM2003 );
    m_method.add("ACM MM2003 simple", FG_ACMMM2003 );
    m_method.add("CVPR 1999", FG_CVPR1999);
}

ForegroundDetector::~ForegroundDetector()
{
}

bool ForegroundDetector::start()
{
    return true;
}

bool ForegroundDetector::stop()
{
    if( m_bgModel != 0 )
    {
        m_bgModel->release(&m_bgModel);
        m_bgModel = 0;
    }
    return true;
}

bool ForegroundDetector::process()
{
    CvMatData in = m_inInput->get();
    const cv::Mat& mat_in = in;

    IplImage current_frame = mat_in;

    // do initialisation on first frame only
    if( m_bgModel == 0 )
    {
        switch( m_method.getSelectedValue() )
        {
        case FG_ACMMM2003:
        case FG_ACMMM2003_SIMPLE:
            m_bgModel = cvCreateFGDStatModel(&current_frame, 0);
            break;
        case FG_CVPR1999:
            m_bgModel = cvCreateGaussianBGModel(&current_frame, 0);
            break;
        }
        return true;
    }

    cvUpdateBGStatModel(&current_frame,m_bgModel);

    // copy the data so we have ownership
    CvMatData fg = m_bgModel->foreground;
    CvMatData bg = m_bgModel->background;

    m_outForeground->put(fg);
    m_outBackground->put(bg);

    return true;
}

void ForegroundDetector::setMethod(plv::Enum method)
{
    QMutexLocker lock(m_propertyMutex);
    m_method = method;
    emit methodChanged(method);
}

plv::Enum ForegroundDetector::getMethod() const
{
    QMutexLocker lock(m_propertyMutex);
    return m_method;
}
