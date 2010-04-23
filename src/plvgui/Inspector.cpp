#include "Inspector.h"
#include "Pin.h"

using namespace plvgui;

Inspector::~Inspector()
{
    disconnect();
}

void Inspector::setPin(const plv::Pin* p)
{
    disconnect();

    connect( p, SIGNAL(newData(Data*)),
             this, SLOT(newData(Data*)) );
}
