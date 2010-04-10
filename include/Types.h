#ifndef TYPES_H
#define TYPES_H

//#include <opencv/cv.h>
//#include <QString>
#include "RefCounted.h"

namespace plv {

/** Base class for resources */
class ComplexData : public RefCounted
{
public:
    ComplexData() {}

    /** This method compares the type and parameters of this object, with
      * the type and parameters of the other object. It returns true if the
      * types are compatible from a resource persepctive. This is not the same
      * as the typeid C++ operator, and compares additional information such as
      * private member fields to determine compatibility between two objects.
      */
    virtual bool isCompatibleFormat( const ComplexData* other ) = 0;

private:

    /** no implicit copy of object allowed */
    ComplexData( const ComplexData& other );
};

/** Base class for resources */
class SimpleData
{
public:
    SimpleData() {}

    /** This method compares the type and parameters of this object, with
      * the type and parameters of the other object. It returns true if the
      * types are compatible from a resource persepctive. This is not the same
      * as the typeid C++ operator, and compares additional information such as
      * private member fields to determine compatibility between two objects.
      */
    virtual bool isCompatibleFormat( const SimpleData& other ) = 0;
};



}

#endif // TYPES_H
