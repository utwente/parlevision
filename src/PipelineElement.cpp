#include "PipelineElement.h"

#include <QString>
#include <QDebug>

#include "Pipeline.h"
#include "RefCounted.h"
#include "Pin.h"

using namespace plv;

std::list<QString> PipelineElement::s_types;

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

PipelineElement::PipelineElement(const PipelineElement &other)
    : QObject(),
        RefCounted(other),
        m_inputPins(other.m_inputPins),
        m_outputPins(other.m_outputPins),
        m_parent(other.m_parent)
{
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

void PipelineElement::addInputPin( IInputPin* pin )
{
    InputPinMap::iterator itr = m_inputPins.find( pin->getName() );
    if( itr != m_inputPins.end() )
    {
        qDebug() << "Error: tried to insert pin with duplicate name"
                << pin->getName() << ".";
        return;
    }
    RefPtr<IInputPin> rpin( pin );
    m_inputPins.insert( std::make_pair( pin->getName(), rpin ));
}

void PipelineElement::addOutputPin( IOutputPin* pin )
{
    OutputPinMap::iterator itr = m_outputPins.find( pin->getName() );
    if( itr != m_outputPins.end() )
    {
        qDebug() << "Error: tried to insert pin with duplicate name"
                << pin->getName() << ".";
        return;
    }
    RefPtr<IOutputPin> rpin( pin );
    m_outputPins.insert( std::make_pair( pin->getName(), rpin ));
}

IInputPin* PipelineElement::getInputPin( const QString& name ) const
{
    InputPinMap::const_iterator itr = m_inputPins.find( name );
    if( itr != m_inputPins.end() )
    {
        return itr->second.getPtr();
    }
    return 0;
}

IOutputPin* PipelineElement::getOutputPin( const QString& name ) const
{
    OutputPinMap::const_iterator itr = m_outputPins.find( name );
    if( itr != m_outputPins.end() )
    {
        return itr->second.getPtr();
    }
    return 0;
}

bool PipelineElement::isReadyForProcessing() const
{
    qDebug( "Error, isReadyForProcessing not implemented by PipelineElement" );
    return false;
}

void PipelineElement::__process()
{
    for( InputPinMap::iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        RefPtr<IInputPin> in = itr->second;
        in->scope();
    }

    try
    {
        this->process();
    }
    catch( ... )
    {
        // TODO do in method
        for( InputPinMap::iterator itr = m_inputPins.begin();
             itr != m_inputPins.end(); ++itr )
        {
            RefPtr<IInputPin> in = itr->second;
            in->unscope();
        }
        throw;
    }

    for( InputPinMap::iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        RefPtr<IInputPin> in = itr->second;
        in->unscope();
    }
}

std::list<QString>* PipelineElement::getInputPinNames() const
{
    std::list<QString>* names = new std::list<QString>(m_inputPins.size());
    for( InputPinMap::const_iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        names->push_back(itr->first);
    }
    return names;
}

std::list<QString>* PipelineElement::getOutputPinNames() const
{
    std::list<QString>* names = new std::list<QString>(m_outputPins.size());
    for( OutputPinMap::const_iterator itr = m_outputPins.begin();
         itr != m_outputPins.end(); ++itr )
    {
        names->push_back(itr->first);
    }
    return names;
}




std::list<QString> PipelineElement::types()
{
    return PipelineElement::s_types;
}

int PipelineElement::registerType(QString typeName)
{
    qDebug() << "Registering PipelineElement" << typeName;
    PipelineElement::s_types.push_back( typeName );
    //return qRegisterMetaType<E>(typeName);
    return 0;
}
