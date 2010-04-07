#include "RefCounted.h"

using namespace plv;

RefCounted::RefCounted() :
    m_referenceCount( 0 )
{
}

RefCounted::~RefCounted()
{
}
