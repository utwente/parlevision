#ifndef PINCONNECTION_H
#define PINCONNECTION_H

#include "RefCounted.h"

namespace plv {

class PinConnection : public RefCounted
{
public:
    PinConnection();
    ~PinConnection();
};

}

#endif // PINCONNECTION_H
