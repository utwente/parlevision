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
        static RefPtr<Pipeline> deserialize( const QString& filename )
            throw(std::runtime_error); /*TODO checked exceptions*/

        static RefPtr<Pipeline> deserialize( QDomDocument* doc )
            throw(std::runtime_error); /*TODO checked exceptions*/

        static void serialize( const QString& filename, Pipeline* pipeline )
            throw(std::runtime_error); /*TODO checked exceptions*/

    private:
        PipelineLoader();
        ~PipelineLoader();

        static QString serialize( Pipeline* pipeline )
            throw(std::runtime_error); /*TODO checked exceptions*/

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
