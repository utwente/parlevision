#include "PipelineElementFactory.h"

#include <QDebug>
#include <QStringList>

using namespace plv;

QList< PipelineElementConstructor* > PipelineElementFactory::m_elementConstructors;
QHash<QString, int> PipelineElementFactory::m_nameIdMapping;

//PipelineElementFactory* PipelineElementFactory::m_instance = 0;

//PipelineElementFactory::PipelineElementFactory()
//{
//}

//PipelineElementFactory::~PipelineElementFactory()
//{
//   // clear instance vars
//    m_nameIdMapping.clear();

//    for( int i=0; i < m_elementConstructors.size(); ++i )
//    {
//        PipelineElementConstructor* plec = m_elementConstructors.at(i);
//        if( plec != 0 )
//            delete plec;
//    }
//}

int PipelineElementFactory::registerElement( PipelineElementConstructor* constructor )
{
    QString name = constructor->getClassName();
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

void PipelineElementFactory::unregisterElement( const QString& name )
{
    if( !m_nameIdMapping.contains( name ) )
        return;

    int id = m_nameIdMapping[name];
    PipelineElementConstructor* plec = m_elementConstructors.at(id);
    m_elementConstructors.replace(id, 0);
    delete plec;
}

int PipelineElementFactory::elementId( const QString& name )
{
    if( !m_nameIdMapping.contains( name ) )
        return -1;
    int id = m_nameIdMapping[name];
    return m_elementConstructors.at(id) != 0 ? id : -1;
}

PipelineElement* PipelineElementFactory::construct( const QString& name )
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

PipelineElement* PipelineElementFactory::construct( int id )
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

QStringList PipelineElementFactory::types()
{
    return QStringList( m_nameIdMapping.keys() );
}

