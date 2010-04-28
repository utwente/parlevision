#include "PipelineElement.h"

#include <QString>
#include <QDebug>

#include "Pipeline.h"
#include "RefCounted.h"
#include "Pin.h"

using namespace plv;

PipelineElement::PipelineElement()
{
}

PipelineElement::~PipelineElement()
{
    if(m_parent != 0)
    {
        m_parent->remove( this );
    }
}

void PipelineElement::setPipeline(Pipeline* parent)
{
    if(m_parent.getPtr() == parent)
    {
        // no effect
        return;
    }

    if(m_parent.getPtr() != 0)
    {
        // switching pipelines.
        // remove ourself from the old pipeline first
        m_parent->remove(this);
    }

    m_parent = parent;
}

void PipelineElement::addInputPin( InputPin* pin )
{
    InputPinMap::iterator itr = m_InputPins.find( pin->getName() );
    if( itr != m_InputPins.end() )
    {
        qDebug() << "Error: tried to insert pin with duplicate name"
                << pin->getName() << ".";
        return;
    }

    m_InputPins.insert( std::make_pair( pin->getName(), RefPtr<InputPin>(pin) ));
}

void PipelineElement::addOutputPin( OutputPin* pin )
{
    OutputPinMap::iterator itr = m_outputPins.find( pin->getName() );
    if( itr != m_outputPins.end() )
    {
        qDebug() << "Error: tried to insert pin with duplicate name"
                << pin->getName() << ".";
        return;
    }
    m_outputPins.insert( std::make_pair( pin->getName(), RefPtr<OutputPin>(pin) ));
}

InputPin* PipelineElement::getInputPin( const QString& name ) const
{
    InputPinMap::const_iterator itr = m_InputPins.find( name );
    if( itr != m_InputPins.end() )
    {
        return itr->second.getPtr();
    }
    return 0;
}

OutputPin* PipelineElement::getOutputPin( const QString& name ) const
{
    OutputPinMap::const_iterator itr = m_outputPins.find( name );
    if( itr != m_outputPins.end() )
    {
        return itr->second.getPtr();
    }
    return 0;
}
