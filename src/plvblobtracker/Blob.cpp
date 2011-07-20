#include "Blob.h"
#include <plvcore/Util.h>
#include <QDebug>

using namespace plvblobtracker;

Blob::Blob() : d(new BlobData())
{
    d->valid = false;
}

Blob::Blob(unsigned int frameNr, const std::vector<cv::Point>& contour) :
    d(new BlobData())
{
    d->frameNr = frameNr;
    d->contour = contour;

    // calculate size
    cv::Mat mat(contour);
    d->size = abs(cv::contourArea(mat));

    //calculate boundingbox
    d->boundingRect = cv::boundingRect( mat );

    // calculate center of gravity
    cv::Moments m = cv::moments( mat );
    int comx = m.m10 / m.m00;
    int comy = m.m01 / m.m00;
    d->cog = cv::Point(comx,comy);
    d->valid = true;
}

Blob::~Blob()
{
}

//int Blob::inArea( const Blob& blob, int margin ) const
//{
//    return 0;
//}

int Blob::matchingArea( const Blob& blob ) const
{
    cv::Rect area;
    if( !overlappingAreaRect( blob, area ) )
        return 0;

    return area.area();
#if 0
    //minimum area rectangle containing a and b
    cv::Rect r = d->boundingRect | blob.getBoundingRect();

    cv::Mat m(r.size(), CV_8UC1);
    m = cv::Scalar(0);

    for( unsigned int i=0; i<d->contour.size(); ++i)
    {
        const cv::Point& p = d->contour[i];
        m.at<uchar>(p.x,p.y) = 1;
    }

    int count = 0;
    const std::vector<cv::Point>& contour = blob.getContour();
    for( unsigned int i=0; i<contour.size(); ++i)
    {
        const cv::Point& p = contour[i];
        if( m.at<uchar>(p.x,p.y) == 1 ) ++count;
    }
    return count;
#endif
}

bool Blob::overlappingAreaRect( const Blob& other, cv::Rect& area ) const
{
    const cv::Rect& rect = other.getBoundingRect();
    return overlappingArea( d->boundingRect, rect, area ) > 0;
}

void Blob::drawString( cv::Mat& target, const QString& str, cv::Scalar color ) const
{
    cv::putText(target, str.toAscii().constData(), d->cog, CV_FONT_HERSHEY_SIMPLEX, 0.5, color, 1, CV_AA, false );
}

void Blob::drawContour( cv::Mat& target, cv::Scalar color, bool fill ) const
{
    std::vector< std::vector<cv::Point> > contours;
    contours.push_back(d->contour);
// in OpenCV < 2.2 cv::drawContours causes a crash when using default parameters
// in release mode
#if (CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 2)
        cv::drawContours(target, contours, 0, color, fill ? CV_FILLED : 0, 8);
#else
        cv::drawContours(target, contours, 0, color, fill ? CV_FILLED : 0, 8,
                         std::vector<cv::Vec4i>(), INT_MAX - 1, cv::Point() );
#endif
}

void Blob::drawBoundingRect( cv::Mat& target, cv::Scalar color ) const
{
    cv::rectangle( target, d->boundingRect, color );
}

void Blob::drawCenterOfGravity( cv::Mat& target, cv::Scalar color ) const
{
    cv::line( target, d->cog, d->cog, color );
}

