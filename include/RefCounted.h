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
    void inc() const;

    /** decreases reference count by one. Deletes this object if
      * reference count becomes 0
      */
    void dec() const;

protected:
    /** destructor is protected so RefCounted objects kan not be explicitely
      * destructed.
      */
    virtual ~RefCounted();

};

}

#endif // REFCOUNTED_H
