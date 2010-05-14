#include "OpenCVImage.h"
#include "RefPtr.h"

#include <opencv/cv.h>
#include <QMutexLocker>

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

OpenCVImage* OpenCVImageFactory::getFromBuffer( const IplImage* buffer )
{
    return getFromBuffer( const_cast<IplImage*>( buffer ), false );
}

OpenCVImage* OpenCVImageFactory::getFromBuffer( IplImage* buffer, bool own )
{
    QMutexLocker lock( &m_factoryMutex );

    assert( buffer != 0 );
    OpenCVImage* img = 0;

    if( !own )
    {
        img = getOrCreate( buffer->width, buffer->height,
                           buffer->depth, buffer->nChannels);
        cvCopyImage( buffer, img->getImageForWriting() );
        img->releaseImageForWriting();
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
    QMutexLocker lock( &m_factoryMutex );
    return getOrCreate( width, height, depth, channels );
}

OpenCVImage* OpenCVImageFactory::getOrCreate( int width, int height, int depth, int channels )
{
    assert( width > 0 );
    assert( height > 0 );
    assert( channels > 0 && channels <= 4 );

    OpenCVImage* image = 0;
    for( std::list<OpenCVImage*>::iterator itr = m_objectPool.begin();
            image == 0 && itr != m_objectPool.end(); ++itr )
    {
        OpenCVImage* current = *itr;
        if( current->getRefCount() == 1 &&
            current->isCompatible( width, height, depth, channels ) )
        {
            image = current;
            m_objectPool.erase( itr );
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
    m_img = 0;
}

OpenCVImage* OpenCVImage::deepCopy() const
{
    QReadLocker lock( &m_lock );

    OpenCVImage* img = OpenCVImageFactory::instance()->get( m_img->width, m_img->height,
                                                            m_img->depth, m_img->nChannels );
    IplImage* iplimg = img->getImageForWriting();
    cvCopyImage( m_img, iplimg );
    img->releaseImageForWriting();

    return img;
}

IplImage* OpenCVImage::getImageForWriting()
{
    m_lock.lockForWrite();
    return m_img;
}

void OpenCVImage::releaseImageForWriting()
{
    m_lock.unlock();
}

bool OpenCVImage::isCompatible( int width, int height, int depth, int channels )
{
    QReadLocker lock( &m_lock );

    if( m_img == 0 )
        return false;
    else
    {
        return( m_img->width     == width &&
                m_img->height    == height &&
                m_img->nChannels == channels &&
                m_img->depth     == depth );
    }
}
