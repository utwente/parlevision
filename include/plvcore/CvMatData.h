/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvopencv module of ParleVision.
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

#ifndef CVMATDATA_H
#define CVMATDATA_H

#include <list>
#include <QMutex>
#include <opencv/cv.h>

#include "Types.h"
#include "PlvExceptions.h"

#define CVMATDATA_MAX_OBJECT_POOL_SIZE (1024 * 1024 * 8)
#define CVMATDATA_USE_POOL 1

namespace plv
{
    class CvMatProperties
    {
    protected:
        int m_width;
        int m_height;
        int m_type;

    public:
        CvMatProperties( int width,int height, int type ) :
                m_width(width), m_height(height), m_type(type) {}

        inline bool operator == (const CvMatProperties& rhs ) const
        {
            return m_width == rhs.m_width &&
                   m_height == rhs.m_height &&
                   m_type == rhs.m_type;
        }

        inline int getWidth() const { return m_width; }
        inline int getHeight() const { return m_height; }
        inline int getType() const { return m_type; }
        inline cv::Size getSize() const { return cv::Size( m_width, m_height ); }
    };
}

namespace plv
{

//    class CvMatData;

//    class PLVCORE_EXPORT CvMatDataFactory
//    {
//    public:

//        /** constructs an CvMatData from an existing buffer. Default operation
//          * is that the newly constructed CvMatData does not own the buffer,
//          * and that a copy is created of the contents of the buffer. When own
//          * is true, the newly constructed CvMatData becomes the owner of
//          * the buffer, meaning it can be deleted by the CvMatData. Deleting
//          * the buffer externally results in undefined behaviour.
//          */
////        CvMatData* getFromBuffer( IplImage* buffer, bool own = false );
////        CvMatData* getFromBuffer( cv::Mat* buffer, bool own = false );

//        /** Same as getFromBuffer called with non const buffer and own false. It
//          * will create a copy from the buffer.
//          */
////        CvMatData* getFromBuffer( const IplImage* buffer );
////        CvMatData* getFromBuffer( const cv::Mat* buffer );

//        /** Will return an CvMatData with appropropriate format. Uses object
//          * pooling to recycle images which are no longer in use by the pipeline.
//          * @returns an CvMatData with appropriate format
//          */
//        CvMatData* get( int width, int height, int depth, int channels );
//        //CvMatData* get( cv::Size size, int type );
//        CvMatData* get( const CvMatProperties& props );

////        inline static CvMatData* get( int width, int height, int depth, int channels )
////        {
////            return CvMatDataFactory::instance()->get( width, height, depth, channels );
////        }

////        inline static CvMatData* get( const CvMatProperties& props )
////        {
////            return CvMatDataFactory::instance()->get( props );
////        }

//        /** @returns the number of object in the object pool */
//        int numObjects();

//        /** @returns the number of objects in the object pool which are in use */
//        int numObjectsInUse();

//        /** @returns the size of the object pool in bytes */
//        int objectPoolSize();

//        /** @returns the maximum size of the object pool in bytes */
//        int maxObjectPoolSize();

//        /** Sets the maximum size of the object pool in bytes. Default is
//          * CVMATDATA_OBJECT_POOL_SIZE_DEFAULT.
//          */
//        void setObjectPoolSize( int bytes );

//        inline static CvMatDataFactory* instance()
//        {
//            if( m_instance == 0 )
//                m_instance = new CvMatDataFactory();
//            return m_instance;
//        }

//    private:
//        CvMatDataFactory( int maxObjectPoolSize = CVMATDATA_MAX_OBJECT_POOL_SIZE );
//        ~CvMatDataFactory();

//        CvMatData* recycleOrAllocate( cv::Size size, int type );

//        /** Purges all objects which are not in used from the object pool. Not thread safe. */
//        void purge();

//        /** Purges all objects, even those in use (refcount > 1). Dangerous! */
//        void purgeAll();

//    private:
//        static CvMatDataFactory* m_instance;  /** singleton class instance */
//        std::list<CvMatData*> m_objectPool;
//        int m_maxObjectPoolSize;    /** max object pool size in bytes */
//        int m_objectPoolSize;       /** object pool size in bytes */
//        QMutex m_factoryMutex;
//    };

    class PLVCORE_EXPORT CvMatData : public Data
    {
        friend class CvMatDataFactory;

    public:

        inline bool isValid() const { return m_mat != 0; }

        const cv::Mat* get() const;
        cv::Mat* getForWriting() throw ( PlvRuntimeException );

        /** @returns a deep copy of this CvMatData, including a copy of the
          * internal data.
          */
//        CvMatData* deepCopy() const;

//        bool isCompatibleDimensions( const CvMatData* other ) const;
//        bool isCompatibleDepth( const CvMatData* other ) const;
//        bool isCompatibleSize( const CvMatData* other ) const;

        /** Compare two opencv images for type equality */
//        bool isCompatible( const CvMatData* other, ImageCompare compareType = ALL ) const;

        /** Compare this opencv images for type equality to parameters */
//        bool isCompatible( int width, int height, int depth, int channels ) const;

        /** @returns the size of the contained matrix data in bytes */
//        int size()const;

    protected:
        CvMatData( cv::Mat* cvMat );

        /** Constructor. Converts IplImage to cv::Mat. Copies IplImage's data
            if copyData is true. Protected, use CvMatDataFactory::getFromBuffer
            instead.
          */
        CvMatData( IplImage* img, bool copyData = false );

        /** copy constructor. Copy's the wrapper but not the data which it
            references. */
        CvMatData( const CvMatData& other );

        /** destructor. Also calls destructor of internal shared matrix data.
            Is only called when reference count reaches 0 */
        ~CvMatData();

        /** the internal open cv matrix */
        cv::Mat* m_mat;

        /** mutex for thread safe data sharing */
        mutable QMutex m_imgLock;
    };
}

#endif
