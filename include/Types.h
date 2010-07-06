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
    protected:
        /** mutable flag used for determining if data resource can be
          * written to
          */
        bool m_mutable;

    public:
        Data() : m_mutable( true ) {}

        /** Copy constructor needs to be implemented by super classes
          * to allow the copying of a data resources when the Pin
          * connection type is set to copy which can be faster with
          * simple types.
          */
        Data( const Data& other );

        /** makes this data unit mutable again. Internal framework method.
          * Should normally not be called by client code
          */
        inline void makeMutable()
        {
            QMutexLocker( &this->m_refMutex );
            m_mutable = true;
        }

        /** makes this data element immutable. This is called by the framework when this element
          * is read only shared in multiple threads
          */
        inline void makeImmutable()
        {
            QMutexLocker( &this->m_refMutex );
            m_mutable = false;
        }

        inline bool isMutable() const
        {
            return m_mutable;
        }

    protected:
        /** protected destructor, data resources are not allowed to be
          * deleted by individual processor since they can be in use
          * by other processors in other threads. Deletion in one processor
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
