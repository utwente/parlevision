#include "RendererFactory.h"

#include <QDebug>
#include <typeinfo>

#include "OpenCVImageRenderer.h"
#include "DataRenderer.h"
#include "OpenCVImage.h"

using namespace plvgui;

DataRenderer* RendererFactory::create(QString dataType, QWidget *parent)
        throw(RendererCreationException)
{
    //TODO make this dynamic
    qDebug() << "RendererFactory creating "<<dataType;
    if(dataType == typeid(plv::OpenCVImage).name())
    {
        return new OpenCVImageRenderer(parent);
    }
    else
    {
        QString msg = "Unsupported type '" + dataType + "'";
        throw RendererCreationException( msg.toStdString() );
    }
}
