#include "Pipeline.h"

#include <QDebug>

#include "PipelineElement.h"
#include "PipelineProcessor.h"
#include "PipelineProducer.h"

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
            if(PipelineProducer* prod = dynamic_cast<PipelineProducer*> (element.getPtr()))
            {
                prod->produce();
            }
            else
            {
                qDebug() << "unknown pipeline element";
            }

        }
        usleep(1000000/40);
    }
}
