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
        m_idCounter( 0 )
{
}

Pipeline::~Pipeline()
{
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
            m_children.erase(itr);
            emit(elementRemoved(element));
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
    emit(connectionAdded(connection));
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
}

void Pipeline::run()
{
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
//        usleep( 1000000 );
    }
}
