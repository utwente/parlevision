#ifndef _PIPELINELOADER_H_
#define _PIPELINELOADER_H_

#include <QXmlStreamReader>
#include "PlvExceptions.h"

namespace plv
{
    class PipelineElement;
    class Pipeline;

    class PipelineLoader
    {
    public:
        PipelineLoader();
        ~PipelineLoader();

        static Pipeline* parsePipeline( const QString& filename )
            throw(std::runtime_error); /*TODO checked exceptions*/
        static Pipeline* parsePipeline( QXmlStreamReader* xmlReader )
            throw(std::runtime_error); /*TODO checked exceptions*/

    private:
        static PipelineElement* parseElement( QXmlStreamReader* xmlReader )
            throw(std::runtime_error) /*TODO checked exceptions*/;
        static void parseConnections( QXmlStreamReader* xmlReader, Pipeline* pipeline )
            throw(std::runtime_error) /*TODO checked exceptions*/;
        static void parseConnection( QXmlStreamReader* xmlReader, Pipeline* pipeline )
            throw(std::runtime_error) /*TODO checked exceptions*/;

        static void readUnknownXmlNode( QXmlStreamReader *xmlReader );
        static QString readXmlNode( QXmlStreamReader *xmlReader );
    };
}

#endif // _PIPELINELOADER_H_
