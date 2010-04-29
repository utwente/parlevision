#ifndef QtImage_H
#define QtImage_H

#include <QImage>

#include "Types.h"

namespace plv {

class QtImage : public Data
{
protected:
    QImage m_img;

public:
    QtImage( QImage img );
    QtImage( const QtImage& other );
    
    /** get the image from this container */
    QImage getImage() const { return m_img; }

    /** returns if this image is compatible with another QtImage */
    //virtual bool isCompatible( const ComplexData* other );
    virtual ~QtImage();
};

}

/** declare the RefPtr template instantiated with the QtImage class as a Qt Metatype
  *  so we can pass RefPtr<QtImage> along with signals and slots across thread boundaries
  */
Q_DECLARE_METATYPE( plv::RefPtr<plv::QtImage> );

#endif // QtImage_H
