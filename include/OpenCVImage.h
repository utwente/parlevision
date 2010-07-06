#ifndef OPENCVIMAGE_H
#define OPENCVIMAGE_H

#include <list>
#include <QMutex>
#include <QReadWriteLock>
#include <opencv/cv.h>

//typedef struct _IplImage IplImage;

#include "Types.h"
#include "PlvExceptions.h"

#define OPENCVIMAGE_MAX_OBJECT_POOL_SIZE (1024 * 1024 * 8)

namespace plv
{
    class OpenCVImage;

    class OpenCVImageFactory
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

        /** Purges all objects which are not in used from the object pool */
        void purge();

        /** Purges all objects, even those in use (refcount > 1). Dangerous! */
        void purgeAll();

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

    private:
        static OpenCVImageFactory* m_instance;  /** singleton class instance */
        std::list<OpenCVImage*> m_objectPool;
        int m_maxObjectPoolSize;    /** max object pool size in bytes */
        int m_objectPoolSize;       /** object pool size in bytes */
        QMutex m_factoryMutex;
    };

    class OpenCVImage : public Data
    {
        friend class OpenCVImageFactory;
        friend class OpenCVImageWriter;

    public:
        inline int getWidth() { return m_img->width; }
        inline int getHeight() { return m_img->height; }
        inline int getNumChannels() { return m_img->nChannels; }
        inline int getDepth() { return m_img->depth; }


        /** returns a const pointer to the internal IplImage.
          * TODO fix documentation
          * Use the OpenCVImageWriter class if you want to have non-const access
          * to the internal IplImage.
          */
        const IplImage* getImage() const { return m_img; }

        IplImage* getImageForWriting() throw ( IllegalAccessException );

        /** @returns a deep copy of this OpenCVImage, including a copy of the internal
          * IplImage.
          */
        OpenCVImage* deepCopy() const;

        /** Compare two opencv images for type equality */
        bool isCompatible( int width, int height, int depth, int channels );

        /** @returns the size of the contained IplImage image data in bytes */
        int size()const;

    protected:
        OpenCVImage( IplImage* img );
        OpenCVImage( const OpenCVImage& other );

        virtual ~OpenCVImage();

//        IplImage* getImageForWriting();
//        void releaseImageForWriting();

    protected:
        IplImage* m_img;
        mutable QMutex m_imgLock;
    };

//    class OpenCVImageWriter
//    {
//    public:
//        OpenCVImageWriter( OpenCVImage* img ) :
//            m_img( img ),
//            m_iplImg( 0 )
//        {
//        }
//
//        ~OpenCVImageWriter()
//        {
//            release();
//        }
//
//        inline IplImage* get()
//        {
//            if( m_iplImg == 0 )
//                m_iplImg = m_img->getImageForWriting();
//            return m_iplImg;
//        }
//
//        inline void release()
//        {
//            if( m_iplImg != 0 )
//                m_img->releaseImageForWriting();
//            m_iplImg = 0;
//        }
//
//    protected:
//        OpenCVImage* m_img;
//        IplImage* m_iplImg;
//    };
}

#endif
