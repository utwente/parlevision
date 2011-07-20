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
#include "BlobDetector.h"
#include "Blob.h"

#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <opencv/cv.h>
#include <set>

using namespace plv;
using namespace plvblobtracker;

BlobDetector::BlobDetector() :
    m_iterations(0),
    m_minBlobSize(0)
{
    m_inputImage = createCvMatDataInputPin( "input image", this );

    m_inputImage->addSupportedChannels(1);
    m_inputImage->addSupportedDepth(CV_8U);

    m_outputImage = createCvMatDataOutputPin( "out", this);
    m_outputImage->addAllChannels();
    m_outputImage->addAllDepths();

    m_outputBlobs = createOutputPin< QList<plvblobtracker::Blob> >("blobs", this);

    PLV_ENUM_ADD(m_mode, CV_RETR_EXTERNAL);
    PLV_ENUM_ADD(m_mode, CV_RETR_LIST);
    PLV_ENUM_ADD(m_mode, CV_RETR_CCOMP);
    PLV_ENUM_ADD(m_mode, CV_RETR_TREE);

    PLV_ENUM_ADD(m_method, CV_CHAIN_CODE);
    PLV_ENUM_ADD(m_method, CV_CHAIN_APPROX_NONE);
    PLV_ENUM_ADD(m_method, CV_CHAIN_APPROX_SIMPLE);
    PLV_ENUM_ADD(m_method, CV_CHAIN_APPROX_TC89_L1);
    PLV_ENUM_ADD(m_method, CV_CHAIN_APPROX_TC89_KCOS);
    PLV_ENUM_ADD(m_method, CV_LINK_RUNS);
}

BlobDetector::~BlobDetector()
{
}

bool BlobDetector::process()
{
    CvMatData in = m_inputImage->get();
    cv::Mat& src = in;

    QList<Blob> newBlobs;

    int mode = CV_RETR_LIST;
    int method = CV_CHAIN_APPROX_NONE;
    std::vector< std::vector< cv::Point > > contours;
    std::vector< cv::Vec4i > hierarchy;

    cv::findContours( src, contours, hierarchy, mode, method, cv::Point() );

    CvMatData out = CvMatData::create(in.width(), in.height(), CV_8UC3 );
    cv::Mat& dst = out;

    cv::Scalar red   = CV_RGB( 255, 0, 0 );
    cv::Scalar green = CV_RGB( 0, 255, 0 );
    cv::Scalar blue  = CV_RGB( 0, 0, 255 );
    cv::Scalar white = CV_RGB( 255, 255, 255 );

    for( unsigned int i=0; i<contours.size(); ++i)
    {
        const std::vector< cv::Point >& c = contours[i];
        Blob b(m_iterations,c);
        if( b.getSize() >= m_minBlobSize )
        {
            newBlobs.append(b);
            b.drawContour(dst, green, true);
            b.drawBoundingRect(dst, red);
            QString info = QString("pos:%1,%2 size:%3")
                    .arg(b.getCenterOfGravity().x)
                    .arg(b.getCenterOfGravity().y)
                    .arg(b.getSize());
            b.drawString(dst, info, white);
        }
        else
        {
            b.drawContour(dst, red, true);
        }
    }

    // filter on minimum blob size
    QMutableListIterator<Blob> itr(newBlobs);
    while( itr.hasNext() )
    {
        const Blob& newBlob = itr.next();
        if(newBlob.getSize() < m_minBlobSize )
            itr.remove();
    }
    m_outputImage->put( out );
    m_outputBlobs->put( newBlobs );
    ++m_iterations;
    return true;
}

plv::Enum BlobDetector::getMethod() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_method;
}

plv::Enum BlobDetector::getMode() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_mode;
}

int BlobDetector::getMinBlobSize() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_minBlobSize;
}

void BlobDetector::setMode( plv::Enum mode )
{
    QMutexLocker lock( m_propertyMutex );
    m_mode = mode;
    emit modeChanged(mode);
}

void BlobDetector::setMethod( plv::Enum method )
{
    QMutexLocker lock( m_propertyMutex );
    m_method = method;
    emit methodChanged(method);
}

void BlobDetector::setMinBlobSize(int size)
{
    QMutexLocker lock( m_propertyMutex );
    if( size >= 0 )
        m_minBlobSize = size;
    emit minBlobSizeChanged(m_minBlobSize);
}


