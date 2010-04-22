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
    if(m_parent != 0)
    {
        m_parent->remove(this);
    }
    m_parent = parent;
}

void PipelineElement::addInputPin( Pin* pin )
{
    InputPinMap::iterator itr = m_inputPins.find( pin->getName() );
    if( itr != m_inputPins.end() )
    {
        qDebug() << "Error: tried to insert pin with duplicate name"
                << pin->getName() << ".";
        return;
    }

    m_inputPins.insert( std::make_pair( pin->getName(), RefPtr<Pin>(pin) ));
}

void PipelineElement::addOutputPin( Pin* pin )
{
    InputPinMap::iterator itr = m_outputPins.find( pin->getName() );
    if( itr != m_outputPins.end() )
    {
        qDebug() << "Error: tried to insert pin with duplicate name"
                << pin->getName() << ".";
        return;
    }
    m_outputPins.insert( std::make_pair( pin->getName(), RefPtr<Pin>(pin) ));
}

const Pin* PipelineElement::getInputPin( const QString& name ) const
{
    InputPinMap::const_iterator itr = m_inputPins.find( name );
    if( itr != m_inputPins.end() )
    {
        return itr->second.getPtr();
    }
    return 0;
}

const Pin* PipelineElement::getOutputPin( const QString& name ) const
{
    OutputPinMap::const_iterator itr = m_outputPins.find( name );
    if( itr != m_inputPins.end() )
    {
        return itr->second.getPtr();
    }
    return 0;
}
