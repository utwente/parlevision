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
#include "matrix.h"
#include "munkres.h"

#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <opencv/cv.h>
#include <plvcore/Util.h>
#include <limits>

using namespace plv;
using namespace plvblobtracker;

BlobTracker::BlobTracker() :  m_idCounter(0)
{
    m_inputImage = createCvMatDataInputPin( "input image", this );
    m_inputImage->addSupportedDepth(CV_8U);
    m_inputImage->addSupportedChannels(3);

    m_inputBlobs = createInputPin< QList<plvblobtracker::Blob> >( "input blobs" , this );
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

    QList<plvblobtracker::Blob> newBlobs = m_inputBlobs->get();
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

    // make matrix of blobs and tracks
    // a1 b1
    // a2 b2
    // a3 b3

    QVector< QList<int> > candidatesForBlobs( blobs.size() );
    QVector< QList<int> > candidatesForTracks( tracks.size() );

    double maxScore = std::numeric_limits<double>::min();
    for( int i=0; i < tracks.size(); ++i )
    {
        const BlobTrack& track = tracks.at(i);
        QList<int>& candidatesForTrack = candidatesForTracks[i];

        for( int j=0; j < blobs.size(); ++j )
        {
            const Blob& blob = blobs.at(j);
            QList<int>& candidatesForBlob = candidatesForBlobs[j];

            double score = (double)track.matches(blob);
            if( score > 0 )
            {
                candidatesForBlob.append(i);
                candidatesForTrack.append(j);
            }
            if( score > maxScore ) maxScore = score;
        }
    }
    maxScore = maxScore - 1.0;

    // grouping

    // two tracks match a single blob and do not match any other blobs

    // two blobs match a single track

    // degrouping
    int blobsSize = blobs.size();
#if 0
    //QVector<int> dummies;
    for( int i=0; i < blobsSize; ++i )
    {
        const QList<int>& candidates = candidatesForBlobs.at(i);
        if( candidates.size() > 1 )
        {
            bool tracksHaveOneBlob = true;
            foreach( int trackId, candidates )
            {
                bool trackHasOneBlob = candidatesForTracks.at(trackId).size() == 1;
                tracksHaveOneBlob = tracksHaveOneBlob && trackHasOneBlob;
            }

            if( tracksHaveOneBlob )
            {
                // at least 2 blobs point to the same track
                QString out = QString("Blob %1 matches tracks ").arg(i);
                for( int j=0; j<candidates.size(); ++j )
                    out.append( QString("%1 ").arg( candidates.at(j) ) );

                qDebug() << out;

                // divide blob in two parts
                const Blob& b = blobs.at(i);

                //blobs.append(b);
                //dummies.append(blobs.size());
                //candidatesForBlobs.append(candidates);
            }
        }
    }
#endif

    // matrix (track,blob);
    int matrixSize = tracks.size() > blobs.size() ? tracks.size() : blobs.size();
    const double infinity =  std::numeric_limits<double>::infinity();
    Matrix<double> m(matrixSize,matrixSize);
    for( int i = 0 ; i < m.rows() ; i++ )
        for( int j = 0 ; j < m.columns(); j++ )
            m(i,j) =  infinity;

    for( int i=0; i < tracks.size(); ++i )
    {
        const BlobTrack& track = tracks.at(i);
        for( int j=0; j < blobs.size(); ++j )
        {
            const Blob& blob = blobs.at(j);
            double score = (double)track.matches(blob);
            if( score > 0 )
            {
                // convert matching score to cost
                m(i,j) = maxScore - score;
            }
        }
    }

    // run munkres matching algorithm
    Munkres munk;
    munk.solve(m);

    QVector<bool> matchedTracks( tracks.size(), 0 );
    QVector<bool> matchedBlobs( blobs.size(), 0);
    for( int i=0; i < tracks.size(); ++i )
    {
        BlobTrack& track = tracks[i];
        int match = -1;
        for( int j=0; j < blobs.size(); ++j )
        {
            if( m(i,j) == 0 )
                match = j;
        }
        if( match != -1 )
        {
            const Blob& blob = blobs.at(match);
            track.addMeasurement(blob);
            matchedBlobs[match] = true;
            matchedTracks[i]    = true;
        }
        else
        {
            track.notMatched( this->getProcessingSerial() );
        }
    }

//    QSet<int> usedBlobs;
//    for( int i=0; i < cadidatesForTracks.size(); ++i )
//    {
//        if( candidatesForTracks[i].size() > 0 )
//        {
//            BlobTrack& track = tracks[i];
//        }
//    }

//    QVector< QVector<int> > legalCombinations( candidatesForTracks.size() );
//    for( int i=0; i < candidatesForTracks.size(); ++i )
//    {
//        const QVector<int>& candidatesForTrack = candidatesForTracks[i];
//        QVector<int> combinations;
//        for( int j=0; j < candidatesForTrack.size(); ++j )
//        {
//            if( candidatesForTrack[j] > 0 )
//            {
//                combinations.push_back(j);
//            }
//        }
//        legalCombinations[i] = combinations;
//    }


    // unmatched newblobs, add them to the collection
    // if they are large enough
    for( int j=0; j < blobsSize; ++j )
    {
        if( !matchedBlobs[j] )
        {
            Blob blob = blobs.at(j);
            BlobTrack track( getNewId(), blob );
            tracks.append(track);
        }
    }

    // remove old items
//    QMutableListIterator<BlobTrack> itr( m_blobTracks );
//    while( itr.hasNext() )
//    {
//        BlobTrack& t = itr.next();
//        if( t.getState() == BlobTrackDead )
//        {
//            itr.remove();
//        }
//    }

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

