#include "DataConsumer.h"
#include "IInputPin.h"
#include "IOutputPin.h"
#include "Pipeline.h"

#include <algorithm>

using namespace plv;

DataConsumer::DataConsumer()
{
}

DataConsumer::~DataConsumer()
{
}

void DataConsumer::initInputPins()
{
    m_hasAsynchronousPin = false;
    m_hasSynchronousPin  = false;

    int synchronousCount = 0;

    for( InputPinMap::const_iterator itr = m_inputPins.begin();
         itr != m_inputPins.end();
         ++itr )
    {
        RefPtr<IInputPin> in = *itr;

        // only automatically check synchronous connections
        if( in->isConnected() )
        {
            if( in->isSynchronous() )
            {
                m_hasSynchronousPin = true;
                ++synchronousCount;
            }
            else
            {
                m_hasAsynchronousPin = true;
            }
        }
    }
    m_scoreboard.setPinCount(synchronousCount);
}

bool DataConsumer::isEndNode() const
{
    /** return true because if we were a DataProducer, this method
        is overridden in the DataProducer class */
    return true;
}

int DataConsumer::addInputPin( IInputPin* pin )
{
    assert( pin != 0 );

    int id = pin->getId();
    if( id == -1 )
    {
        id = getNextInputPinId();
        pin->setId(id);
        assert( m_inputPins.find(id) == m_inputPins.end() );
    }
    else
    {
        InputPinMap::iterator itr = m_inputPins.find(id);
        if( itr != m_inputPins.end() )
        {
            QString err = QString( tr( "Tried to add input pin %1 with id %2 "
                                       "which already exists. Ignoring."))
                    .arg( pin->getName() ).arg( pin->getId() );
            qWarning() << err;
            return -1;
        }
    }
    RefPtr<IInputPin> rpin( pin );
    m_inputPins.insert( id, rpin );
    emit inputPinAdded(pin);
    return id;
}

void DataConsumer::removeInputPin( int id )
{
    InputPinMap::iterator itr = m_inputPins.find(id);
    if( itr != m_inputPins.end() )
    {
        QString err = QString(tr( "Tried to remove an input pin with invalid id %1. "
                                  "Ignoring.").arg(id));
        qWarning() << err;
    }
    emit inputPinRemoved(itr.key());
    m_inputPins.erase(itr);
}

int DataConsumer::getNextInputPinId() const
{
    int id=0;

    InputPinMap::const_iterator itr = m_inputPins.begin();
    for( ;itr != m_inputPins.end(); ++itr, ++id)
    {
        int cid = itr.key();
        if( cid > id )
            return id;
    }
    return id;
}

IInputPin* DataConsumer::getInputPin( int id ) const
{
    InputPinMap::const_iterator itr = m_inputPins.find( id );
    if( itr != m_inputPins.end() )
    {
        return itr.value().getPtr();
    }
    qDebug() << tr("Could not find pin with id %1 in PipelineElement::getInputPin").arg(id);
    return 0;
}

const InputPinMap& DataConsumer::getInputPins() const
{
    // return a const reference
    return m_inputPins;
}

QSet<PipelineElement*> DataConsumer::getConnectedElementsToInputs() const
{
    QSet<PipelineElement*> elements;
    for( InputPinMap::const_iterator itr = m_inputPins.begin();
        itr != m_inputPins.end(); ++itr )
    {
        RefPtr<IInputPin> in = itr.value();
        if( in->isConnected() )
        {
            RefPtr<PinConnection> connection = in->getConnection();
            RefPtr<const IOutputPin> fromPin = connection->fromPin();
            PipelineElement* pinOwner = fromPin->getOwner();
            elements.insert( pinOwner );
        }
    }
    return elements;
}

bool DataConsumer::requiredInputPinsConnected() const
{
    for( InputPinMap::const_iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        RefPtr<IInputPin> in = itr.value();
        if( in->isRequired() )
            if( !in->isConnected() )
                return false;
    }
    return true;
}

bool DataConsumer::dataAvailableOnInputPins( unsigned int& nextSerial )
{
    // synchronous processor
    if( m_hasSynchronousPin )
    {
        std::vector<unsigned int> serials;
        for( InputPinMap::const_iterator itr = m_inputPins.begin();
             itr != m_inputPins.end();
             ++itr )
        {
            IInputPin* in = itr.value().getPtr();

            // only check synchronous connections
            if( in->isConnected() )
            {
                if( in->isSynchronous() )
                {
                    if( in->hasData() )
                    {
                        unsigned int serial;
                        bool isNull;
                        in->peekNext(serial, isNull);
                        serials.push_back( serial );
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }

        // check if all serials are the same (which should be the case)
        bool valid = true;
        for( unsigned int i=0; i<serials.size() && valid; ++i)
        {
            valid = (serials[0] == serials[i]);
        }

        // the model should guarantee that
        // this should never happen obviously
        if( !valid )
        {
            setError( PlvPipelineRuntimeError, "Input corrupted" );
            return false;
        }

        // save the serial
        nextSerial = serials[0];

        return true;
    }
    // asynchronous processor, only has asynchronous pins
    else if( m_hasAsynchronousPin )
    {
        std::vector<unsigned int> serials;
        for( InputPinMap::const_iterator itr = m_inputPins.begin();
             itr != m_inputPins.end();
             ++itr )
        {
            IInputPin* in = itr.value().getPtr();

            // only check asynchronous connections
            if( in->isConnected() ) //&& in->isAsynchronous() )
            {
                if( in->hasData() )
                {
                    unsigned int serial;
                    bool isNull;
                    in->peekNext(serial, isNull);
                    serials.push_back(serial);
                }
            }
        }

        if( serials.size() > 0 )
        {
            std::sort( serials.begin(), serials.end() );
            // return smallest serial
            nextSerial = serials[0];
            return true;
        }
    }
    return false;
}

QStringList DataConsumer::getInputPinNames() const
{
    QStringList names;
    for( InputPinMap::const_iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        names.push_back(itr.value()->getName());
    }
    return names;
}

int DataConsumer::inputPinsConnectionCount() const
{
    int connectionCount = 0;

    for( InputPinMap::const_iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        RefPtr<IInputPin> pin = itr.value();
        assert(pin.isNotNull());
        if( pin->isConnected() ) ++connectionCount;
    }

    return connectionCount;
}

int DataConsumer::maxInputQueueSize() const
{
    int maxQueueSize = 0;

    for( InputPinMap::const_iterator itr = m_inputPins.begin();
        itr!=m_inputPins.end(); ++itr )
    {
        int queueSize = 0;

        IInputPin* inputPin = itr.value();
        if( inputPin->isConnected() )
        {
            queueSize = inputPin->getConnection()->size();
        }

        if( queueSize > maxQueueSize ) maxQueueSize = queueSize;
    }
    return maxQueueSize;
}

bool DataConsumer::visit( QList< PipelineElement* >& ordering,
                          QSet< PipelineElement* >& visited )
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
    visited.insert( this );

    // visit all incoming connections
    if( !this->visitAllIncomingConnections( ordering, visited ) )
    {
        return false;
    }

    // go up in call stack
    visited.remove( this );
    ordering.append( this );
    return true;
}

bool DataConsumer::visitAllIncomingConnections( QList< PipelineElement* >& ordering,
                                                QSet< PipelineElement* >& visited )
{
    for( InputPinMap::const_iterator itr = m_inputPins.begin();
        itr!=m_inputPins.end(); ++itr )
    {
        IInputPin* inputPin = itr.value();
        if( inputPin->isConnected() )
        {
            PipelineElement* node = inputPin->getConnection()->fromPin()->getOwner();
            // directly quit if cycle detected
            if( !node->visit( ordering, visited ) )
                return false;
        }
    }
    return true;
}

void DataConsumer::preInput( bool& nullDetected )
{
    nullDetected = false;

    // call pre on input pins and look for null data items
    for( InputPinMap::iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        IInputPin* in = itr.value().getPtr();

        if( in->isConnected() && in->isSynchronous() )
        {
            unsigned int serial;
            bool isNull;
            in->peekNext(serial, isNull);
            if( isNull ) nullDetected = true;
        }
        in->pre();
    }
}

void DataConsumer::postInput()
{

}

void DataConsumer::newData(IInputPin* pin, unsigned int serial)
{
    assert(getPipeline() != 0);

    if( m_hasSynchronousPin )
    {
        // synchronous processor
        // use scoreboarding

        if( pin->isAsynchronous() )
        {
            // ignore asynchronous pins when we are
            // a synchronous processor
            return;
        }

        // scoreboard entry under correct serial
        // check if row complete for serial
        // fire ready signal for serial

//        qDebug() << QString("Called scoreboard.add on %1 with serial %2")
//                    .arg(this->getName())
//                    .arg(serial);


        assert( serial > getProcessingSerial() || serial == 0 );

        if( m_scoreboard.add( pin->getId(), serial) )
        {
            m_pipeline->pipelineDataConsumerReady(serial, this);
        }
    }
    else
    {
        // asynchronous processor
        // send ready signal
        m_pipeline->pipelineDataConsumerReady(serial, this);
    }
}

void DataConsumer::flushFirstOnSynchronousPins()
{
    for( InputPinMap::iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        IInputPin* in = itr.value().getPtr();

        if( in->isConnected() && in->isSynchronous() )
        {
            // just remove first data item
            in->removeFirst();
        }
    }
}

void DataConsumer::onInputConnectionSet(IInputPin* pin, PinConnection* connection)
{
    Q_UNUSED(pin);
    Q_UNUSED(connection);
}

void DataConsumer::onInputConnectionRemoved(IInputPin* pin, PinConnection* connection)
{
    Q_UNUSED(pin);
    Q_UNUSED(connection);
}

