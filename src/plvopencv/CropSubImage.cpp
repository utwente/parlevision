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

#include "CropSubImage.h"
#include <plvcore/CvMatData.h>
#include <plvcore/Types.h>

#include <plvcore/CvMatDataPin.h>
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

CropSubImage::CropSubImage()
{
    m_imageInput = createCvMatDataInputPin( "image", this );
    m_regions    = createInputPin<RectangleData>( "regions", this );
    m_imagesOutput = createOutputPin< QList<CvMatData> >( "out images", this );

    m_imageInput->addAllChannels();
    m_imageInput->addAllDepths();
}

CropSubImage::~CropSubImage()
{
}

bool CropSubImage::process()
{
    CvMatData img = m_imageInput->get();
    RectangleData data = m_regions->get();

    // open input image for reading
    const cv::Mat& in = img;

    QList<CvMatData> subregions;
    QList<QRect> rects = data.getRects();
    foreach( const QRect& r, rects )
    {
        cv::Rect roi(r.x(), r.y(), r.width(), r.height());
        cv::Mat subregion = in(roi);
        subregions.append( CvMatData(subregion) );
    }
    m_imagesOutput->put(subregions);
    return true;
}

