#ifndef REFCOUNTED_H
#define REFCOUNTED_H

namespace plv {

class RefCounted
{
protected:
    mutable int m_referenceCount;

public:
    /** Initializes reference count to 0 */
    RefCounted();


    /** increases reference count by one */
    inline void inc() const
    {
        ++m_referenceCount;
    }

    /** decreases reference count by one. Deletes this object if
      * reference count becomes 0
      */
    inline void dec() const
    {
        --m_referenceCount;
        if( m_referenceCount < 1 )
        {
            delete this;
        }
    }

protected:
    /** destructor is protected so RefCounted objects kan not be explicitely
      * destructed.
      */
    ~RefCounted();

};

}

#endif // REFCOUNTED_H
