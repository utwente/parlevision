/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvgui module of ParleVision.
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

#ifndef ELEMENTCONFIGFORMBUILDER_H
#define ELEMENTCONFIGFORMBUILDER_H

#include <QObject>
#include <QMetaType>
#include <QHash>

#include "plvcore/plvglobal.h"

class QString;
class QWidget;

namespace plv
{
    class PipelineElement;
}

using namespace plv;

namespace plvgui
{
    /** Interface for plugins that need custom configuration forms.
      * To enable custom config forms,
      * implement this interface for your PipelineElement
      * and call plvRegisterConfigFormBuilder<YourElement,YourFormBuilder>("YourFormBuilder");
      * Don't forget to include the Q_OBJECT macro in your subclass!
      */
    class PLVGUI_EXPORT ElementConfigFormBuilder : public QObject
    {
        Q_OBJECT

    public:
        /** Implement this method to return a custom QWidget
          * that knows how to configure element
          */
        virtual QWidget* buildForm(PipelineElement* element, QWidget* parent=0)=0;

        /** Register the given type as a PipelineElement Type.
          * The type needs to be known to Qt's MetaType system,
          * so you will likely rarely call this yourself.
          * Use the plvRegisterFormBuilder macro instead.
          * @require typeName was not registered before.
          * @require typeName is a type registered to the Qt MetaType system
          *     e.g. QMetaType::type(typeName) returns a valid ID
          */
        static void registerType(QString elementTypeName, QString builderTypeName);

        /** Check if a custom builder exists for this element type */
        static bool existsFor(QString elementTypeName);

        /** Construct a new ElementConfigFormBuilder for typeName
          * @require existsFor(elementTypeName)
          *             (e.g. typeName has been registered to a valid type)
          */
        static ElementConfigFormBuilder* getBuilderFor(QString elementTypeName);



        ElementConfigFormBuilder() {}
        virtual ~ElementConfigFormBuilder() {}
        ElementConfigFormBuilder(const ElementConfigFormBuilder&) : QObject() {}

    private:
        static QHash<QString,QString> s_typeMap;
    };
}

// usage:
// plvRegisterConfigFormBuider<CameraConfigFormBuilder>("plv::CameraProducer,
//                                                      "plv::CameraConfigFormBuilder");
template<typename CFBT>
int plvRegisterConfigFormBuilder(const char* elementTypeName, const char* builderTypeName)
{
    plvgui::ElementConfigFormBuilder::registerType(elementTypeName, builderTypeName);
    return qRegisterMetaType<CFBT>(builderTypeName);
}


#endif // ELEMENTCONFIGFORMBUILDER_H
