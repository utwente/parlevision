#ifndef _PIPELINELOADER_H_
#define _PIPELINELOADER_H_

#include <QXmlStreamReader>
#include <QDomDocument>
#include <QVariant>
#include "PlvExceptions.h"
#include "RefPtr.h"

namespace plv
{
    class PipelineElement;
    class Pipeline;

    /** Class which parses XML and maps XML data to properties in QObject classes.
      * Original idea and source thanks to Matteo Bertozzi
      * (http://th30z.netsons.org/2009/04/qt4-xml-objec-mapper/)
      */
    class PipelineLoader
    {
    public:
        static RefPtr<Pipeline> parsePipeline( const QString& filename )
            throw(std::runtime_error); /*TODO checked exceptions*/

        static RefPtr<Pipeline> parsePipeline( QDomDocument* doc )
            throw(std::runtime_error); /*TODO checked exceptions*/

        static void pipelineToXML( Pipeline* pipeline )
            throw(std::runtime_error); /*TODO checked exceptions*/

    private:
        PipelineLoader();
        ~PipelineLoader();

        static void parseElements( QDomNodeList* list, Pipeline* pipeline )
            throw(std::runtime_error); /*TODO checked exceptions*/

        static void parseConnections( QDomNodeList* list, Pipeline* pipeline )
            throw(std::runtime_error); /*TODO checked exceptions*/



        /** QObject propery system helper methods. Used to map XML data
          * to properties in QObject classes.
          * Original idea and source thanks to Matteo Bertozzi
          * (http://th30z.netsons.org/2009/04/qt4-xml-objec-mapper/)
          */
        static int propertyIndex( QObject* qobject, const QString& name );
        static QVariant::Type propertyType( QObject* qobject, int index );
        static QVariant::Type propertyType( QObject* qobject, const QString& name );
        static void setProperty( QObject* qobject, int index, const QVariant& value );
        static void setProperty( QObject* qobject, const QString& name, const QVariant& value );
        static QVariant convertData (QVariant::Type type, const QString& data);
    };
}

#endif // _PIPELINELOADER_H_
