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
    child->setPipeline(this);
    m_children.insert( std::make_pair( m_idCounter, RefPtr<PipelineElement>(child)) );
    return m_idCounter++;
}

void Pipeline::remove( PipelineElement* child )
{
    for( PipelineElementMap::iterator itr = m_children.begin()
        ; itr != m_children.end(); ++itr )
    {
        if( child == itr->second.getPtr() )
        {
            m_children.erase(itr);
        }
    }
}

void Pipeline::remove( int id )
{
    PipelineElementMap::iterator itr = m_children.find( id );
    if( itr != m_children.end() )
    {
        m_children.erase( itr );
    }
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
    }
}
