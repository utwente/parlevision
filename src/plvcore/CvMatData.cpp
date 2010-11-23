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

#include "CvMatData.h"
#include "RefPtr.h"

#include <opencv/cv.hpp>
#include <QMutexLocker>

using namespace plv;

//CvMatDataFactory* CvMatDataFactory::m_instance = 0;

//CvMatDataFactory::CvMatDataFactory( int maxObjectPoolSize ) :
//    m_maxObjectPoolSize( maxObjectPoolSize ), m_objectPoolSize( 0 )
//{
//}

//CvMatDataFactory::~CvMatDataFactory()
//{
//    QMutexLocker lock( &m_factoryMutex );
//    purgeAll();
//}

//void CvMatDataFactory::purge()
//{
//    std::list<CvMatData*>::iterator itr = m_objectPool.begin();

//    while(itr != m_objectPool.end())
//    {
//        CvMatData* img = *itr;
//        if( img->getRefCount() == 1 )
//        {
//            // this will auto delete the image
//            itr = m_objectPool.erase( itr );
//            m_objectPoolSize -= img->size();
//            img->dec();
//        }
//        else
//        {
//            ++itr;
//        }
//    }
//}

//void CvMatDataFactory::purgeAll()
//{
//    QMutexLocker lock( &m_factoryMutex );

//    std::list<CvMatData*>::iterator itr = m_objectPool.begin();
//    while(itr != m_objectPool.end())
//    {
//        CvMatData* img = *itr;
//        m_objectPoolSize -= img->size();
//        if( img->getRefCount() > 1 )
//        {
//            qWarning() << "WARNING. Forcing delete of object "
//                       << img << " which has a reference count of " << img->getRefCount()
//                       << " in CvMatDataFactory::purgeAll()";
//            delete img;
//        }
//        else
//        {
//            // this will auto delete the image
//            img->dec();
//        }
//        itr = m_objectPool.erase( itr );
//    }
//}

//int CvMatDataFactory::objectPoolSize()
//{
//    QMutexLocker lock( &m_factoryMutex );
//    return m_objectPoolSize;
//}

//int CvMatDataFactory::maxObjectPoolSize()
//{
//    QMutexLocker lock( &m_factoryMutex );
//    return m_maxObjectPoolSize;
//}

//int CvMatDataFactory::numObjects()
//{
//    QMutexLocker lock( &m_factoryMutex );
//    return m_objectPool.size();
//}

//int CvMatDataFactory::numObjectsInUse()
//{
//    QMutexLocker lock( &m_factoryMutex );
//    int count = 0;
//    for( std::list<CvMatData*>::iterator itr = m_objectPool.begin();
//            itr != m_objectPool.end(); ++itr )
//    {
//        CvMatData* img = *itr;
//        if( img->getRefCount() > 1 )
//        {
//            ++count;
//        }
//    }
//    return count;
//}

//CvMatData* CvMatDataFactory::get( int width, int height, int depth, int channels )
//{
//    QMutexLocker lock( &m_factoryMutex );
//    return recycleOrAllocate( width, height, depth, channels );
//}

//CvMatData* CvMatDataFactory::recycleOrAllocate( cv::Size size, int type )
//{
//    // this function is private and should only be called when the mutex
//    // has already been acquired
//    assert( width > 0 );
//    assert( height > 0 );
//    assert( channels > 0 && channels <= 4 );

//    CvMatData* image = 0;
//    for( std::list<CvMatData*>::iterator itr = m_objectPool.begin();
//            image == 0 && itr != m_objectPool.end(); ++itr )
//    {
//        CvMatData* current = *itr;

//        if( current->getRefCount() == 1 &&
//            current->isCompatible( width, height, depth, channels ) )
//        {
//            image = current;

//            // reset mutable state so this image can be freely written to
//            image->makeMutable();
//        }
//    }

//    if( image == 0 )
//    {
//        IplImage* cvimg = cvCreateImage( cvSize( width, height ), depth, channels );
//        image = new CvMatData( cvimg );

//#if CVMATDATA_USE_POOL
//        if( m_objectPoolSize > CVMATDATA_MAX_OBJECT_POOL_SIZE )
//        {
//            purge();
//        }

//        // up the ref count by one
//        image->inc();
//        m_objectPool.push_back( image );
//        m_objectPoolSize += image->size();
//#endif
//    }

//    assert( image != 0 );
//    assert( image->isMutable() );

//    return image;
//}

/*****************************************************************************/

CvMatData::CvMatData( IplImage* img, bool copyData ) :
        Data()
{
    m_mat = new cv::Mat( img, copyData );
}

CvMatData::~CvMatData()
{
    if( m_mat != 0 )
        delete m_mat;
}

cv::Mat* CvMatData::getForWriting() throw ( plv::PlvRuntimeException )
{
    if( !isMutable() )
    {
        throw plv::PlvRuntimeException( "Tried to access image data on an immutable image.",
                                        __FILE__, __LINE__ );
    }
    return m_mat;
}

//CvMatData* CvMatData::deepCopy() const
//{
//    QMutexLocker lock( &m_imgLock );

//    CvMatData* matWrapper = CvMatDataFactory::get( m_mat->getProperties() );
//    cv::Mat* mat = matWrapper->getImageForWriting();
//    cvCopyImage( m_img, iplimg );
//    return img;
//}

//int CvMatData::size() const
//{
//    QMutexLocker lock( &m_imgLock );
//    if( !m_mat ) return 0;
//    return m_mat->;
//}

//bool CvMatData::isCompatibleDimensions( const CvMatData* other ) const
//{
//    QMutexLocker lock( &m_imgLock );
//    return getWidth() == other->getWidth() && getHeight() == other->getHeight();
//}

//bool CvMatData::isCompatibleDepth( const CvMatData* other ) const
//{
//    QMutexLocker lock( &m_imgLock );
//    return getDepth() == other->getDepth();
//}

//bool CvMatData::isCompatibleSize( const CvMatData* other ) const
//{
//    QMutexLocker lock( &m_imgLock );
//    return getNumChannels() == other->getNumChannels();
//}

//bool CvMatData::isCompatible( const CvMatData* other, ImageCompare flags ) const
//{
//    if( this->isNull() || other->isNull() ) return false;

//    bool equals = false;

//    if( flags != 0 )
//    {
//        equals = true;
//        if((flags & DEPTH) == DEPTH )
//        {
//            equals = isCompatibleDepth( other );
//        }
//        if( equals && ((flags & DIMENSIONS) == DIMENSIONS))
//        {
//            equals = isCompatibleDimensions( other );
//        }
//        if( equals && ((flags & CHANNELS) == CHANNELS))
//        {
//            equals = isCompatibleSize( other );
//        }
//    }
//    return equals;
//}

//bool CvMatData::isCompatible( int width, int height, int depth, int channels ) const
//{
//    QMutexLocker lock( &m_imgLock );

//    if( m_img == 0 )
//        return false;
//    else
//    {
//        return( m_img->width     == width &&
//                m_img->height    == height &&
//                m_img->nChannels == channels &&
//                m_img->depth     == depth );
//    }
//}

/*
CV_BGR2BGRA     Convert BGR color to BGRA color
CV_BGRA2BGR     Convert BGRA color to BGR color
CV_BGR2GRAY     Convert BGR color to GRAY color
CV_GRAY2BGR     Convert GRAY color to BGR color
CV_GRAY2BGRA    Convert GRAY color to BGRA color
CV_BGRA2GRAY    Convert BGRA color to GRAY color
CV_BGR2BGR565   Convert BGR color to BGR565 color
CV_BGR5652BGR   Convert BGR565 color to BGR color
CV_BGRA2BGR565  Convert BGRA color to BGR565 color
CV_BGR5652BGRA  Convert BGR565 color to BGRA color
CV_GRAY2BGR565  Convert GRAY color to BGR565 color
CV_BGR5652GRAY  Convert BGR565 color to GRAY color
CV_BGR2BGR555   Convert BGR color to BGR555 color
CV_BGR5552BGR   Convert BGR555 color to BGR color
CV_BGRA2BGR555  Convert BGRA color to BGR555 color
CV_BGR5552BGRA  Convert BGR555 color to BGRA color
CV_GRAY2BGR555  Convert GRAY color to BGR555 color
CV_BGR5552GRAY  Convert BGR555 color to GRAY color
CV_BGR2XYZ      Convert BGR color to XYZ color
CV_XYZ2BGR      Convert XYZ color to BGR color
CV_BGR2YCrCb    Convert BGR color to YCrCb color
CV_YCrCb2BGR    Convert YCrCb color to BGR color
CV_BGR2HSV      Convert BGR color to HSV color
CV_BGR2Lab      Convert BGR color to Lab color
CV_BayerBG2BGR  Convert BayerBG color to BGR color
CV_BayerGB2BGR  Convert BayerGB color to BGR color
CV_BayerRG2BGR  Convert BayerRG color to BGR color
CV_BayerGR2BGR  Convert BayerGR color to BGR color
CV_BGR2Luv      Convert BGR color to Luv color
CV_BGR2HLS      Convert BGR color to HLS color
CV_HSV2BGR      Convert HSV color to BGR color
CV_Lab2BGR      Convert Lab color to BGR color
CV_Luv2BGR      Convert Luv color to BGR color
CV_HLS2BGR      Convert HLS color to BGR color
*/
