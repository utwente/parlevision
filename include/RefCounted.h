#ifndef REFCOUNTED_H
#define REFCOUNTED_H

#include <QMutex>
#include <QMutexLocker>
#include <QDebug>

namespace plv {

class RefCounted
{
private:
    mutable int m_referenceCount;
    mutable QMutex m_refMutex;

public:
    /** Initializes reference count to 0 */
    RefCounted();

    /** Copy constructor of RefCounted objects initializes
      * the reference count of the copy to 0.
      */
    explicit RefCounted( const RefCounted& )
        : m_referenceCount( 0 )
    {
    }

    /** increases reference count by one */
    inline void inc() const
    {
        QMutexLocker lock( &m_refMutex );
        ++m_referenceCount;
    }

    /** decreases reference count by one. Deletes this object if
      * reference count becomes 0
      */
    inline void dec() const
    {
        m_refMutex.lock();
        --m_referenceCount;
        if( m_referenceCount < 1 )
        {
            m_refMutex.unlock();
            delete this;
            return;
        }
        m_refMutex.unlock();
    }

#if 0
    virtual void inc() const
    {
        QMutexLocker lock( &m_refMutex );
        ++m_referenceCount;
    }

    virtual void dec() const
    {
        m_refMutex.lock();
        --m_referenceCount;
        if( m_referenceCount < 1 )
        {
            m_refMutex.unlock();
            delete this;
            return;
        }
        m_refMutex.unlock();
    }
#endif

    inline int getRefCount() const
    {
        QMutexLocker lock( &m_refMutex );
        return m_referenceCount;
    }

    /** RefCounted objects should not explicitely be destructed!
      */
    virtual ~RefCounted();



};

}

#endif // REFCOUNTED_H
