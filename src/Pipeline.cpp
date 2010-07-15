#include "Pipeline.h"

#include <QDebug>
#include <list>

#include "PipelineElement.h"
#include "PipelineProcessor.h"
#include "PipelineProducer.h"
#include "Pin.h"
#include "PinConnection.h"

using namespace plv;

Pipeline::Pipeline() :
        m_idCounter( 1 ), m_stopRequested( false ), m_running( false )
{
}

Pipeline::~Pipeline()
{
    assert( !m_running );
}

int Pipeline::add( PipelineElement* child )
{
    RefPtr<PipelineElement> element = child;
    element->setPipeline(this);
    m_children.insert( std::make_pair( m_idCounter, element) );
    emit(elementAdded(element));
    return m_idCounter++;
}

void Pipeline::remove( PipelineElement* child )
{
    for( PipelineElementMap::iterator itr = m_children.begin()
        ; itr != m_children.end(); ++itr )
    {
        if( child == itr->second.getPtr() )
        {
            // preserve the element so we can send it over the signal later
            RefPtr<PipelineElement> element = itr->second;
            removeConnectionsForElement( element.getPtr() );
            m_children.erase(itr);
            emit( elementRemoved(element) );
        }
    }
}

PipelineElement* Pipeline::getElement( int id )
{
    PipelineElementMap::iterator itr = m_children.find( id );
    if( itr != m_children.end() )
    {
        // preserve the element so we can send it over the signal later
        return itr->second.getPtr();
    }
    return 0;
}

void Pipeline::remove( int id )
{
    PipelineElementMap::iterator itr = m_children.find( id );
    if( itr != m_children.end() )
    {
        // preserve the element so we can send it over the signal later
        RefPtr<PipelineElement> element = itr->second;
        m_children.erase(itr);
        emit(elementRemoved(element));
    }
}

void Pipeline::removeAllElements()
{
    for( PipelineElementMap::iterator itr = m_children.begin(); itr!=m_children.end(); ++itr )
    {
        // preserve the element so we can send it over the signal later
        RefPtr<PipelineElement> element = itr->second;
        elementRemoved( element );
        m_children.erase( itr );
    }
}

void Pipeline::removeConnectionsForElement( PipelineElement* element )
{
    RefPtr<PipelineElement> ple( element );

    std::list< RefPtr<IInputPin> >* inputPins = ple->getInputPins();
    std::list< RefPtr<IOutputPin> >* outputPins = ple->getOutputPins();

    for( std::list< RefPtr<IInputPin> >::iterator itr = inputPins->begin()
        ; itr!=inputPins->end(); ++itr)
    {
        RefPtr<IInputPin> ipp = *itr;
        if( ipp->isConnected() )
        {
            ipp->removeConnection();
            RefPtr<PinConnection> connection = ipp->getConnection();
            connection->disconnect();
            removeConnection( connection );
        }
    }

    for( std::list< RefPtr<IOutputPin> >::iterator itr = outputPins->begin()
        ; itr!=outputPins->end(); ++itr)
    {
        RefPtr<IOutputPin> opp = *itr;

        if( opp->isConnected() )
        {
            const std::list< RefPtr<PinConnection> >& connections = opp->getConnections();
            for( std::list< RefPtr<PinConnection> >::const_iterator itr = connections.begin();
                 itr!= connections.end(); ++itr )
            {
                removeConnection( *itr );
            }
        }
    }


    // TODO better to use automatic removal than this
    delete inputPins;
    delete outputPins;
}

std::list< RefPtr<PipelineElement> > Pipeline::getChildren() const
{
    //TODO implement this properly when I have access to documentation for std::map and std::list
    std::list< RefPtr<PipelineElement> > elements;

    for( PipelineElementMap::const_iterator itr = m_children.begin()
        ; itr != m_children.end(); ++itr )
    {
        elements.push_back(itr->second);
    }

    return elements;
}

const std::list< RefPtr<PinConnection> >& Pipeline::getConnections() const
{
    return m_connections;
}

void Pipeline::connectPins( IOutputPin* outputPin, IInputPin* inputPin)
{
    RefPtr<PinConnection> connection = new PinConnection(outputPin, inputPin);
    m_connections.push_back(connection);
    emit( connectionAdded(connection) );
}

void Pipeline::removeAllConnections()
{
    assert( !m_running );

    // first signal impending removal of connections
    for( PipelineConnectionsList::iterator itr = m_connections.begin();
            itr != m_connections.end(); ++itr )
    {
        RefPtr<PinConnection> connection = *itr;
        connectionRemoved( connection );
    }

    // now remove all connections
    m_connections.clear();
}

void Pipeline::removeConnection( PinConnection* connection )
{
    RefPtr<PinConnection> con1( connection );

    for( PipelineConnectionsList::iterator itr = m_connections.begin();
            itr != m_connections.end(); ++itr )
    {
        RefPtr<PinConnection> con2 = *itr;
        if( con1.getPtr() == con2.getPtr() )
        {
            m_connections.erase( itr );
        }
    }
}

// TODO maybe refactor so we can report the processor which fails to initialize
bool Pipeline::init()
{
    for( PipelineElementMap::iterator itr = m_children.begin()
        ; itr != m_children.end(); ++itr )
    {
        RefPtr<PipelineElement> element = itr->second;
        bool state = element->init();
        if( !state )
        {
            return false;
        }
    }
    return true;
}

void Pipeline::clear()
{
    assert( !m_running );

    // we need to explicitly remove the connections
    // and the children because they hold a ref pointer
    // to Pipeline and will prevent us from deleting ourselves
    removeAllConnections();
    removeAllElements();
}

void Pipeline::start()
{
    // TODO make thread safe
    // and ensure that pipeline cannot be started twice
    m_stopRequested = false;
    QThread::start();
}

void Pipeline::stop()
{
    m_stopRequested = true;
    //TODO more elegant solution here with QTimer
    while( m_running )
    {
        usleep(100);
    }
}

void Pipeline::run()
{
    m_running = true;
    while(!m_stopRequested)
    {
        for( PipelineElementMap::iterator itr = m_children.begin()
            ; itr != m_children.end(); ++itr )
        {
            RefPtr<PipelineElement> element = itr->second;
            try
            {
                element->__process();
            }
            catch( PipelineException& pe )
            {
                qDebug() << "Uncaught exception in PipelineElement::process()"
                         << "of type PipelineException with message: " << pe.what();
            }
            catch( IllegalAccessException& iae )
            {
                qDebug() << "Uncaught exception in PipelineElement::process()"
                         << "of type IllegalAccessException with message: " << iae.what();
            }
            catch( ... )
            {
                qDebug() << "Uncaught exception in PipelineElement::process()"
                         << "of unknown type.";
            }
        }
//        usleep(100);
        usleep(1000000/40);
    }
    m_running = false;
}
