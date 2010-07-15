#include <QMetaProperty>
#include <QMetaObject>

#include <QDateTime>
#include <QTime>
#include <QUrl>

#include "xmlmapper.h"

using namespace plv;

/* ============================================================================
 *  PRIVATE class
 *
 */
class XmlMapperPrivate 
{
	public:
		bool useIntrospection;
        XmlHashMap mapping;
		QObject *entity;

	public:
		int propertyIndex (const QString& name);

		QVariant::Type propertyType (int index);
		QVariant::Type propertyType (const QString& name);

		void setProperty (int index, const QVariant& value);
		void setProperty (const QString& name, const QVariant& value);
};

int XmlMapperPrivate::propertyIndex (const QString& name) 
{
	if (!useIntrospection)
		return(-1);

    const QMetaObject* metaObject = entity->metaObject();
    const char* charName = name.toAscii().constData();
    int index = metaObject->indexOfProperty( charName );
    return index;
}

QVariant::Type XmlMapperPrivate::propertyType (int index)
{
	if (index < 0) return(QVariant::Invalid);
	return(entity->metaObject()->property(index).type());
}

QVariant::Type XmlMapperPrivate::propertyType (const QString& name)
{
	return(propertyType(propertyIndex(name)));
}

void XmlMapperPrivate::setProperty (int index, const QVariant& value)
{
	if (index >= 0) entity->metaObject()->property(index).write(entity, value);
}

void XmlMapperPrivate::setProperty (const QString& name, const QVariant& value)
{
	setProperty(propertyIndex(name), value);
}

/* ============================================================================
 *  PUBLIC Constructor/Destructor
 */
XmlMapper::XmlMapper()
    : d(new XmlMapperPrivate)
{
	d->entity = NULL;
	d->useIntrospection = true;
}

XmlMapper::XmlMapper (QObject *entity)
    : d(new XmlMapperPrivate)
{
	d->entity = entity;
	d->useIntrospection = true;
}

XmlMapper::XmlMapper (QObject *entity, const XmlHashMap& mapping)
    : d(new XmlMapperPrivate)
{
	d->entity = entity;
	d->mapping = mapping;
	d->useIntrospection = true;
}

XmlMapper::XmlMapper (const XmlHashMap& mapping)
    : d(new XmlMapperPrivate)
{
	d->entity = NULL;
	d->mapping = mapping;
	d->useIntrospection = true;
}

XmlMapper::~XmlMapper() {
	delete d;
}


/* ============================================================================
 *  PUBLIC Methods
 */
void XmlMapper::map (QIODevice *device)
{
	QXmlStreamReader xmlReader(device);
	map(&xmlReader);
}

void XmlMapper::map (const QString& xml)
{
	QXmlStreamReader xmlReader(xml);
    while (!xmlReader.atEnd())
    {
		xmlReader.readNext();
		if (xmlReader.isStartElement())
			break;
	}

	map(&xmlReader);
}

void XmlMapper::map (const QByteArray& xml)
{
    QXmlStreamReader xmlReader(xml);
    while (!xmlReader.atEnd())
    {
		xmlReader.readNext();
		if (xmlReader.isStartElement())
			break;
	}
	map(&xmlReader);
}

void XmlMapper::map (QXmlStreamReader *xmlReader)
{
    while (!xmlReader->atEnd())
    {
		xmlReader->readNext();

		if (xmlReader->isEndElement())
			break;

		if (xmlReader->isStartElement())
			mapXmlNode(xmlReader);
	}
}


/* ============================================================================
 *  PUBLIC Properties
 */
XmlHashMap& XmlMapper::mapping (void) const
{
	return(d->mapping);
}

void XmlMapper::setMapping (const XmlHashMap& mapping)
{
	d->mapping = mapping;
}


QObject *XmlMapper::entity (void) const
{
	return(d->entity);
}

void XmlMapper::setEntity (QObject *entity)
{
	d->entity = entity;
}


bool XmlMapper::useIntrospection (void)
{
	return(d->useIntrospection);
}

void XmlMapper::setUseIntrospection (bool useIntrospection)
{
	d->useIntrospection = useIntrospection;
}

/* ============================================================================
 *  PROTECTED Methods
 */
void XmlMapper::mapXmlNode (QXmlStreamReader *xmlReader)
{
	QString tagName = xmlReader->name().toString();
    int pIndex;

    if (d->mapping.contains(tagName))
    {
		tagName = d->mapping[tagName];
		QString data = readXmlNode(xmlReader);
		d->setProperty(tagName, convertData(d->propertyType(tagName), data));
    }
    else if ((pIndex = d->propertyIndex(tagName)) >= 0)
    {
		QString data = readXmlNode(xmlReader);
		d->setProperty(pIndex, convertData(d->propertyType(pIndex), data));
    }
    else
    {
		readUnknownXmlNode(xmlReader);
	}
}

void XmlMapper::readUnknownXmlNode (QXmlStreamReader *xmlReader)
{
    while (!xmlReader->atEnd())
    {
		xmlReader->readNext();

		if (xmlReader->isEndElement())
			break;

		if (xmlReader->isStartElement())
			readUnknownXmlNode(xmlReader);
	}
}

QString XmlMapper::readXmlNode (QXmlStreamReader *xmlReader)
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

QVariant XmlMapper::convertData (QVariant::Type type, const QString& data)
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

