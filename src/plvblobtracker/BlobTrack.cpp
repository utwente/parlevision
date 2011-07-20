#include "BlobTrack.h"
#include <stdlib.h>
#include <limits>

using namespace plvblobtracker;

BlobTrack::BlobTrack(unsigned int id,
                     Blob& blob,
                     int birthThreshold,
                     int birthWindow,
                     int dieThreshold,
                     int historySize,
                     int trackSize)
{
    d = new BlobTrackData(id, blob, birthThreshold, birthWindow, dieThreshold, historySize, trackSize);

    d->id = id;
    d->historySize = historySize;
    d->trackSize = trackSize;
    d->age = 0;
    d->color = cvScalar( qrand()%255, qrand()%255, qrand()%255);
    d->history.append(blob);
}

BlobTrack::~BlobTrack()
{
    // nothing to do, QSharedDataPointer does ref counting
}

const Blob& BlobTrack::getLastMeasurement() const
{
    return d->history.last();
}

/** called when this track did not match any blob */
void BlobTrack::notMatched( unsigned int frameNumber )
{
    ++d->age;
    if( frameNumber - d->history.last().getFrameNr() > d->dieThreshold )
    {
        d->state = BlobTrackDead;
    }
}

/** adds a measurement to this track */
void BlobTrack::addMeasurement( const Blob& blob )
{
    ++d->age;
    d->history.append(blob);
    if( d->history.size() > d->historySize )
    {
        d->history.removeFirst();
    }

    // TODO use ringbuffer
    d->track.push_back(blob.getCenterOfGravity());
    if( d->track.size() > d->trackSize )
    {
        d->track.remove(0);
    }

    if( d->state == BlobTrackBirth )
    {
        int from = d->history.size() - d->birthWindow;
        from = from > 0 ? from : 0;

        int count = 0;
        for( int i = from; i < d->history.size(); ++i )
        {

        }
        if( count >= d->birthThreshold )
            d->state = BlobTrackNormal;
    }
}

int BlobTrack::matches( const Blob& blob ) const
{
    if( d->state == BlobTrackDead )
        return 0;
    const Blob& last = getLastMeasurement();
    return blob.matchingArea(last);
}

/** draws the blob, its track and prediction */
void BlobTrack::draw( cv::Mat& target ) const
{
    assert( d->history.size() > 0 );
    assert( target.depth() == CV_8U );

    const double maxVal    = 255.0;
    const cv::Scalar red   = CV_RGB(maxVal,0,0);
    const cv::Scalar blue  = CV_RGB(0,0,maxVal);
    const cv::Scalar green = CV_RGB(0,maxVal,0);

    // draw tail
    const int tailLength = d->historySize;
    const double step = maxVal / tailLength;
    int count = 0;
    for( int i=d->history.size() <= tailLength ? 0 : d->history.size()-tailLength; i < d->history.size(); ++i)
    {
        const Blob& b = d->history[i];
        int shade = step*++count;
        b.drawContour(target, CV_RGB(0,shade,0), true);
    }

    // draw last measurement
    const Blob& b = getLastMeasurement();
    b.drawContour(target, green, true);
    b.drawCenterOfGravity(target, blue);
    b.drawBoundingRect(target,red);
    QString info = QString("ID: %1").arg(d->id);
    b.drawString(target, info);

    // draw track
    if( d->track.size() > 1 )
    {
        for(int i=0; i< d->track.size()-1; ++i)
        {
            cv::line(target, d->track[i], d->track[i+1], d->color, 1, CV_AA);
        }
    }
}
