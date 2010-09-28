/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvcore module of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

#include "OpenCVImage.h"
#include <plvcore/RefPtr.h>

#include <opencv/cv.h>
#include <QMutexLocker>

using namespace plv;
using namespace plvopencv;

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

bool OpenCVImage::isCompatibleDimensions( const OpenCVImage* other ) const
{
    return getWidth() == other->getWidth() && getHeight() == other->getHeight();
}

bool OpenCVImage::isCompatibleDepth( const OpenCVImage* other ) const
{
    return getDepth() == other->getDepth();
}

bool OpenCVImage::isCompatibleSize( const OpenCVImage* other ) const
{
    return getNumChannels() == other->getNumChannels();
}

bool OpenCVImage::isCompatible( const OpenCVImage* other, ImageCompare flags ) const
{
    if( this->isNull() || other->isNull() ) return false;

    bool equals = false;

    if( flags != 0 )
    {
        equals = true;
        if((flags & DEPTH) == DEPTH )
        {
            equals = isCompatibleDepth( other );
        }
        if( equals && ((flags & DIMENSIONS) == DIMENSIONS))
        {
            equals = isCompatibleDimensions( other );
        }
        if( equals && ((flags & CHANNELS) == CHANNELS))
        {
            equals = isCompatibleSize( other );
        }
    }
    return equals;
}

bool OpenCVImage::isCompatible( int width, int height, int depth, int channels ) const
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
