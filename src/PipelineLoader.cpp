#include "PipelineLoader.h"
#include "Pipeline.h"
#include "PipelineProcessor.h"
#include "XmlMapper.h"

#include <QStringList>
#include <QFile>
#include <QMetaProperty>

using namespace plv;

PipelineLoader::PipelineLoader()
{

}

PipelineLoader::~PipelineLoader()
{

}

Pipeline* PipelineLoader::parsePipeline( const QString& filename )
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
    QXmlStreamReader xmlReader( &file );
    return parsePipeline( &xmlReader );
}

Pipeline* PipelineLoader::parsePipeline( QXmlStreamReader* xmlReader )
        throw(std::runtime_error) /*TODO checked exceptions*/
{
    Pipeline* pipeline = new Pipeline();

    try
    {
        while( !xmlReader->atEnd() )
        {
            xmlReader->readNext();

            if( xmlReader->isEndElement() )
                break;

            if( xmlReader->isStartElement() )
            {
                QString tagName = xmlReader->name().toString();

                if( QString::compare(tagName, "element") == 0 )
                {
                    PipelineElement* ple = parseElement( xmlReader );

                    // this adds processor to the pipeline
                    // and makes pipeline responsible for cleanup
                    pipeline->add( ple );
                }
                else if( QString::compare(tagName, "connections") == 0 )
                {
                    parseConnections( xmlReader, pipeline );
                }
            }
        }
    }
    catch( ... )
    {
        // do cleanup
        delete pipeline;
        throw;
    }
    return pipeline;
}

PipelineElement* PipelineLoader::parseElement( QXmlStreamReader* xmlReader )
        throw(std::runtime_error) /*TODO checked exceptions*/
{
    QString name;
    int id = 0;

    QXmlStreamAttributes attributes = xmlReader->attributes();
    for( QXmlStreamAttributes::iterator itr = attributes.begin(); itr != attributes.end(); ++itr )
    {
        QXmlStreamAttribute attribute = *itr;
        QString tmp = attribute.name().toString();
        if( tmp == "name" )
        {
            name = attribute.value().toString();
        }
        else if( tmp == "id")
        {
            id = attribute.value().toString().toInt();
        }
    }
    if( name.isEmpty() )
    {
        throw std::runtime_error ( "XML document contains processor type with empty name attribute." );
    }
    if( id == 0 )
    {
        throw std::runtime_error ( "XML document contains processor type with no or invalid id attribute." );
    }

    // Use Qt metatype system to locate this processor type
    int typeId = QMetaType::type( name.toAscii() );
    if(typeId == 0)
    {
        throw std::runtime_error (
            QString( "XML document contains unknown processor type " + name).toStdString() );
    }
    // and instantiate it
    PipelineElement* ple = static_cast<PipelineElement*>( QMetaType::construct( typeId ) );

    while( !xmlReader->atEnd() )
    {
        xmlReader->readNext();

        if (xmlReader->isEndElement())
            break;

        if( xmlReader->isStartElement() )
        {
            QString tagName = xmlReader->name().toString();
            if( QString::compare(tagName, "properties") == 0 )
            {
                // map all properties to this processor using Qt property system.
                XmlMapper xmlMapper1( ple );
                xmlMapper1.map( xmlReader );
            }
            else
            {
                readUnknownXmlNode( xmlReader );
            }
        }
    }

    qDebug() << "Loaded element " << name << " with properties";

    const QMetaObject* metaObject = ple->metaObject();
    QStringList properties;
    for(int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i)
    {
        QMetaProperty property = metaObject->property(i);
        qDebug() << QString::fromLatin1( property.name() );
    }

    return ple;
}

void PipelineLoader::parseConnection( QXmlStreamReader* xmlReader, Pipeline* pipeline )
    throw(std::runtime_error) /*TODO checked exceptions*/
{
    qDebug() << "Parsing connection ... ";

    int sourceId = 0;
    QString sourceElementName;
    QString sourcePinName;

    int sinkId = 0;
    QString sinkElementName;
    QString sinkPinName;

    while( !xmlReader->atEnd() )
    {
        xmlReader->readNext();

        if( xmlReader->isEndElement() )
            break;

        if( xmlReader->isStartElement() )
        {
            QString tagName = xmlReader->name().toString();
            if( QString::compare(tagName, "source") == 0 )
            {
                while( !xmlReader->atEnd() )
                {
                    xmlReader->readNext();

                    if( xmlReader->isEndElement() )
                        break;

                    if( xmlReader->isStartElement() )
                    {
                        tagName = xmlReader->name().toString();

                        if( QString::compare(tagName, "processorId") == 0 )
                        {
                            sourceId = readXmlNode( xmlReader ).toInt();
                        }
                        else if( QString::compare(tagName, "processorName") == 0 )
                        {
                            sourceElementName = readXmlNode( xmlReader );
                        }
                        else if( QString::compare(tagName, "pinName") == 0 )
                        {
                            sourcePinName = readXmlNode( xmlReader );
                        }
                        else
                        {
                            readUnknownXmlNode( xmlReader );
                        }
                    }
                }
            }
            else if( QString::compare(tagName, "sink") == 0 )
            {
                while( !xmlReader->atEnd() )
                {
                    xmlReader->readNext();

                    if( xmlReader->isEndElement() )
                        break;

                    if( xmlReader->isStartElement() )
                    {
                        tagName = xmlReader->name().toString();

                        if( QString::compare(tagName, "processorId") == 0 )
                        {
                            sinkId = readXmlNode( xmlReader ).toInt();
                        }
                        else if( QString::compare(tagName, "processorName") == 0 )
                        {
                            sinkElementName = readXmlNode( xmlReader );
                        }
                        else if( QString::compare(tagName, "pinName") == 0 )
                        {
                            sinkPinName = readXmlNode( xmlReader );
                        }
                        else
                        {
                            readUnknownXmlNode( xmlReader );
                        }
                    }
                }
            }
        }
    }

    qDebug() << "Found source with id: " << sourceId << " elementName: " << sourceElementName << " and sourcePinName: " << sourcePinName;
    qDebug() << "Found sink with id: " << sinkId << " selementName: " << sinkElementName << " and sinkPinName: " << sinkPinName;

    PipelineElement* sourceElement = pipeline->getElement( sourceId );
    PipelineElement* sinkElement = pipeline->getElement( sinkId );

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
        throw std::runtime_error( QString("No pin with name" + sourcePinName).toStdString() );
    }

    if( iip == 0 )
    {
        throw std::runtime_error( QString("No pin with name" + sinkPinName).toStdString() );
    }

    pipeline->connectPins( iop, iip );
}

void PipelineLoader::parseConnections( QXmlStreamReader* xmlReader, Pipeline* pipeline )
    throw(std::runtime_error) /*TODO checked exceptions*/
{
    while( !xmlReader->atEnd() )
    {
        xmlReader->readNext();

        if( xmlReader->isEndElement() )
            break;

        if( xmlReader->isStartElement() )
        {
            QString tagName = xmlReader->name().toString();
            if( QString::compare(tagName, "connection") == 0 )
            {
                parseConnection( xmlReader, pipeline );
            }
            else
            {
                readUnknownXmlNode( xmlReader );
            }
        }
    }
}

void PipelineLoader::readUnknownXmlNode( QXmlStreamReader *xmlReader )
{
    if (xmlReader->isStartElement())
    {
        QString tagName = xmlReader->name().toString();
        qDebug() << "Unknown XML tag: " << tagName;
    }

    while( !xmlReader->atEnd() )
    {
        xmlReader->readNext();

        if (xmlReader->isEndElement())
            break;

        if (xmlReader->isStartElement())
        {
            readUnknownXmlNode(xmlReader);
        }
    }
}

QString PipelineLoader::readXmlNode (QXmlStreamReader *xmlReader)
{
    QString buffer;

    while (!xmlReader->atEnd())
    {
        xmlReader->readNext();

        if (xmlReader->isEndElement())
            break;

        buffer = xmlReader->text().toString();
    }
    return(buffer);
}
