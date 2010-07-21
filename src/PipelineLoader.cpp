#include "PipelineLoader.h"
#include "Pipeline.h"
#include "PipelineProcessor.h"
#include "XmlMapper.h"

#include <QStringList>
#include <QFile>
#include <QMetaProperty>
#include <QStringBuilder>
#include <QDateTime>
#include <QUrl>

using namespace plv;

PipelineLoader::PipelineLoader()
{

}

PipelineLoader::~PipelineLoader()
{

}

RefPtr<Pipeline> PipelineLoader::parsePipeline( const QString& filename )
        throw(std::runtime_error) /*TODO checked exceptions*/
{
    if( !QFile::exists( filename ) )
    {
        throw std::runtime_error (
            QString( "Failed to open file " + filename + ". File does not exist.").toStdString() );
    }

    QFile file( filename );
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw std::runtime_error (
            QString( "Failed to open file " + filename).toStdString() );
    }
    //QXmlStreamReader xmlReader( &file );
    QDomDocument doc;
    doc.setContent( &file );
    return parsePipeline( &doc );
}

RefPtr<Pipeline> PipelineLoader::parsePipeline( QDomDocument* document )
    throw(std::runtime_error) /*TODO checked exceptions*/
{
    RefPtr<Pipeline> pipeline = new Pipeline();

    qDebug() << document->elementsByTagName("pipeline").count() << " pipeline, "
             << document->elementsByTagName("element").count() << " elements and "
             << document->elementsByTagName("connection").count() << " connections.";

    QDomNodeList elementsList = document->elementsByTagName( "element" );
    parseElements( &elementsList, pipeline.getPtr() );

    QDomNodeList connectionsList = document->elementsByTagName( "connection" );
    parseConnections( &connectionsList, pipeline.getPtr() );

    return pipeline;
}

void PipelineLoader::parseElements( QDomNodeList* list, Pipeline* pipeline )
    throw(std::runtime_error) /*TODO checked exceptions*/
{
    for( unsigned int i=0; i < list->length(); ++i )
    {
        QDomNode elementNode = list->item( i );
        QDomNamedNodeMap attributes = elementNode.attributes();

        QDomNode idNode = attributes.namedItem( "id" );
        QString nodeValue = idNode.nodeValue();
        int id = nodeValue.toInt();

        QDomNode nameNode = attributes.namedItem( "name" );
        QString name = nameNode.nodeValue();

        qDebug() << "Creating element with name: " << name << " and id: " << id;

        // Use Qt metatype system to locate this processor type
        int typeId = QMetaType::type( name.toAscii() );
        if( typeId == 0 )
        {
            QString msg = "XML document contains unknown processor type " % name;
            throw std::runtime_error ( msg.toStdString() );
        }
        // and instantiate it
        PipelineElement* ple = static_cast<PipelineElement*>( QMetaType::construct( typeId ) );

        // set the id
        ple->setId( id );

        // check if it is valid
        if( !pipeline->canAddElement( ple ) )
        {
            QString msg = "XML parsing failed. PipelineElement with name " % name % " has invalid or used id " + id;
            throw std::runtime_error( msg.toStdString() );
        }
        pipeline->addElement( ple );

        // now parse the properties
        QDomElement propertiesNode = elementNode.firstChildElement( "properties" );
        if( propertiesNode.hasChildNodes() )
        {
            QDomNodeList children = propertiesNode.childNodes();
            for( unsigned int i=0; i < children.length(); ++i )
            {
                QDomNode child = children.item( i );
                QDomElement element = child.toElement();

                if( element.isNull() )
                {
                    //error
                }

                QString propertyName = element.nodeName();
                QString propertyValue = element.text();

                // convert the data to a known QVariant datatype
                QVariant data = convertData( propertyType(ple, propertyName), propertyValue );

                qDebug() << "Found property with name: " << propertyName << " and value: " << data;

                setProperty( ple, propertyName, data );
            }
        }
    }
}

void PipelineLoader::parseConnections( QDomNodeList* list, Pipeline* pipeline )
    throw(std::runtime_error) /*TODO checked exceptions*/
{
    for( unsigned int i=0; i < list->length(); ++i )
    {
        QDomNode connectionNode = list->item( i );

        QDomElement sinkNode = connectionNode.firstChildElement( "sink" );
        if( sinkNode.isNull() )
        {
            // error
        }
        QDomElement sinkProcIdNode = sinkNode.firstChildElement( "processorId" );
        if( sinkProcIdNode.isNull() )
        {
            // error
        }

        QDomElement sinkPinNameNode = sinkNode.firstChildElement( "pinName" );
        if( sinkPinNameNode.isNull() )
        {
            // error
        }

        int sinkProcId = sinkProcIdNode.text().toInt();
        QString sinkPinName = sinkPinNameNode.text();

        QDomElement sourceNode =connectionNode.firstChildElement( "source" );
        if( sourceNode.isNull() )
        {
            // error
        }
        QDomElement sourceProcIdNode = sourceNode.firstChildElement( "processorId" );
        if( sourceProcIdNode.isNull() )
        {
            //error
        }
        QDomElement sourcePinNameNode = sourceNode.firstChildElement( "pinName" );
        if( sourcePinNameNode.isNull() )
        {
            //error
        }

        int sourceProcId = sourceProcIdNode.text().toInt();
        QString sourcePinName = sourcePinNameNode.text();

        qDebug() << "Found connection with source( " << sourceProcId << "," << sourcePinName << ") and sink( "
                << sinkProcId << "," << sinkPinName << ")";

        PipelineElement* sourceElement = pipeline->getElement( sourceProcId );
        PipelineElement* sinkElement = pipeline->getElement( sinkProcId );

        if( sourceElement == 0 )
        {
            throw std::runtime_error( "Connection specified with invalid source id");
        }

        if( sinkElement == 0 )
        {
            throw std::runtime_error( "Connection specified with invalid sink id");
        }

        IOutputPin* iop = sourceElement->getOutputPin( sourcePinName );
        IInputPin* iip = sinkElement->getInputPin( sinkPinName );

        if( iop == 0 )
        {
            QString msg = "No pin with name " % sourcePinName;
            throw std::runtime_error( msg.toStdString() );
        }

        if( iip == 0 )
        {
            QString msg = "No pin with name " % sinkPinName;
            throw std::runtime_error( msg.toStdString() );
        }

        pipeline->connectPins( iop, iip );
    }
}

int PipelineLoader::propertyIndex( QObject* qobject, const QString& name )
{
    return qobject->metaObject()->indexOfProperty(  name.toAscii().constData() );
}

QVariant::Type PipelineLoader::propertyType( QObject* qobject, int index )
{
    if( index < 0 ) return( QVariant::Invalid );
    return( qobject->metaObject()->property(index).type() );
}

QVariant::Type PipelineLoader::propertyType( QObject* qobject, const QString& name )
{
    return( propertyType( qobject, propertyIndex( qobject, name ) ) );
}

void PipelineLoader::setProperty( QObject* qobject, int index, const QVariant& value )
{
    if( index >= 0 ) qobject->metaObject()->property(index).write( qobject, value );
}

void PipelineLoader::setProperty( QObject* qobject, const QString& name, const QVariant& value )
{
    setProperty( qobject, propertyIndex( qobject, name), value );
}

QVariant PipelineLoader::convertData( QVariant::Type type, const QString& data )
{
    if (type == QVariant::Bool)
    {
        return(bool(data.compare("true", Qt::CaseInsensitive) ||  data == "1"));
    }
    else if (type == QVariant::Char)
    {
        return(data.size() > 0 ? data.at(0) : QChar());
    }
    else if (type == QVariant::ByteArray)
    {
        return(data.toAscii());
    }
    else if (type == QVariant::Date)
    {
        return(QDateTime::fromString(data));
    }
    else if (type == QVariant::Double)
    {
        return(data.toDouble());
    }
    else if (type == QVariant::Int)
    {
        return(data.toInt());
    }
    else if (type == QVariant::LongLong)
    {
        return(data.toLongLong());
    }
    else if (type == QVariant::Time)
    {
        return(QTime::fromString(data));
    }
    else if (type == QVariant::UInt)
    {
        return(data.toUInt());
    }
    else if (type == QVariant::ULongLong)
    {
        return(data.toULongLong());
    }
    else if (type == QVariant::Url)
    {
        return(QUrl(data));
    }

    return(data);
}
