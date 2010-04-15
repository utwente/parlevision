#include "PipelineElement.h"

#include <QString>

#include "Pipeline.h"
#include "RefCounted.h"
#include "Pin.h"

using namespace plv;

PipelineElement::PipelineElement( Pipeline* parent )
{
    m_parent = parent;
    m_parent->add( this );
}

PipelineElement::~PipelineElement()
{
    m_parent->remove( this );
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
