#include "DataProducer.h"

#include "IOutputPin.h"
#include "IInputPin.h"
#include "PinConnection.h"

using namespace plv;

DataProducer::DataProducer()
{
}

DataProducer::~DataProducer()
{
}

void DataProducer::initOutputPins()
{
}

int DataProducer::addOutputPin( IOutputPin* pin )
{
    assert( pin != 0 );

    int id = pin->getId();
    if( id == -1 )
    {
        id = getNextOutputPinId();
        assert( m_outputPins.find(id) == m_outputPins.end() );
        pin->setId(id);
    }
    else
    {
        OutputPinMap::iterator itr = m_outputPins.find(id);
        if( itr != m_outputPins.end() )
        {
            QString err = QString( tr( "Tried to add output pin %1 with id %2 "
                                       "which already exists. Ignoring."))
                    .arg( pin->getName() ).arg( pin->getId() );
            qWarning() << err;
            return -1;
        }
    }
    RefPtr<IOutputPin> rpin( pin );
    m_outputPins.insert( id, rpin );
    emit outputPinAdded(pin);
    return id;
}

void DataProducer::removeOutputPin( int id )
{
    OutputPinMap::iterator itr = m_outputPins.find(id);
    if( itr != m_outputPins.end() )
    {
        QString err = QString( tr( "Tried to remove an output pin with invalid id %1. "
                                   "Ignoring.").arg(id) );
        qWarning() << err;
    }
    emit outputPinRemoved(itr.key());
    m_outputPins.erase(itr);
}

int DataProducer::getNextOutputPinId() const
{
    int id=0;

    OutputPinMap::const_iterator itr = m_outputPins.begin();
    for( ;itr != m_outputPins.end(); ++itr, ++id)
    {
        int cid = itr.key();
        if( cid > id )
            return id;
    }
    return id;
}

IOutputPin* DataProducer::getOutputPin( int id ) const
{
    OutputPinMap::const_iterator itr = m_outputPins.find( id );
    if( itr != m_outputPins.end() )
    {
        return itr.value().getPtr();
    }
    qDebug() << tr("Could not find pin with name %1 in DataProducer::getOutputPin").arg(id);
    return 0;
}

const OutputPinMap& DataProducer::getOutputPins() const
{
    // return a const reference
    return m_outputPins;
}

QSet<DataProducer*> DataProducer::getConnectedElementsToOutputs() const
{
    QSet<DataProducer*> elements;
    for( OutputPinMap::const_iterator itr = m_outputPins.begin();
        itr != m_outputPins.end(); ++itr )
    {
        RefPtr<IOutputPin> out = itr.value();
        if( out->isConnected() )
        {
            std::list< RefPtr<PinConnection> > connections = out->getConnections();
            for( std::list< RefPtr<PinConnection> >::const_iterator connItr = connections.begin();
                 connItr != connections.end(); ++connItr )
            {
                RefPtr<PinConnection> connection = *connItr;
                RefPtr<const IInputPin> toPin = connection->toPin();
                DataProducer* pinOwner = static_cast<DataProducer*>(toPin->getOwner());
                elements.insert( pinOwner );
            }
        }
    }
    return elements;
}

bool DataProducer::isEndNode() const
{
    for( OutputPinMap::const_iterator itr = m_outputPins.begin();
         itr != m_outputPins.end(); ++itr )
    {
        RefPtr<IOutputPin> out = itr.value();
        if( out->isConnected() )
            return false;
    }
    return true;
}

bool DataProducer::visit( QList<PipelineElement*>& ordering,
                          QSet<PipelineElement*>& visited )
{
    // check if this node is already in partial ordering
    if( ordering.contains( this ))
        return true;

    // check for cycles
    if( visited.contains( this ))
    {
        // cycle detected
        ordering.append( this );
        return false;
    }
    ordering.append( this );
    return true;
}

QStringList DataProducer::getOutputPinNames() const
{
    QStringList names;
    for( OutputPinMap::const_iterator itr = m_outputPins.begin();
         itr != m_outputPins.end(); ++itr )
    {
        names.push_back(itr.value()->getName());
    }
    return names;
}

int DataProducer::outputPinsConnectionCount() const
{
    int connectionCount = 0;

    for( OutputPinMap::const_iterator itr = m_outputPins.begin();
         itr != m_outputPins.end(); ++itr )
    {
        RefPtr<IOutputPin> pin = itr.value();
        assert(pin.isNotNull());
        connectionCount += pin->connectionCount();
    }

    return connectionCount;
}

int DataProducer::maxOutputQueueSize() const
{
    int maxQueueSize = 0;

    for( OutputPinMap::const_iterator itr = m_outputPins.begin();
        itr!=m_outputPins.end(); ++itr )
    {
        IOutputPin* outputPin = itr.value();
        int queueSize = outputPin->maxDataOnConnection();
        if( queueSize > maxQueueSize )
            maxQueueSize = queueSize;
    }
    return maxQueueSize;
}

void DataProducer::preOutput()
{
    for( OutputPinMap::iterator itr = m_outputPins.begin();
         itr != m_outputPins.end(); ++itr )
    {
        IOutputPin* out = itr.value().getPtr();
        out->pre();
    }
}

void DataProducer::postOutput()
{
    for( OutputPinMap::iterator itr = m_outputPins.begin();
         itr != m_outputPins.end(); ++itr )
    {
        IOutputPin* out = itr.value().getPtr();
        out->post();
    }
}

void DataProducer::onOutputConnectionAdded(IOutputPin *pin, PinConnection *connection)
{
    Q_UNUSED(pin);
    Q_UNUSED(connection);
}

void DataProducer::onOutputConnectionRemoved(IOutputPin* pin, PinConnection* connection)
{
    Q_UNUSED(pin);
    Q_UNUSED(connection);
}
