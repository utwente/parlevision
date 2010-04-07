#include <opencv/cv.h>

#include "Types.h"

namespace plv {

class OpenCVImage : public Resource
{
protected:
    int m_id;
    IplImage* m_img;

    OpenCVImage( int id, IplImage* img );
    ~OpenCVImage();

public:
    int getId() const { return m_id; }
    IplImage* getImage() const { return m_img; }

    /** implementation of the isCompatible method of the Resource abstract
      * base class
      */
    virtual bool isCompatible( const Resource* other );
};

}
