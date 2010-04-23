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


