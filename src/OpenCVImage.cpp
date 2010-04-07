#include "OpenCVImage.h"
#include "RefPtr.h"

using namespace plv;

OpenCVImage::OpenCVImage( int id, IplImage* img ) :
    m_id( id ),
    m_img( img )
{
}

OpenCVImage::~OpenCVImage()
{
    cvReleaseImage( &m_img );
}

/** Compare two opencv images for type equality */
bool OpenCVImage::isCompatible( const Resource* other )
{
    RefPtr<const OpenCVImage> img = dynamic_cast<const OpenCVImage*>(other);
    if( img.isValid() && m_img != 0 )
    {
        IplImage* other = img->getImage();
        return( m_img->nChannels == other->nChannels &&
                m_img->depth     == other->depth &&
                m_img->width     == other->width &&
                m_img->height    == other->height );
    }
    return false;
}
