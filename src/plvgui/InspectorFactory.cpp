#include "InspectorFactory.h"

#include <QDebug>
#include <typeinfo>

#include "OpenCVImageInspector.h"
#include "Inspector.h"
#include "OpenCVImage.h"

using namespace plvgui;

Inspector* InspectorFactory::create(QString dataType, QWidget *parent)
        throw(InspectorCreationException)
{
    //TODO make this dynamic
    qDebug() << "InspectorFactory creating "<<dataType;
    if(dataType == typeid(plv::OpenCVImage).name())
    {
        return new OpenCVImageInspector(parent);
    }
    else
    {
        QString msg = "Unsupported type '" + dataType + "'";
        throw InspectorCreationException( msg.toStdString() );
    }
}
