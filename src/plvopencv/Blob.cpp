#include "Blob.h"
#include <plvcore/Util.h>

using namespace plvopencv;

Blob::Blob(const std::vector<cv::Point>& contour,
           unsigned int frameNum,
           float conversionFactorX,
           float conversionFactorY ) :
    m_id( 0 ),
    m_frameNum( frameNum ),
    m_contour( contour )
{
    m_cfx = conversionFactorX;
    m_cfy = conversionFactorY;

    // calculate size
    cv::Mat mat(contour);
    m_size = abs(cv::contourArea(mat));

    //calculate boundingbox
    m_boundingRect = cv::boundingRect( mat );

    m_bottomLeft.x = m_boundingRect.x;
    m_bottomLeft.y = m_boundingRect.y;
    m_topRight.x = m_boundingRect.x + m_boundingRect.width;
    m_topRight.y = m_boundingRect.y + m_boundingRect.height;

    m_minX = m_bottomLeft.x;
    m_maxX = m_topRight.x;
    m_minY = m_bottomLeft.y;
    m_maxY = m_topRight.y;

    cv::Moments m = cv::moments( mat );
    int comx = m.m10 / m.m00;
    int comy = m.m01 / m.m00;
    m_cog = cv::Point(comx,comy);
}

Blob::~Blob()
{
}

void Blob::drawInformation( cv::Mat& target, cv::Scalar color ) const
{
    std::ostringstream oss;
    oss << "ID:" << m_id;
    cv::putText(target, oss.str() ,m_cog, CV_FONT_HERSHEY_SIMPLEX, 0.5, color, 1, CV_AA, false );
}

void Blob::drawContour( cv::Mat& target, cv::Scalar color, bool fill ) const
{
    std::vector< std::vector<cv::Point> > contours;
    contours.push_back(m_contour);
    cv::drawContours(target, contours, 0, color, fill ? CV_FILLED : 0, 8);
}

void Blob::drawBoundingRect( cv::Mat& target, cv::Scalar color ) const
{
    cv::rectangle( target, m_boundingRect, color );
}

void Blob::drawCenterOfGravity( cv::Mat& target, cv::Scalar color ) const
{
    cv::line( target, m_cog, m_cog, color );
}

/*
float maxDistance = sqrtf( m_width * m_width * m_height * m_height );

    CvPoint cogB = blob->getCog();
    float distX = m_cog.x - cogB.x;
    float distY = m_cog.y - cogB.y;
    float cogDistance = sqrtf( distX * distX + distY * distY );

    float distScore = 1.0 - ( cogDistance / maxDistance );

    return distScore;
*/

int Blob::inArea( const Blob* blob, int margin ) const
{
    return 0;
}

int Blob::matches( const Blob* blob ) const
{
    cv::Rect area;
    if( !overlappingAreaRect( blob, area ) )
        return 0;

    return area.area();

    /*cvSetZero( m_scratchpad );

    CvRect maxRect = cvMaxRect( &m_boundingRect, &blob->getBoundingRect() );

    IplImage* img1 = m_scratchpad;
    IplImage* img2 = blob->getScratchpad();

    cvSetImageROI( img1, maxRect );
    cvSetImageROI( img2, maxRect );

    IplImage* result = cvCreateImage( cvSize(maxRect.width, maxRect.height), img1->depth, img1->nChannels );
    cvAnd( img1, img2, result );
    double numPixels = cvSumPixels( result );*/

    //std::vector< cv::Point >& c1 = getContour();
    //std::vector< cv::Point >& c2 = blob->getContour();

    //assert( c1 != 0 );
    //assert( c2 != 0 );

    //double test = cvMatchContours( c1, c2, CV_CONTOURS_MATCH_I1 );
}

bool Blob::overlappingAreaRect( const Blob* other, cv::Rect& area ) const
{
    const cv::Rect& rect = other->getBoundingRect();
    return overlappingArea( m_boundingRect, rect, area );
}

