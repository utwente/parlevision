#ifndef QtImage_H
#define QtImage_H

#include <QImage>

#include "Types.h"

namespace plv {

class QtImage : public Resource
{
protected:
    int m_id;
    QImage* m_img;

     virtual ~QtImage();

public:
    QtImage( int id, QImage* img );

    int getId() const { return m_id; }
    QImage* getImage() const { return m_img; }

    virtual bool isCompatible( const Resource* other );
};

}

#endif // QtImage_H
