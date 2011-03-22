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

#include "ElementConfigFormBuilder.h"
#include <QMetaType>
#include <QDebug>
#include <stdexcept>
#include <assert.h>

using namespace plvgui;
using namespace plv;

QHash<QString,QString> ElementConfigFormBuilder::s_typeMap;

void ElementConfigFormBuilder::registerType(QString elementTypeName, QString builderTypeName)
{
    qDebug() << "Registering ElementconfigFormBuilder " << builderTypeName
                    << " for " << "'" << elementTypeName << "'";

    ElementConfigFormBuilder::s_typeMap.insert( elementTypeName, builderTypeName );
}

bool ElementConfigFormBuilder::existsFor(QString elementTypeName)
{
    return s_typeMap.contains(elementTypeName);
}

ElementConfigFormBuilder* ElementConfigFormBuilder::getBuilderFor(QString elementTypeName)
{
    assert(existsFor(elementTypeName));
    if(!existsFor(elementTypeName))
    {
        throw new std::runtime_error(
                QString("No formbuilder registered for "+elementTypeName).toStdString());
    }

    const QString builderTypeName = ElementConfigFormBuilder::s_typeMap[elementTypeName];

    int typeId = QMetaType::type(builderTypeName.toAscii());
    if(typeId == 0)
        throw new std::runtime_error(
                QString("Tried to create unknown formbuilder "+builderTypeName).toStdString());

    ElementConfigFormBuilder* b = static_cast<ElementConfigFormBuilder*>(QMetaType::construct(typeId));

    return b;
}
