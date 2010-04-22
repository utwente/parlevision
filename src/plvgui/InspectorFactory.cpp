#include "InspectorFactory.h"
#include "OpenCVImageInspector.h"
#include "Inspector.h"

using namespace plvgui;

Inspector* InspectorFactory::create(QString dataType)
        throw(InspectorCreationException)
{
    //TODO make this dynamic
    if(dataType == "OpenCVImage")
    {
        return new OpenCVImageInspector();
    }
    else
    {
        QString msg = "Unsupported type '" + dataType + "'";
        throw InspectorCreationException( msg.toStdString() );
    }
}
