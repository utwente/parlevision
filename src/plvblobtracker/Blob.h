#ifndef PLVBLOBTRACK_BLOB_H
#define PLVBLOBTRACK_BLOB_H

#include <opencv/cv.h>
#include <QVector>
#include <QMetaType>
#include <QSharedData>

namespace plvblobtracker
{
    /** Blob is implemented as shared data class. See QSharedDataPointer documentation */
    class BlobData : public QSharedData
    {
    public:
        unsigned int frameNr;     /** the frame number in which this blob was detected */
        std::vector<cv::Point> contour; /** the blob contour, a collection of pixels which make out the blob */
        cv::Point topRight;       /** top right coordinate */
        cv::Point bottomLeft;     /** bottom left coordinate */
        cv::Point cog;            /** center of gravity */
        double size;              /** size of blob in pixels */
        cv::Mat blobImg;          /** image of the blob */
        cv::Rect boundingRect;    /** the axis aligned bounding rectangle */
        bool valid;               /** false when Blob is called with default constructor */
    };

    class Blob
    {
    public:
        Blob();
        Blob(unsigned int frameNr, const std::vector< cv::Point >& contour);
        virtual ~Blob();

        inline bool isValid() const { return d->valid; }
        inline Blob& operator=(const Blob& other) { d=other.d; return *this; }

        /** returns the size in pixels */
        inline double getSize() { return d->size; }

        inline unsigned int getFrameNr() const { return d->frameNr; }

        /** returns the center of gravity */
        inline const cv::Point& getCenterOfGravity() const { return d->cog; }

        bool overlappingAreaRect( const Blob& other, cv::Rect& area ) const;

        /** returns the number of matching pixels with other */
        int matchingArea( const Blob& other ) const;

        //int inArea( const Blob& blob, int margin ) const;

        inline const cv::Rect& getBoundingRect() const { return d->boundingRect; }
        inline const std::vector< cv::Point > getContour() const { return d->contour; }
        inline double getSize() const { return d->size; }

        /** drawing functions */
        void drawContour( cv::Mat& target, cv::Scalar color = CV_RGB(128, 128, 128), bool fill = true ) const;
        void drawBoundingRect( cv::Mat& target, cv::Scalar color = CV_RGB(255, 255, 255) ) const;
        void drawCenterOfGravity( cv::Mat& target, cv::Scalar color = CV_RGB(255, 255, 255) ) const;
        void drawString(cv::Mat& target, const QString& str, cv::Scalar color = CV_RGB(255, 255, 255) ) const;

    private:
         QSharedDataPointer<BlobData> d;
    };
}
Q_DECLARE_METATYPE( QList<plvblobtracker::Blob> )

#endif
