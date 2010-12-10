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

#if 0

#ifndef OPENCVIMAGE_H
#define OPENCVIMAGE_H

#include <list>
#include <QMutex>
#include <opencv/cv.h>

#include "Types.h"
#include "PlvExceptions.h"

#define OPENCVIMAGE_MAX_OBJECT_POOL_SIZE (1024 * 1024 * 8)
#define OPENCVIMAGE_USE_POOL 1

namespace plv
{
    class PLVCORE_EXPORT OpenCVImageProperties
    {
    protected:
        int m_width;
        int m_height;
        int m_depth;
        int m_numChannels;

    public:
        OpenCVImageProperties( int width,int height, int depth, int nChannels ) :
                m_width(width), m_height(height),
                m_depth(depth), m_numChannels(nChannels) {}

        inline bool operator == (const OpenCVImageProperties& rhs ) const
        {
            return m_width == rhs.m_width &&
                   m_height == rhs.m_height &&
                   m_depth == rhs.m_depth &&
                   m_numChannels == rhs.m_numChannels;
        }

        inline int getWidth() const { return m_width; }
        inline int getHeight() const { return m_height; }
        inline int getDepth() const { return m_depth; }
        inline int getNumChannels() const { return m_numChannels; }

        inline void setWidth( int width ) { m_width = width; }
        inline void setHeight( int height ) { m_height = height; }
        inline void setDepth( int depth ) { m_depth = depth; }
        inline void setNumChannels( int numChannels ) { m_numChannels = numChannels; }
    };

    class OpenCVImage;

    class PLVCORE_EXPORT OpenCVImageFactory
    {
    public:

        /** constructs an OpenCVImage from an existing buffer. Default operation
          * is that the newly constructed OpenCVImage does not own the buffer,
          * and that a copy is created of the contents of the buffer. When own
          * is true, the newly constructed OpenCVImage becomes the owner of
          * the buffer, meaning it can be deleted by the OpenCVImage. Deleting
          * the buffer externally results in undefined behaviour.
          */
        OpenCVImage* getFromBuffer( IplImage* buffer, bool own = false );

        /** Same as getFromBuffer called with non const buffer and own false. It
          * will create a copy from the buffer.
          */
        OpenCVImage* getFromBuffer( const IplImage* buffer );

        /** Will return an OpenCVImage with appropropriate format. Uses object
          * pooling to recycle images which are no longer in use by the pipeline.
          * @returns an OpenCVImage with appropriate format
          */
        OpenCVImage* get( int width, int height, int depth, int channels );

        static inline void get( RefPtr<OpenCVImage>& rv, const OpenCVImageProperties& props )
        {
            rv = OpenCVImageFactory::instance()->get( props.getWidth(), props.getHeight(),
                                                      props.getDepth(), props.getNumChannels() );
        }

        static inline RefPtr<OpenCVImage> get( const OpenCVImageProperties& props )
        {
            RefPtr<OpenCVImage> img;
            img = OpenCVImageFactory::instance()->get( props.getWidth(), props.getHeight(),
                                                      props.getDepth(), props.getNumChannels() );
            return img;
        }

        /** @returns the number of object in the object pool */
        int numObjects();

        /** @returns the number of objects in the object pool which are in use */
        int numObjectsInUse();

        /** @returns the size of the object pool in bytes */
        int objectPoolSize();

        /** @returns the maximum size of the object pool in bytes */
        int maxObjectPoolSize();

        /** Sets the maximum size of the object pool in bytes. Default is
          * OPENCVIMAGE_OBJECT_POOL_SIZE_DEFAULT.
          */
        void setObjectPoolSize( int bytes );

        inline static OpenCVImageFactory* instance()
        {
            if( m_instance == 0 )
                m_instance = new OpenCVImageFactory();
            return m_instance;
        }

    private:
        OpenCVImageFactory( int maxObjectPoolSize = OPENCVIMAGE_MAX_OBJECT_POOL_SIZE );
        ~OpenCVImageFactory();

        OpenCVImage* getOrCreate( int width, int height, int depth, int channels );

        /** Purges all objects which are not in used from the object pool. Not thread safe. */
        void purge();

        /** Purges all objects, even those in use (refcount > 1). Dangerous! */
        void purgeAll();

    private:
        static OpenCVImageFactory* m_instance;  /** singleton class instance */
        std::list<OpenCVImage*> m_objectPool;
        int m_maxObjectPoolSize;    /** max object pool size in bytes */
        int m_objectPoolSize;       /** object pool size in bytes */
        QMutex m_factoryMutex;
    };

    class PLVCORE_EXPORT OpenCVImage : public Data
    {
        friend class OpenCVImageFactory;

    public:
        typedef enum ImageCompare {
            DIMENSIONS = 1,
            DEPTH = 2,
            CHANNELS = 4,
            ALL = 7
        } ImageCompare;

        inline int getWidth() const { return m_img->width; }
        inline int getHeight() const { return m_img->height; }
        inline int getNumChannels() const { return m_img->nChannels; }
        inline int getDepth() const { return m_img->depth; }

        inline OpenCVImageProperties getProperties() const
        {
            return OpenCVImageProperties( getWidth(), getHeight(),
                                          getDepth(), getNumChannels() );
        }

        inline bool isNull() const { return m_img == 0; }

        /** returns a const pointer to the internal IplImage.
          * TODO fix documentation
          * Use the OpenCVImageWriter class if you want to have non-const access
          * to the internal IplImage.
          */
        const IplImage* getImage() const { return m_img; }

        IplImage* getImageForWriting() throw ( PlvRuntimeException );

        /** @returns a deep copy of this OpenCVImage, including a copy of the internal
          * IplImage.
          */
        OpenCVImage* deepCopy() const;

        bool isCompatibleDimensions( const OpenCVImage* other ) const;
        bool isCompatibleDepth( const OpenCVImage* other ) const;
        bool isCompatibleSize( const OpenCVImage* other ) const;

        /** Compare two opencv images for type equality */
        bool isCompatible( const OpenCVImage* other, ImageCompare compareType = ALL ) const;

        inline bool isCompatible( const RefPtr<OpenCVImage>& other, ImageCompare compareType = ALL ) const
        {
            return isCompatible( other.getPtr(), compareType );
        }

        /** Compare this opencv images for type equality to parameters */
        bool isCompatible( int width, int height, int depth, int channels ) const;

        /** @returns the size of the contained IplImage image data in bytes */
        int size()const;

        static const char* depthToString( int depth );

    protected:
        OpenCVImage( IplImage* img );
        OpenCVImage( const OpenCVImage& other );
        virtual ~OpenCVImage();

        IplImage* m_img;
        mutable QMutex m_imgLock;
    };

    class PLVCORE_EXPORT OpenCVImages : public Data
    {
        QList< RefPtr<OpenCVImage> > m_images;

    public:
        void addImage( plv::RefPtr<OpenCVImage>& img ) { m_images.append( img ); }
        inline QList< RefPtr<OpenCVImage> > getImages() const { return m_images; }
    };
}

#endif
#endif
