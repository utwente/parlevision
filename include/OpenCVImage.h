#ifndef OPENCVIMAGE_H
#define OPENCVIMAGE_H

#include <opencv/cv.h>

#include "Types.h"

namespace plv {

class OpenCVImage : public Resource
{
protected:
    int m_id;
    IplImage* m_img;

public:
    int getId() const { return m_id; }
    IplImage* getImage() const { return m_img; }

    OpenCVImage( int id, IplImage* img );
    virtual ~OpenCVImage();

    /** implementation of the isCompatible method of the Resource abstract
      * base class
      */
    virtual bool isCompatible( const Resource* other );
};

}

#endif
