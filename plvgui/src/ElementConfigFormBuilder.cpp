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

    ElementConfigFormBuilder::s_typeMap[elementTypeName] = builderTypeName;
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
