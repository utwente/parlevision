#include "PipelineElement.h"

#include <QString>
#include <QDebug>
#include <QMetaObject>
#include <QMetaProperty>

#include "Pipeline.h"
#include "RefCounted.h"
#include "Pin.h"

using namespace plv;

std::list<QString> PipelineElement::s_types;
std::map<QString,QString> PipelineElement::s_names;

PipelineElement::PipelineElement() : m_id( -1 )
{
}

PipelineElement::~PipelineElement()
{
    if( m_parent != 0 )
    {
        m_parent->removeElement( this );
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

void PipelineElement::setPipeline( Pipeline* parent )
{
    if( m_parent.getPtr() == parent )
    {
        // no effect
        return;
    }

    if( m_parent.getPtr() != 0 )
    {
        // switching pipelines.
        // remove ourself from the old pipeline first
        m_parent->removeElement(this);
    }

    m_parent = parent;
}

void PipelineElement::addInputPin( IInputPin* pin ) throw (IllegalArgumentException)
{
    InputPinMap::iterator itr = m_inputPins.find( pin->getName() );
    if( itr != m_inputPins.end() )
    {
        QString err = "Tried to add input pin with name which already exists";
        err += pin->getName();
        throw IllegalArgumentException( err.toStdString() );
    }
    RefPtr<IInputPin> rpin( pin );
    m_inputPins.insert( std::make_pair( pin->getName(), rpin ));
}

void PipelineElement::addOutputPin( IOutputPin* pin ) throw (IllegalArgumentException)
{
    OutputPinMap::iterator itr = m_outputPins.find( pin->getName() );
    if( itr != m_outputPins.end() )
    {
        QString err = "Tried to add output pin with name which already exists";
        err += pin->getName();
        throw IllegalArgumentException( err.toStdString() );
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
    qDebug() << "could not find pin named " << name;
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

QString PipelineElement::getName() const
{
    return PipelineElement::nameForType(this->metaObject()->className());
}

void PipelineElement::getConfigurablePropertyNames(std::list<QString>& list)
{
    const QMetaObject* metaObject = this->metaObject();
    for(int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i)
    {
        list.push_back(QString::fromLatin1(metaObject->property(i).name()));
    }
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
    std::list<QString>* names = new std::list<QString>();
    for( InputPinMap::const_iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        QString name = itr->first;
        qDebug() << "listing " << name;
        names->push_back(name);
    }
    return names;
}

const PipelineElement::InputPinMap& PipelineElement::getInputPins() const
{
//    std::list< RefPtr<IInputPin> >* pins = new std::list< RefPtr<IInputPin> >();

//    for( InputPinMap::iterator itr = m_inputPins.begin();
//         itr != m_inputPins.end(); ++itr )
//    {
//        RefPtr<IInputPin> pin = itr->second;
//        assert(pin.isNotNull());
//        pins->push_back(pin);
//    }
//    return pins;
    return m_inputPins;
}

std::list<QString> PipelineElement::getOutputPinNames() const
{
    std::list<QString> names;
    for( OutputPinMap::const_iterator itr = m_outputPins.begin();
         itr != m_outputPins.end(); ++itr )
    {
        names.push_back(itr->first);
    }
    return names;
}

const PipelineElement::OutputPinMap& PipelineElement::getOutputPins() const
{
//    std::list< RefPtr<IOutputPin> >* pins = new std::list< RefPtr<IOutputPin> >();

//    for( OutputPinMap::iterator itr = m_outputPins.begin();
//         itr != m_outputPins.end(); ++itr )
//    {
//        RefPtr<IOutputPin> pin = itr->second;
//        assert(pin.isNotNull());
//        pins->push_back(pin);
//    }
//    return pins;
    return m_outputPins;
}

int PipelineElement::outputPinsConnectionCount() const
{
    int connectionCount = 0;

    for( OutputPinMap::const_iterator itr = m_outputPins.begin();
         itr != m_outputPins.end(); ++itr )
    {
        RefPtr<IOutputPin> pin = itr->second;
        assert(pin.isNotNull());
        connectionCount += pin->connectionCount();
    }

    return connectionCount;
}

int PipelineElement::inputPinsConnectionCount() const
{
    int connectionCount = 0;

    for( InputPinMap::const_iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        RefPtr<IInputPin> pin = itr->second;
        assert(pin.isNotNull());
        if( pin->isConnected() ) ++connectionCount;
    }

    return connectionCount;
}

int PipelineElement::pinsConnectionCount() const
{
    return inputPinsConnectionCount() + outputPinsConnectionCount();
}

std::list<QString> PipelineElement::types()
{
    return PipelineElement::s_types;
}

int PipelineElement::registerType(QString typeName, QString humanName)
{
    qDebug() << "Registering PipelineElement " << typeName
                    << " as " << "'" << humanName << "'";

    PipelineElement::s_types.push_back( typeName );
    PipelineElement::s_names[typeName] = humanName;
    //return qRegisterMetaType<E>(typeName);
    return 0;
}

QString PipelineElement::nameForType(QString typeName)
{
    return PipelineElement::s_names[typeName];
}
