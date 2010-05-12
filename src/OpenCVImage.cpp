#include "OpenCVImage.h"
#include "RefPtr.h"

#include <opencv/cv.h>

using namespace plv;

OpenCVImageFactory* OpenCVImageFactory::m_instance = 0;

OpenCVImageFactory::OpenCVImageFactory()
{
}

OpenCVImageFactory::~OpenCVImageFactory()
{
    purge();
}

void OpenCVImageFactory::purge()
{
    for( std::list<OpenCVImage*>::iterator itr = m_objectPool.begin();
            itr != m_objectPool.end(); ++itr )
    {
        OpenCVImage* img = *itr;
        if( img->getRefCount() == 1 )
        {
            // this will auto delete the image
            img->dec();
            m_objectPool.erase( itr );
        }
    }
}

OpenCVImage* OpenCVImageFactory::getFromBuffer( IplImage* buffer, bool own )
{
    assert( buffer != 0 );

    OpenCVImage* img = 0;

    if( !own )
    {
        img = get( buffer->width, buffer->height,
                                buffer->depth, buffer->nChannels);
        cvCopyImage( buffer, img );
    }
    else
    {
        img = new OpenCVImage( buffer );

        // up the ref count by one and add to pool
        img->inc();
        m_objectPool.push_back( img );
    }
    return img;
}

OpenCVImage* OpenCVImageFactory::get( int width, int height, int depth, int channels )
{
    assert( width > 0 );
    assert( height > 0 );
    assert( channels > 0 && channels <= 4 );

    OpenCVImage* image = 0;
    for( std::list<OpenCVImage*>::iterator itr = m_objectPool.begin();
            image == 0 && itr != m_objectPool.end(); ++itr )
    {
        OpenCVImage* current = *itr;
        if( current->getRefCount() == 1 )
        {
            image = current;
        }
    }

    if( image == 0 )
    {
        IplImage* cvimg = cvCreateImage( cvSize( width, height ), depth, channels );
        image = new OpenCVImage( cvimg );

        // up the ref count by one
        image->inc();
        m_objectPool.push_back( image );
    }

    assert( image != 0 );

    return image;
}

/*****************************************************************************/

OpenCVImage::OpenCVImage( IplImage* img ) :
        Data(),
        m_img( img )
{
}

OpenCVImage::~OpenCVImage()
{
    cvReleaseImage( &m_img );
}

//OpenCVImage::OpenCVImage( const OpenCVImage& other ) :
//        Data()
//{
////    m_img = cvCreateImage( cvSize(other.m_img->width, other.m_img->height),
////                           other.m_img->depth, other.m_img->nChannels );
//
//    m_img = OpenCVImageFactory::instance()->getFromBuffer( other.m_img, false );
//    //cvCopyImage( other.m_img, m_img );
//}

IplImage* OpenCVImage::getImage()
{
    return m_img;
}

OpenCVImage* OpenCVImage::deepcopy()
{
    IplImage* img = cvCreateImage( cvSize(m_img->width, m_img->height),
                           m_img->depth, m_img->nChannels );
    cvCopyImage( m_img, img );
    return new OpenCVImage( img );
}

/** Compare two opencv images for type equality */
//bool OpenCVImage::isCompatibleFormat( const ComplexData* data )
//{
//    RefPtr<const OpenCVImage> img = dynamic_cast<const OpenCVImage*>(data);
//    if( img.isNotNull() && m_img != 0 )
//    {
//        IplImage* other = img->getImage();
//        return( m_img->nChannels == other->nChannels &&
//                m_img->depth     == other->depth &&
//                m_img->width     == other->width &&
//                m_img->height    == other->height );
//    }
//    return false;
//}
