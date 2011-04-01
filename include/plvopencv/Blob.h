#ifndef PLVOPENCV_BLOB_H
#define PLVOPENCV_BLOB_H

#include <opencv/cv.h>

namespace plvopencv
{
    class Blob
    {
    private:
        unsigned int m_id;
        unsigned int m_frameNum;
        std::vector< cv::Point > m_contour;
        cv::Point m_topRight;
        cv::Point m_bottomLeft;
        cv::Point m_cog;
        double m_size;
        cv::Mat m_blobImg;
        float m_cfx; // this is the conversion factor from pixels to millimeters.
        float m_cfy; // use this to calculate the real world x and y in millimters on the displaysurface.
        int m_minX;
        int m_maxX;
        int m_minY;
        int m_maxY;
        cv::Mat m_scratchpad;
        cv::Rect m_boundingRect;

    public:
        Blob(const std::vector< cv::Point >& contour,
             unsigned int frameNum,
             float conversionFactorX=0.0f,
             float conversionFactorY=0.0f );

        virtual ~Blob();

        void drawContour( cv::Mat& target, cv::Scalar color = CV_RGB(128, 128, 128), bool fill = true ) const;
        void drawBoundingRect( cv::Mat& target, cv::Scalar color = CV_RGB(255, 255, 255) ) const;
        void drawCenterOfGravity( cv::Mat& target, cv::Scalar color = CV_RGB(255, 255, 255) ) const;
        void drawInformation( cv::Mat& target, cv::Scalar color = CV_RGB(255, 255, 255) ) const;

        inline double getSize() { return m_size; }
        inline const cv::Point& getCenterOfGravity() const { return m_cog; }

        inline void setId( unsigned int nid ) { m_id = nid; };
        inline unsigned int getId() { return m_id; }
        inline unsigned int getFrameNum() { return m_frameNum; }
        inline void setFrameNr(unsigned int nr ) { m_frameNum = nr; }

        cv::Mat& createSubImage(cv::Mat *image, cv::Rect roi);

        bool overlappingAreaRect( const Blob* other, cv::Rect& area ) const;
        int matches( const Blob* blob ) const;
        int inArea( const Blob* blob, int margin ) const;

        inline const cv::Rect& getBoundingRect() const { return m_boundingRect; }
        //inline const cv::Mat& getScratchpad() const { return m_scratchpad; }
        inline std::vector< cv::Point > getContour() { return m_contour; }

        inline double getSize() const { return m_size; }

        //inline bool vertexXIn( int x ) const { return x >= m_minX && x <= m_maxX; }
        //inline bool vertexYIn( int y ) const { return y >= m_minY && y <= m_maxY; }
        //inline bool vertexInside( const cv::Point& p ) const { return vertexXIn( p.x ) && vertexYIn( p.y ); }

    };
}

#endif
