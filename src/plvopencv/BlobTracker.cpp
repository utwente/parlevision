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
#include "BlobTracker.h"

#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <opencv/cv.h>
#include <plvcore/Util.h>

using namespace plv;
using namespace plvopencv;

BlobTracker::BlobTracker() :  m_idCounter(0)
{
    m_inputImage = createCvMatDataInputPin( "input image", this );
    m_inputImage->addSupportedDepth(CV_8U);
    m_inputImage->addSupportedChannels(3);

    m_inputBlobs = createInputPin< QList<plvopencv::Blob> >( "input blobs" , this );
    m_outputImage = createCvMatDataOutputPin( "output image", this);
}

BlobTracker::~BlobTracker()
{
}

bool BlobTracker::init()
{
    return true;
}

bool BlobTracker::deinit() throw()
{
    m_blobTracks.clear();
    return true;
}

bool BlobTracker::start()
{
    return true;
}

bool BlobTracker::stop()
{
    m_blobTracks.clear();
    return true;
}

bool BlobTracker::process()
{
    CvMatData in = m_inputImage->get();

    CvMatData out = CvMatData::create(in.properties());
    cv::Mat& dst = out;
    dst = cv::Scalar(0,0,0);

    QList<plvopencv::Blob> newBlobs = m_inputBlobs->get();
    matchBlobs(newBlobs, m_blobTracks);

    foreach( BlobTrack t, m_blobTracks )
    {
        t.draw(dst);
    }

    m_outputImage->put(out);
    return true;
}

// match the old blob to a new detected blob using
// maximum area converage
void BlobTracker::matchBlobs(QList<Blob>& blobs, QList<BlobTrack>& tracks)
{
    //const int sizeDiffThreshold = 1000;
    //const int minBlobThreshold = 0;

    //const int acceptedSizeDiff = (width * height * sizeDiffThreshold) / 1000.0f;
    //const int minMatch = (width * height * minBlobThreshold) / 10000.0f;

    // max accepted size difference between two blobs in pixels
    //const int maxSizeDiff = INT_MAX;

    // filter on minimum blob size
    //const int minArea = 1;

    //const unsigned int frameNr = this->getProcessingSerial();

    QMap<int, QVector<int> > candidatesForBlobs;
    for( int j=0; j < blobs.size(); ++j )
    {
        candidatesForBlobs[j] = QVector<int>(tracks.size(), 0);
    }

    QMap<int, QVector<int> > candidatesForTracks;
    for( int i=0; i < tracks.size(); ++i )
    {
        candidatesForTracks[i] = QVector<int>(blobs.size(), 0);
    }

    for( int i=0; i < tracks.size(); ++i )
    {
        const BlobTrack& track = tracks.at(i);
        QVector<int>& candidatesForTrack = candidatesForTracks[i];

        for( int j=0; j < blobs.size(); ++j )
        {
            const Blob& blob = blobs.at(j);
            QVector<int>& candidatesForBlob = candidatesForBlobs[j];

            int score = track.matches(blob);
            if( score > 0 )
            {
                candidatesForTrack[j] = score;
                candidatesForBlob[i]  = score;
            }
        }
    }

    QSet<int> usedBlobs;
    for( int i=0; i < cadidatesForTracks.size(); ++i )
    {
        if( candidatesForTracks[i].size() > 0 )
        {
            BlobTrack& track = tracks[i];
        }



    }

    QVector< QVector<int> > legalCombinations( candidatesForTracks.size() );
    for( int i=0; i < candidatesForTracks.size(); ++i )
    {
        const QVector<int>& candidatesForTrack = candidatesForTracks[i];
        QVector<int> combinations;
        for( int j=0; j < candidatesForTrack.size(); ++j )
        {
            if( candidatesForTrack[j] > 0 )
            {
                combinations.push_back(j);
            }
        }
        legalCombinations[i] = combinations;
    }



    QVector<int> matchedBlobs( blobs.size(), 0);
    for( int i=0; i<maxCombination.size(); ++i)
    {
        int idx = maxCombination[i];
        if( idx != -1 )
            matchedBlobs[idx] = 1;
    }

    // unmatched newblobs, add them to the collection
    // if they are large enough
    for( int j=0; j < blobs.size(); ++j )
    {
        if( !matchedBlobs[j] )
        {
            Blob blob = blobs.at(j);
            BlobTrack track( getNewId(), blob );
            tracks.append(track);
        }
    }

    // a blob can match multiple tracks
    // a blob can match one track
    // - right track
    // - wrong track

    // a track can match multiple blobs
    // - right blob
    // - wrong blob

    // a track can match no blob
    // - but there is a blob

    // a track can match a single blob
    // - right blob
    // - wrong blob

    // check for collisions
}

