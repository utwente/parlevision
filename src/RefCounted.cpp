#include "RefCounted.h"
#include <QDebug>

using namespace plv;

RefCounted::RefCounted() :
    m_referenceCount( 0 )
 {
 }

RefCounted::~RefCounted()
{
    if(m_referenceCount > 0)
    {
        qDebug() << "WARNING: "
                    << "delete called on RefCounted object still in use "
                    << "(reference count = "<<m_referenceCount<<")." << endl
                    << "You should let the reference counter handle deletion.";
    }
}

/** increases reference count by one */
void RefCounted::inc() const
{
    ++m_referenceCount;
}

/** decreases reference count by one. Deletes this object if
  * reference count becomes 0
  */
void RefCounted::dec() const
{
    --m_referenceCount;
    if( m_referenceCount < 1 )
    {
        delete this;
    }
}
