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
using namespace plvopencv;

BlobDetector::BlobDetector() : m_idCounter(0), m_nrFramesToKeep(10)
{
    m_inputImage = createCvMatDataInputPin( "input image", this );

    m_inputImage->addSupportedChannels(1);
    m_inputImage->addSupportedDepth(CV_8U);

    m_outputImage = createCvMatDataOutputPin( "out", this);
    m_outputImage->addAllChannels();
    m_outputImage->addAllDepths();

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
    unsigned int serial = this->getProcessingSerial();
    CvMatData in = m_inputImage->get();
    cv::Mat& src = in;

    QList<Blob*> newBlobs;
    //int minSize = 5;
    //const int minBlobSize = (minSize * in.width() * in.height()) / 1000.0f;

    int mode = CV_RETR_LIST;
    int method = CV_CHAIN_APPROX_NONE;
    std::vector< std::vector< cv::Point > > contours;
    std::vector< cv::Vec4i > hierarchy;

    cv::findContours( src, contours, hierarchy, mode, method, cv::Point() );

    CvMatData out = CvMatData::create(in.width(), in.height(), CV_8UC3 );
    cv::Mat& dst = out;

    cv::Scalar red = CV_RGB( 255, 0, 0 );
    cv::Scalar white = CV_RGB( 255, 255, 255 );
    for( unsigned int i=0; i<contours.size(); ++i)
    {
        const std::vector< cv::Point >& c = contours[i];
        Blob* b = new Blob( c, serial, 1.0f, 1.0f );
        newBlobs.append(b);
        b->drawContour(dst, white, true);
    }

    // match blobs. The vector blobs will hold all matched and unmatched blobs
    // after this call. Unmatched blobs will have a new id.
    m_blobs = matchBlobs( newBlobs, m_blobs );

    foreach( const Blob* blob, m_blobs )
    {
        blob->drawBoundingRect(dst, red);
        blob->drawInformation(dst);
    }

    m_outputImage->put(out);
    return true;
}

// match the old blob to a new detected blob using
// maximum area converage
QList<Blob*> BlobDetector::matchBlobs( QList<Blob*>& newBlobs, const QList<Blob*>& oldBlobs )
{
    //const int sizeDiffThreshold = 1000;
    //const int minBlobThreshold = 0;

    //const int acceptedSizeDiff = (width * height * sizeDiffThreshold) / 1000.0f;
    //const int minMatch = (width * height * minBlobThreshold) / 10000.0f;

    // max accepted size difference between two blobs in pixels
    const int maxSizeDiff = INT_MAX;

    // minimum matching pixels
    const int minArea = 1;
    const int minBlobSize = 1;
    for( int j=0; j < newBlobs.size(); ++j )
    {
        Blob* newBlob = newBlobs.at(j);
        if(newBlob->getSize() <= minBlobSize )
            newBlobs.removeAt(j);
    }

    const unsigned int nrFramesToKeep = 10;
    const unsigned int frameNr = this->getProcessingSerial();

    // create a vector for bookkeeping
    // here we insert all matches of old blobs
    QList<Blob*> matchedBlobs;
    QSet<Blob*> matchedOld;
    QSet<Blob*> matchedNew;

    QMap<Blob*, QList<Blob*> > matchedOldBlobs;
    QMap<Blob*, QList<Blob*> > matchedNewBlobs;
    for( int i=0; i < oldBlobs.size(); ++i )
    {
        Blob* oldBlob = oldBlobs.at(i);

        for( int j=0; j < newBlobs.size(); ++j )
        {
            Blob* newBlob = newBlobs.at(j);
            int area = oldBlob->matches(newBlob);
            int sizeDiff = abs(oldBlob->getSize() - newBlob->getSize());
            if( area >= minArea && sizeDiff <= maxSizeDiff )
            {
                matchedOldBlobs[oldBlob].append(newBlob);
                matchedNewBlobs[newBlob].append(oldBlob);
            }
        }
    }

    // add all blobs which have only one match to the matchedBlobs
    // collection
    QMapIterator< Blob*, QList<Blob*> > itr(matchedOldBlobs);
    while( itr.hasNext() )
    {
        itr.next();
        Blob* oldBlob = itr.key();
        QList<Blob*> list = itr.value();
        if( list.size() == 1 )
        {
            Blob* newBlob = list.at(0);
            QList<Blob*> list2 = matchedNewBlobs[newBlob];
            if( list2.size() == 1 )
            {
                assert( list2.at(0) == oldBlob );
                newBlob->setId( oldBlob->getId() );
                newBlob->setFrameNr( frameNr );
                matchedBlobs.append(newBlob);
                matchedOld.insert(oldBlob);
                matchedNew.insert(newBlob);
            }
        }
    }

    // keep old blobs which have not been matched but might be again
    // in the future
    for(int i=0; i < oldBlobs.size(); ++i)
    {
        Blob* unmatchedBlob = oldBlobs[i];
        if( !matchedOld.contains(unmatchedBlob) )
        {
            if(frameNr - unmatchedBlob->getFrameNum() <= nrFramesToKeep)
            {
                matchedBlobs.append(unmatchedBlob);
            }
        }
    }

    // unmatched newblobs, add them to the collection
    // if they are large enough
    for( int j=0; j < newBlobs.size(); ++j )
    {
        Blob* newBlob = newBlobs.at(j);
        if( !matchedNew.contains(newBlob) )
        {
            if( newBlob->getSize() > minBlobSize )
            {
                newBlob->setId( getNewId() );
                matchedBlobs.append(newBlob);
            }
        }
    }
    return matchedBlobs;
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


