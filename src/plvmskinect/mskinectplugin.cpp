#include "mskinectplugin.h"
#include <QtPlugin>
#include <QtDebug>

#include "mskinectproducer.h"
#include "mskinectdatatypes.h"
#include "skeletondataviewer.h"
#include <plvgui/RendererFactory.h>

using namespace plv;
using namespace plvmskinect;

//const char* Test::getDataTypeName() const
//{
//    return QMetaType::typeName( qMetaTypeId<plvmskinect::SkeletonFrame>() );
//}

//plvgui::DataRenderer* Test::create(QWidget* parent) const
//{
//    plvgui::DataRenderer* r = static_cast<plvgui::DataRenderer*>(new plvmskinect::SkeletonDataViewer(parent));
//    r->setParent( parent );
//    return r;
//}

MSKinectPlugin::MSKinectPlugin()
{
    qDebug() << "MSKinectPlugin constructor";
}

MSKinectPlugin::~MSKinectPlugin()
{
    qDebug() << "MSKinectPlugin destructor";
}

void MSKinectPlugin::onLoad()
{
    qDebug() << "MSKinectPlugin onLoad";

    qRegisterMetaType< plvmskinect::SkeletonFrame >( "plvmskinect::SkeletonFrame" );

    plvRegisterPipelineElement<plvmskinect::MSKinectProducer>();

    // register renderers
    plvgui::RendererFactory::add<plvmskinect::SkeletonFrame, plvmskinect::SkeletonDataViewer>();

    //plvgui::RendererFactory::instance()->add( new Test() );
}

Q_EXPORT_PLUGIN2(mskinect_plugin, MSKinectPlugin)
