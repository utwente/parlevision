#ifndef PLVBLOBTRACK_BLOBTRACK_H
#define PLVBLOBTRACK_BLOBTRACK_H

#include <opencv/cv.h>
#include <QVector>
#include "Blob.h"

namespace plvblobtracker
{
    enum PlvOpenCVBlobTrackState {
        BlobTrackBirth,  /** not enough measurements yet */
        BlobTrackNormal, /** normal state */
        BlobTrackLost,   /** not showing often enough any more */
        BlobTrackDead    /** removed from tracking */
    };

    class BlobTrackData : public QSharedData
    {
    public:


        inline BlobTrackData( unsigned int _id,
                              Blob& _blob,
                              int _birthThreshold,
                              int _birthWindow,
                              int _dieThreshold,
                              int _historySize,
                              int _trackSize) :
            id(_id),
            birthThreshold( _birthThreshold ),
            birthWindow( _birthWindow ),
            dieThreshold( _dieThreshold ),
            historySize(_historySize),
            trackSize(_trackSize),
            age(0),
            state(BlobTrackBirth)
        {
            assert(_blob.isValid());
            history.append(_blob);
        }

        unsigned int id;

        /** track birth if i matches in j frames */
        unsigned int birthThreshold;
        unsigned int birthWindow;

        /** track dies if not seen for dieThreshold frames */
        unsigned int dieThreshold;

        unsigned int historySize;
        unsigned int trackSize;
        unsigned int age;

        PlvOpenCVBlobTrackState state;

        QList<Blob>        history; /** the history of this blob track */
        QVector<cv::Point> track;   /** the actual route this blob has followed */
        cv::Vec2d          speed;   /** the speed of this blob track */
        cv::Vec2d          conversionFactor; /** conversion factor from pixels to millimeters. */
        cv::Scalar         color;   /** color to use when drawing this track onto an image */
    };

    class BlobTrack
    {
    public:
        BlobTrack(unsigned int id,
                  Blob& blob,
                  int birthThreshold=3,
                  int birthWindow=10,
                  int dieThreshold=10,
                  int historySize=10,
                  int trackSize=INT_MAX);

        virtual ~BlobTrack();

        /** copy constructor */
        inline BlobTrack( const BlobTrack& other ) { d = other.d; }

        /** assignment operator */
        inline BlobTrack& operator=( const BlobTrack& other )
        {
            d = other.d;
            return *this;
        }

        inline unsigned int getId() const { return d->id; }

        /** adds a measurement to this track */
        void addMeasurement( const Blob& blob );

        /** call when track is not matched */
        void notMatched(  unsigned int frameNumber );

        /** returns last blob measurement */
        const Blob& getLastMeasurement() const;

        /** returns the size of the history */
        inline int getHistorySize() const { return d->history.size(); }

        /** draws the blob, its track and prediction. Target must have depth CV_8U. */
        void draw( cv::Mat& target ) const;

        int matches( const Blob& blob ) const;

        inline cv::Vec2d getSpeed() const { return d->speed; }

        inline unsigned int getLastFrameNr() const { return d->history[0].getFrameNr(); }

        inline int getAge() const { return d->age; }

        inline PlvOpenCVBlobTrackState getState() const { return d->state; }

    private:
        QSharedDataPointer<BlobTrackData> d;
    };
}

#endif
