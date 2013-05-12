#include "DynamicInputPin.h"
#include "DataConsumer.h"

plv::DynamicInputPin* plv::createDynamicInputPin( const QString& name, plv::DataConsumer* owner,
                              plv::IInputPin::Required required,
                              plv::IInputPin::Synchronized synchronized, int typeId)
throw (plv::IllegalArgumentException)
{
    // if add fails pin is automatically deleted and exception is thrown
    plv::DynamicInputPin* pin = new plv::DynamicInputPin( name, owner, required, synchronized, typeId );
    owner->addInputPin( pin );
    return pin;
}
