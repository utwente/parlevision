#ifndef QtImage_H
#define QtImage_H

#include <QImage>

#include "Types.h"

namespace plv {

class QtImage : public Data
{
protected:
    int m_id;
    QImage* m_img;

    virtual ~QtImage();

public:
    QtImage( int id, QImage* img );
    QtImage( const QtImage& other );

    int getId() const { return m_id; }
    
    /** get the image from this container */
    const QImage* getImage() const { return m_img; }

    /** returns if this image is compatible with another QtImage */
    //virtual bool isCompatible( const ComplexData* other );
};

}

#endif // QtImage_H
