#ifndef TYPES_H
#define TYPES_H

#include <QMetaType>
#include "RefPtr.h"

namespace plv 
{

    /** Base class for data resources.
      *
      */
    class Data : public RefCounted
    {
    public:
        Data() {}

        /** Copy constructor needs to be implemented by super classes
          * to allow the copying of a data resources when the Pin
          * connection type is set to copy which can be faster with
          * simple types.
          */
        Data( const Data& other );

    protected:

        /** protected destructor, data resources are not allowed to be
          * deleted by individual processor since they can be in use
          * by other processors in other thread. Deletion in one processor
          * could cause a crash. Reference counting is used to let data
          * resources delete themselves.
          */
        virtual ~Data() {};
    };

}
/** declare the RefPtr template instantiated with the Data class as a Qt Metatype 
  *  so we can pass RefPtr<Data> along with signals and slots across thread boundaries
  */
Q_DECLARE_METATYPE( plv::RefPtr<plv::Data> );

#endif // TYPES_H
