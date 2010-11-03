/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvcore module of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

#include "PipelineLoader.h"
#include "Pipeline.h"
#include "PipelineProcessor.h"
#include "Pin.h"
#include "Enum.h"

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

void PipelineLoader::serialize( const QString& filename, Pipeline* pipeline )
    throw(std::runtime_error) /*TODO checked exceptions*/
{
    QFile file( filename );
    if( !file.open(QIODevice::WriteOnly | QIODevice::Text ))
    {
        throw std::runtime_error (
            QString( "Failed to open file " + filename).toStdString() );
    }
    QTextStream out(&file);
    out << serialize( pipeline );
}

QString PipelineLoader::serialize( Pipeline* pl )
    throw(std::runtime_error) /*TODO checked exceptions*/
{
    //RefPtr<Pipeline> pl = pipeline;

    QDomDocument doc;

    QDomElement xmlPipeline = doc.createElement( "pipeline" );
    doc.appendChild( xmlPipeline );

    QDomElement xmlElements = doc.createElement( "elements" );
    xmlPipeline.appendChild( xmlElements );

    const Pipeline::PipelineElementMap& ples = pl->getChildren();
    QMapIterator<int, RefPtr<PipelineElement> > itr( ples );
    while( itr.hasNext() )
    {
        itr.next();
        RefPtr<PipelineElement> ple = itr.value();

        QString className = ple->metaObject()->className();
        int id = ple->getId();

        QDomElement xmlElement = doc.createElement( "element" );
        xmlElement.setAttribute( "id", id );
        xmlElement.setAttribute( "name", className );
        xmlElements.appendChild( xmlElement );

        QDomElement xmlProperties = doc.createElement( "properties" );
        xmlElement.appendChild( xmlProperties );

        // first do static properties
        const QMetaObject* metaObject = ple->metaObject();
        for(int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i)
        {
            QMetaProperty property = metaObject->property(i);
            QString propertyName = QString::fromLatin1( property.name() );

            QString propertyValue;
            QVariant::Type propertyType = property.type();

            // custom types are saved here
            if( propertyType == QVariant::UserType )
            {
                // TODO: pass control here to pipeline element
                // to parse custom types?

                QVariant value = property.read( ple );
                if( value.canConvert<plv::Enum>() )
                {
                    plv::Enum e = value.value<plv::Enum>();
                    propertyValue = e.getSelectedItemName();
                }
            }
            // all other QVariant can easily be converted to string
            else
            {
                propertyValue = property.read( ple ).toString();
            }
            QDomElement xmlProperty = doc.createElement( propertyName );
            QDomText text = doc.createTextNode( propertyValue );
            xmlProperty.appendChild( text );
            xmlProperties.appendChild( xmlProperty );
        }

        // now dynamic properties
        // these are not used by processor definitions
        {
            QVariant xVal = ple->property("sceneCoordX");
            QVariant yVal = ple->property("sceneCoordY");
            if( xVal.isValid() && yVal.isValid() )
            {
                QDomElement xmlXValProperty = doc.createElement( "sceneCoordX" );
                QDomText xValText = doc.createTextNode( xVal.toString() );
                xmlXValProperty.appendChild( xValText );
                xmlProperties.appendChild( xmlXValProperty );

                QDomElement xmlYValProperty = doc.createElement( "sceneCoordY" );
                QDomText yValText = doc.createTextNode( yVal.toString() );
                xmlYValProperty.appendChild( yValText );
                xmlProperties.appendChild( xmlYValProperty );
            }
        }
    }

    QDomElement xmlConnections = doc.createElement( "connections" );
    xmlPipeline.appendChild( xmlConnections );

    const Pipeline::PipelineConnectionsList& connections = pl->getConnections();
    foreach( RefPtr<PinConnection> connection, connections )
    {
        QDomElement xmlConnection = doc.createElement( "connection" );
        xmlConnections.appendChild( xmlConnection );

        QDomElement xmlSink = doc.createElement("sink");
        xmlConnection.appendChild( xmlSink );

        QDomElement xmlSinkPinName = doc.createElement( "pinName" );
        QDomElement xmlSinkId = doc.createElement( "processorId" );

        xmlSink.appendChild( xmlSinkPinName );
        xmlSink.appendChild( xmlSinkId );

        QString sinkPinName = connection->toPin()->getName();
        QString sinkId = QVariant( connection->toPin()->getOwner()->getId() ).toString();

        QDomText xmlSinkNameText = doc.createTextNode( sinkPinName );
        QDomText xmlSinkIdText = doc.createTextNode( sinkId );

        xmlSinkPinName.appendChild( xmlSinkNameText );
        xmlSinkId.appendChild( xmlSinkIdText );

        QDomElement xmlSource = doc.createElement("source");
        xmlConnection.appendChild( xmlSource );

        QDomElement xmlSourcePinName = doc.createElement( "pinName" );
        QDomElement xmlSourceId = doc.createElement( "processorId" );

        xmlSource.appendChild( xmlSourcePinName );
        xmlSource.appendChild( xmlSourceId );

        QString sourcePinName = connection->fromPin()->getName();
        QString sourceId = QVariant( connection->fromPin()->getOwner()->getId() ).toString();

        QDomText xmlSourceNameText = doc.createTextNode( sourcePinName );
        QDomText xmlSourceIdText = doc.createTextNode( sourceId );

        xmlSourcePinName.appendChild( xmlSourceNameText );
        xmlSourceId.appendChild( xmlSourceIdText );
    }
    return doc.toString();
}


RefPtr<Pipeline> PipelineLoader::deserialize( const QString& filename )
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

    QDomDocument doc;
    doc.setContent( &file );
    RefPtr<Pipeline> pl = deserialize( &doc );
    file.close();
    return pl;
}

RefPtr<Pipeline> PipelineLoader::deserialize( QDomDocument* document )
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
            QString msg = "XML parsing failed. PipelineElement with name "
                          % name % " has invalid or used id " + id;
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

                QString propNameXml = element.nodeName();
                QString propValueXml = element.text();

                // convert the data to a known QVariant datatype
                // if the property is unknown it will add the property
                // as string

                const QMetaObject* metaObject = ple->metaObject();
                int index = metaObject->indexOfProperty( propNameXml.toAscii() );
                QMetaProperty property = metaObject->property(index);
                QVariant propValue;
                if( property.type() == QVariant::UserType )
                {
                    propValue = ple->property( propNameXml.toAscii() );
                    if( propValue.canConvert<plv::Enum>() )
                    {
                        plv::Enum e = propValue.value<plv::Enum>();
                        e.setSelected( propValueXml );
                        propValue.setValue( e );

                    }
                }
                else
                {
                    propValue = convertData( property.type(), propValueXml );
                }
                ple->setProperty( propNameXml.toAscii(), propValue );

                qDebug()<< "Found property with name: " << propNameXml
                        << " and value: " << propValue;

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

        qDebug() << "Found connection with source( " << sourceProcId << ","
                 << sourcePinName << ") and sink( " << sinkProcId << ","
                 << sinkPinName << ")";

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
    switch( type )
    {
    case QVariant::Bool:
    {
        // accept either string "true" or "1"
        // everything else is considered false
        bool val = data.compare("true", Qt::CaseInsensitive) == 0;
        val = val || data.compare("1") == 0;
        return val;
    }
    case QVariant::Char:
    {
        return(data.size() > 0 ? data.at(0) : QChar());
    }
    case QVariant::ByteArray:
    {
        return(data.toAscii());
    }
    case QVariant::Date:
    {
        return(QDateTime::fromString(data));
    }
    case QVariant::Double:
    {
        return(data.toDouble());
    }
    case QVariant::Int:
    {
        return(data.toInt());
    }
    case QVariant::LongLong:
    {
        return(data.toLongLong());
    }
    case QVariant::Time:
    {
        return(QTime::fromString(data));
    }
    case QVariant::UInt:
    {
        return(data.toUInt());
    }
    case QVariant::ULongLong:
    {
        return(data.toULongLong());
    }
    case QVariant::Url:
    {
        return(QUrl(data));
    }
    case QVariant::UserType:
    default:
        // unknown, just return as string
        return(data);
    }
}
