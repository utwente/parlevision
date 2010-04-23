#ifndef REFCOUNTED_H
#define REFCOUNTED_H

#include <QMutex>
#include <QMutexLocker>
#include <QDebug>

namespace plv {

class RefCounted
{
protected:
    mutable int m_referenceCount;
    mutable QMutex m_mutex;

public:
    /** Initializes reference count to 0 */
    RefCounted();

    /** Copy constructor of RefCounted objects initializes
      * the reference count of the copy to 0.
      */
    explicit RefCounted( const RefCounted& other )
        : m_referenceCount( 0 )
    {
    }

    /** increases reference count by one */
    void inc() const
    {
        QMutexLocker lock( &m_mutex );
        ++m_referenceCount;
    }

    /** decreases reference count by one. Deletes this object if
      * reference count becomes 0
      */
    void dec() const
    {
        QMutexLocker lock( &m_mutex );
        --m_referenceCount;
        if( m_referenceCount < 1 )
        {
            delete this;
        }
    }

    inline int getRefCount() const
    {
        QMutexLocker lock( &m_mutex );
        return m_referenceCount;
    }

    /** RefCounted objects should not explicitely be destructed!
      */
    virtual ~RefCounted();



};

}

#endif // REFCOUNTED_H
