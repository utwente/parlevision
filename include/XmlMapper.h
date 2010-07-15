#ifndef _XMLMAPPER_H_
#define _XMLMAPPER_H_

#include <QXmlStreamReader>
#include <QVariant>
#include <QObject>
#include <QHash>

class XmlMapperPrivate;

namespace plv
{

    typedef QHash<QString, QString> XmlHashMap;

    /** Utility class which maps XML data to properties in QObject classes.
      * Original source thanks to Matteo Bertozzi
      * (http://th30z.netsons.org/2009/04/qt4-xml-objec-mapper/)
      * Modified to suit Parlevision code conventions.
      *
      *
      */
    class XmlMapper
    {
        public:
            XmlMapper ();
            XmlMapper (QObject *entity);
            XmlMapper (QObject *entity, const XmlHashMap& mapping);
            XmlMapper (const XmlHashMap& mapping);
            ~XmlMapper();

            // Methods
            void map (QIODevice *device);
            void map (const QString& xml);
            void map (const QByteArray& xml);
            void map (QXmlStreamReader *xmlReader);

            // Properties
            XmlHashMap& mapping (void) const;
            void setMapping (const XmlHashMap& mapping);

            QObject *entity (void) const;
            void setEntity (QObject *entity);

            bool useIntrospection (void);
            void setUseIntrospection (bool useIntrospection);

        protected:
            virtual void mapXmlNode (QXmlStreamReader *xmlReader);
            virtual void readUnknownXmlNode (QXmlStreamReader *xmlReader);

            virtual QString readXmlNode (QXmlStreamReader *xmlReader);
            virtual QVariant convertData (QVariant::Type type, const QString& data);

        private:
            XmlMapperPrivate *d;
    };

}

#endif /* !_XMLMAPPER_H_ */

