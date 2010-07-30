#include "OpenCVImage.h"
#include "RefPtr.h"

#include <opencv/cv.h>
#include <QMutexLocker>

using namespace plv;

OpenCVImageFactory* OpenCVImageFactory::m_instance = 0;

OpenCVImageFactory::OpenCVImageFactory( int maxObjectPoolSize ) :
    m_maxObjectPoolSize( maxObjectPoolSize ), m_objectPoolSize( 0 )
{
}

OpenCVImageFactory::~OpenCVImageFactory()
{
    purgeAll();
}

void OpenCVImageFactory::purge()
{
    for( std::list<OpenCVImage*>::iterator itr = m_objectPool.begin();
            itr != m_objectPool.end(); ++itr )
    {
        OpenCVImage* img = *itr;
        m_objectPoolSize -= img->size();
        if( img->getRefCount() == 1 )
        {
            // this will auto delete the image
            img->dec();
            m_objectPool.erase( itr );
        }
    }
}

void OpenCVImageFactory::purgeAll()
{
    for( std::list<OpenCVImage*>::iterator itr = m_objectPool.begin();
            itr != m_objectPool.end(); ++itr )
    {
        OpenCVImage* img = *itr;
        m_objectPoolSize -= img->size();
        if( img->getRefCount() > 1 )
        {
            qDebug() << "OpenCVImageFactory::purgeAll() called when object " <<
                    img << " has reference count of " << img->getRefCount();
            qDebug() << "WARNING. Forcing delete.";
            delete img;
        }
        else
        {
            // this will auto delete the image
            img->dec();
        }
        m_objectPool.erase( itr );
    }
}

int OpenCVImageFactory::objectPoolSize()
{
    return m_objectPoolSize;
}

int OpenCVImageFactory::maxObjectPoolSize()
{
    return m_maxObjectPoolSize;
}

int OpenCVImageFactory::numObjects()
{
    return m_objectPool.size();
}

int OpenCVImageFactory::numObjectsInUse()
{
    int count = 0;
    for( std::list<OpenCVImage*>::iterator itr = m_objectPool.begin();
            itr != m_objectPool.end(); ++itr )
    {
        OpenCVImage* img = *itr;
        if( img->getRefCount() > 1 )
        {
            ++count;
        }
    }
    return count;
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
    }
    else
    {
        img = new OpenCVImage( buffer );

#if OPENCVIMAGE_USE_POOL
        // up the ref count by one and add to pool
        img->inc();
        m_objectPool.push_back( img );
        m_objectPoolSize += img->size();
#endif
    }
    return img;
}

OpenCVImage* OpenCVImageFactory::get( int width, int height, int depth,
                                      int channels )
{
    QMutexLocker lock( &m_factoryMutex );
    return getOrCreate( width, height, depth, channels );
}

OpenCVImage* OpenCVImageFactory::getOrCreate( int width, int height, int depth,
                                              int channels )
{
    // this function is private and should only be called when the mutex
    // has already been acquired
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

            // reset mutable state so this image can be freely written to
            image->makeMutable();
        }
    }

    if( image == 0 )
    {
        IplImage* cvimg = cvCreateImage( cvSize( width, height ), depth, channels );
        image = new OpenCVImage( cvimg );

#if OPENCVIMAGE_USE_POOL
        // up the ref count by one
        image->inc();
        m_objectPoolSize += image->size();
        m_objectPool.push_back( image );
#endif
    }

    assert( image != 0 );
    assert( image->isMutable() );

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

IplImage* OpenCVImage::getImageForWriting() throw ( IllegalAccessException )
{
    if( !isMutable() )
    {
        throw IllegalAccessException( "Tried to access image data on an immutable image." );
    }
    return m_img;
}

OpenCVImage* OpenCVImage::deepCopy() const
{
    QMutexLocker lock( &m_imgLock );

    OpenCVImage* img = OpenCVImageFactory::instance()->get( m_img->width, m_img->height,
                                                            m_img->depth, m_img->nChannels );
    IplImage* iplimg = img->getImageForWriting();
    cvCopyImage( m_img, iplimg );
    return img;
}

int OpenCVImage::size() const
{
    if( !m_img ) return 0;
    return m_img->imageSize;
}

bool OpenCVImage::isCompatible( int width, int height, int depth, int channels )
{
    QMutexLocker lock( &m_imgLock );

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
