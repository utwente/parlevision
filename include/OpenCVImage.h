#ifndef OPENCVIMAGE_H
#define OPENCVIMAGE_H

#include <list>
//#include <opencv/cv.h>

typedef struct _IplImage IplImage;

#include "Types.h"

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

        inline static OpenCVImageFactory* instance()
        {
            if( m_instance == 0 )
                m_instance = new OpenCVImageFactory();
            return m_instance;
        }

    private:
        OpenCVImageFactory();
        ~OpenCVImageFactory();

    private:
        static OpenCVImageFactory* m_instance;
        std::list<OpenCVImage*> m_objectPool;
    };

    class OpenCVImage : public Data
    {
        friend class OpenCVImageFactory;

    public:
        int getId() const { return m_id; }
        const IplImage* getImageReadOnly() const { return m_img; }
        IplImage* getImage();
        OpenCVImage* deepcopy();

    protected:
        OpenCVImage( IplImage* img );
        OpenCVImage( const OpenCVImage& other );

        virtual ~OpenCVImage();

    protected:
        unsigned int m_id;
        IplImage* m_img;
    };
}

#endif
