#include "Pin.h"

using namespace plv;


void OutputPin::addConnection(PinConnection *connection)
{
    assert(connection != 0);

    this->m_connections.push_back(connection);
}

bool OutputPin::isConnected() const
{
    return !m_connections.empty();
}





void InputPin::setConnection(PinConnection *connection)
{
    assert(!this->isConnected());
    assert(connection != 0);

    this->m_connection = connection;
}

bool InputPin::isConnected() const
{
    return this->m_connection.isNotNull();
}
