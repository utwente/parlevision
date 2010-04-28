#include "OpenCVImage.h"
#include "RefPtr.h"

using namespace plv;

OpenCVImage::OpenCVImage( int id, IplImage* img ) :
        Data(),
        m_id( id ),
        m_img( img )
{
}

OpenCVImage::~OpenCVImage()
{
    cvReleaseImage( &m_img );
}

OpenCVImage::OpenCVImage( const OpenCVImage& other ) :
        Data(),
        m_id( other.m_id )
{
    m_img = cvCreateImage( cvSize(other.m_img->width, other.m_img->height),
                           other.m_img->depth, other.m_img->nChannels );
    cvCopyImage( other.m_img, m_img );
}

OpenCVImage* OpenCVImage::deepcopy()
{
    IplImage* img = cvCreateImage( cvSize(m_img->width, m_img->height),
                           m_img->depth, m_img->nChannels );
    cvCopyImage( m_img, img );
    return new OpenCVImage(this->m_id, img);
}

/** Compare two opencv images for type equality */
//bool OpenCVImage::isCompatibleFormat( const ComplexData* data )
//{
//    RefPtr<const OpenCVImage> img = dynamic_cast<const OpenCVImage*>(data);
//    if( img.isValid() && m_img != 0 )
//    {
//        IplImage* other = img->getImage();
//        return( m_img->nChannels == other->nChannels &&
//                m_img->depth     == other->depth &&
//                m_img->width     == other->width &&
//                m_img->height    == other->height );
//    }
//    return false;
//}
