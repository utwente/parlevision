#include "Pipeline.h"

#include <QDebug>

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

std::list< RefPtr<PipelineElement> > Pipeline::getChildren()
{
    //TODO implement this properly when I have access to documentation for std::map and std::list
    std::list< RefPtr<PipelineElement> > elements;

    for( PipelineElementMap::iterator itr = m_children.begin()
        ; itr != m_children.end(); ++itr )
    {
        elements.push_back(itr->second);
    }

    return elements;
}

void Pipeline::connectPins(OutputPin* outputPin, InputPin* inputPin)
{
    RefPtr<PinConnection> connection = new PinConnection(outputPin, inputPin);
    m_connections.push_back(connection);
}

void Pipeline::init()
{

}

void Pipeline::start()
{
    QThread::start();
}

void Pipeline::stop()
{

}

void Pipeline::run()
{
    while(true)
    {
        for( PipelineElementMap::iterator itr = m_children.begin()
            ; itr != m_children.end(); ++itr )
        {
            RefPtr<PipelineElement> element = itr->second;

            //TODO make this not so ugly
            if(RefPtr<PipelineProducer> prod = ref_ptr_dynamic_cast<PipelineProducer>(element))
            {
                prod->produce();
            }
            else if(RefPtr<PipelineProcessor> proc = ref_ptr_dynamic_cast<PipelineProcessor>(element))
            {
                proc->process();
            }
            else
            {
                qDebug() << "unknown pipeline element";
            }

        }
//        usleep(100);
        usleep(1000000/40);
        //usleep( 1000000 );
    }
}
