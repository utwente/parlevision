#ifndef TYPES_H
#define TYPES_H

//#include <opencv/cv.h>
#include <QString>
#include "RefCounted.h"

namespace plv {

    class Data
    {
    };

    /** Base class for data resources which contain pointers and
      * need to be shared between threads instead of copied.
      */
    class ReferencedData : public Data, public RefCounted
    {
    public:
        ReferencedData() {}
    protected:
        virtual ~ReferencedData() {};
    private:
        /** no implicit copy of object allowed */
        ReferencedData( const ReferencedData& other );
    };

    /** Base class for data resources which contain only primitve types
      * and no pointers and can be copied between threads instead of shared.
      */
    class ValueData : public Data
    {
    public:
        ValueData() {}
    };

    /** This method compares the type and parameters of this object, with
      * the type and parameters of the other object. It returns true if the
      * types are compatible from a resource persepctive. This is not the same
      * as the typeid C++ operator, and compares additional information such as
      * private member fields to determine compatibility between two objects.
      */
    //virtual bool isCompatibleFormat( const ComplexData* other ) = 0;


    }

#endif // TYPES_H
