#include "PipelineElementFactory.h"

#include <QDebug>

using namespace plv;

PipelineElementFactory* PipelineElementFactory::m_instance = 0;

PipelineElementFactory::PipelineElementFactory()
{
}

PipelineElementFactory::~PipelineElementFactory()
{
   // clear instance vars
    m_nameIdMapping.clear();

    for( int i=0; i < m_elementConstructors.size(); ++i )
    {
        PipelineElementConstructor* plec = m_elementConstructors.at(i);
        if( plec != 0 )
            delete plec;
    }
}

int PipelineElementFactory::__registerElement( PipelineElementConstructor* constructor )
{
    QString name = constructor->getName();
    if( m_nameIdMapping.contains( name ))
    {
        QString msg = QString( "Element with name \"%1\" already registered" )
                      .arg(name);
        qWarning() << msg;
        return false;
    }
    int id = m_elementConstructors.size();
    m_nameIdMapping.insert( name, id );
    m_elementConstructors.push_back( constructor );
    return id;
}

void PipelineElementFactory::__unregisterElement( const QString& name )
{
    if( !m_nameIdMapping.contains( name ) )
        return;

    int id = m_nameIdMapping[name];
    PipelineElementConstructor* plec = m_elementConstructors.at(id);
    m_elementConstructors.replace(id, 0);
    delete plec;
}

int PipelineElementFactory::__isElementRegistered( const QString& name )
{
    if( !m_nameIdMapping.contains( name ) )
        return -1;
    return m_nameIdMapping[name];
}

PipelineElement* PipelineElementFactory::__construct( const QString& name ) const
{
    if( !m_nameIdMapping.contains( name ) )
    {
        QString msg = QString( "Element with name \"%1\" is not registered" )
                      .arg(name);
        qWarning() << msg;
        return 0;
    }
    int id = m_nameIdMapping[name];
    PipelineElementConstructor* plec = m_elementConstructors[id];
    return plec->construct();
}

PipelineElement* PipelineElementFactory::__construct( int id ) const
{
    if( id < 0 || m_elementConstructors.size() < id )
    {
        QString msg = QString( "Invalid PipelineElement id \"%1\"." )
                      .arg(id);
        qWarning() << msg;
        return 0;
    }

    PipelineElementConstructor* plec = m_elementConstructors.at(id);
    if( plec == 0 )
    {
        QString msg = QString( "Element with id \"%1\" is not registered" )
                      .arg(id);
        qWarning() << msg;
        return 0;
    }
    return plec->construct();
}

