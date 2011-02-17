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
#include <QMutexLocker>

#include "ImageColorConvert.h"
#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>

using namespace plv;
using namespace plvopencv;

/* Constants for color conversion
CV_BGR2BGRA
CV_RGB2RGBA
CV_BGRA2BGR
CV_RGBA2RGB

CV_BGR2RGBA
CV_RGB2BGRA

CV_RGBA2BGR
CV_BGRA2RGB

CV_BGR2RGB
CV_RGB2BGR

CV_BGRA2RGBA
CV_RGBA2BGRA

CV_BGR2GRAY
CV_RGB2GRAY
CV_GRAY2BGR
CV_GRAY2RGB
CV_GRAY2BGRA
CV_GRAY2RGBA
CV_BGRA2GRAY
CV_RGBA2GRAY

CV_BGR2BGR565
CV_RGB2BGR565
CV_BGR5652BGR
CV_BGR5652RGB
CV_BGRA2BGR565
CV_RGBA2BGR565
CV_BGR5652BGRA
CV_BGR5652RGBA

CV_GRAY2BGR565
CV_BGR5652GRAY

CV_BGR2BGR555
CV_RGB2BGR555
CV_BGR5552BGR
CV_BGR5552RGB
CV_BGRA2BGR555
CV_RGBA2BGR555
CV_BGR5552BGRA
CV_BGR5552RGBA

CV_GRAY2BGR555
CV_BGR5552GRAY

CV_BGR2XYZ
CV_RGB2XYZ
CV_XYZ2BGR
CV_XYZ2RGB

CV_BGR2YCrCb
CV_RGB2YCrCb
CV_YCrCb2BGR
CV_YCrCb2RGB

CV_BGR2HSV
CV_RGB2HSV

CV_BGR2Lab
CV_RGB2Lab

CV_BayerBG2BGR
CV_BayerGB2BGR
CV_BayerRG2BGR
CV_BayerGR2BGR

CV_BayerBG2RGB
CV_BayerGB2RGB
CV_BayerRG2RGB
CV_BayerGR2RGB

CV_BGR2Luv
CV_RGB2Luv
CV_BGR2HLS
CV_RGB2HLS

CV_HSV2BGR
CV_HSV2RGB

CV_Lab2BGR
CV_Lab2RGB
CV_Luv2BGR
CV_Luv2RGB
CV_HLS2BGR
CV_HLS2RGB
*/

ImageColorConvert::ImageColorConvert()
{
    m_inputPin  = createCvMatDataInputPin( "input", this );
    m_outputPin = createCvMatDataOutputPin( "output", this );

    // first one added is default
    PLV_ENUM_ADD( m_conversionType, CV_RGB2GRAY );
    PLV_ENUM_ADD( m_conversionType, CV_RGB2RGBA );

    setConversionType( m_conversionType );

    m_inputPin->addAllDepths();
    m_inputPin->addAllChannels();

    m_outputPin->addAllDepths();
    m_outputPin->addAllChannels();
}

ImageColorConvert::~ImageColorConvert()
{
}

int ImageColorConvert::getInChannels( int code )
{
    int inChannels;

    switch( code )
    {
        // 1
        case CV_GRAY2BGRA:
        case CV_GRAY2BGR:
        case CV_GRAY2BGR565:
        case CV_GRAY2BGR555:
        case CV_BayerBG2BGR:
        case CV_BayerGB2BGR:
        case CV_BayerRG2BGR:
        case CV_BayerGR2BGR:
            inChannels = 1;
            break;

        // 2
        case CV_BGR5652BGRA:
        case CV_BGR5552BGRA:
        case CV_BGR5652RGBA:
        case CV_BGR5552RGBA:
        case CV_BGR5652BGR:
        case CV_BGR5552BGR:
        case CV_BGR5652RGB:
        case CV_BGR5552RGB:
        case CV_BGR5652GRAY:
        case CV_BGR5552GRAY:
            inChannels = 2;
            break;

        // 3
        case CV_BGR2BGRA:
        case CV_RGB2BGRA:
        case CV_BGR2YCrCb:
        case CV_RGB2YCrCb:
        case CV_BGR2XYZ:
        case CV_RGB2XYZ:
        case CV_BGR2HSV:
        case CV_RGB2HSV:
        case CV_BGR2Lab:
        case CV_RGB2Lab:
        case CV_BGR2Luv:
        case CV_RGB2Luv:
        case CV_BGR2HLS:
        case CV_RGB2HLS:
        case CV_RGB2BGR:
        case CV_BGR2BGR565:
        case CV_BGR2BGR555:
        case CV_RGB2BGR565:
        case CV_RGB2BGR555:
        case CV_BGR2GRAY:
        case CV_RGB2GRAY:
        case CV_YCrCb2BGR:
        case CV_YCrCb2RGB:
        case CV_XYZ2BGR:
        case CV_XYZ2RGB:
        case CV_HSV2BGR:
        case CV_HSV2RGB:
        case CV_Lab2BGR:
        case CV_Lab2RGB:
        case CV_Luv2BGR:
        case CV_Luv2RGB:
        case CV_HLS2BGR:
        case CV_HLS2RGB:
            inChannels = 3;
            break;

        // 4
        case CV_BGRA2RGBA:
        case CV_BGRA2BGR:
        case CV_RGBA2BGR:
        case CV_BGRA2BGR565:
        case CV_BGRA2BGR555:
        case CV_RGBA2BGR565:
        case CV_RGBA2BGR555:
        case CV_BGRA2GRAY:
        case CV_RGBA2GRAY:
            inChannels = 4;
            break;
    default:
        // error
        inChannels = -1;
    }

    return inChannels;
}

int ImageColorConvert::getOutChannels( int code )
{
    int outChannels;

    switch( code )
    {
    case CV_BGR2BGRA:
    case CV_RGB2BGRA:
    case CV_BGRA2RGBA:
    case CV_BGR5652BGRA:
    case CV_BGR5552BGRA:
    case CV_BGR5652RGBA:
    case CV_BGR5552RGBA:
    case CV_GRAY2BGRA:
        outChannels = 4;
        break;

    case CV_BGR2YCrCb:
    case CV_RGB2YCrCb:
    case CV_BGR2XYZ:
    case CV_RGB2XYZ:
    case CV_BGR2HSV:
    case CV_RGB2HSV:
    case CV_BGR2Lab:
    case CV_RGB2Lab:
    case CV_BGR2Luv:
    case CV_RGB2Luv:
    case CV_BGR2HLS:
    case CV_RGB2HLS:
        outChannels = 3;
        break;

    case CV_BayerBG2BGR:
    case CV_BayerGB2BGR:
    case CV_BayerRG2BGR:
    case CV_BayerGR2BGR:

    case CV_BGRA2BGR:
    case CV_RGBA2BGR:
    case CV_RGB2BGR:
    case CV_BGR5652BGR:
    case CV_BGR5552BGR:
    case CV_BGR5652RGB:
    case CV_BGR5552RGB:
    case CV_GRAY2BGR:

    case CV_YCrCb2BGR:
    case CV_YCrCb2RGB:
    case CV_XYZ2BGR:
    case CV_XYZ2RGB:
    case CV_HSV2BGR:
    case CV_HSV2RGB:
    case CV_Lab2BGR:
    case CV_Lab2RGB:
    case CV_Luv2BGR:
    case CV_Luv2RGB:
    case CV_HLS2BGR:
    case CV_HLS2RGB:
        outChannels = 3;
        break;

    case CV_BGR2BGR565:
    case CV_BGR2BGR555:
    case CV_RGB2BGR565:
    case CV_RGB2BGR555:
    case CV_BGRA2BGR565:
    case CV_BGRA2BGR555:
    case CV_RGBA2BGR565:
    case CV_RGBA2BGR555:
    case CV_GRAY2BGR565:
    case CV_GRAY2BGR555:
        outChannels = 2;
        break;

    case CV_BGR2GRAY:
    case CV_BGRA2GRAY:
    case CV_RGB2GRAY:
    case CV_RGBA2GRAY:
    case CV_BGR5652GRAY:
    case CV_BGR5552GRAY:
        outChannels = 1;
        break;
    default:
        outChannels = -1;
    }

    return outChannels;
}

void ImageColorConvert::setConversionType(plv::Enum e)
{
    // update pin settings
    m_inChannels  = getInChannels( e.getSelectedValue() );
    m_outChannels = getOutChannels( e.getSelectedValue() );

    m_inputPin->clearChannels();
    m_outputPin->clearChannels();

    m_inputPin->addSupportedChannels( m_inChannels );
    m_outputPin->addSupportedChannels( m_outChannels );

    // update selection
    m_conversionType = e;

    // update GUI
    emit( conversionTypeChanged(e) );
}

bool ImageColorConvert::process()
{
    assert(m_inputPin != 0);
    assert(m_outputPin != 0);

    CvMatData in = m_inputPin->get();

    CvMatDataProperties props = in.properties();
    props.setNumChannels( m_outChannels );
    CvMatData out = CvMatData::create( props );

    // open for reading
    const cv::Mat& src = in;

    // open image for writing
    cv::Mat& dst = out;

    // cvCvtColor function, see OpenCV documentation for details
    cv::cvtColor(src, dst, m_conversionType.getSelectedValue(), m_outChannels );

    // publish the new image
    m_outputPin->put( out );

    return true;
}
