#include "DataRenderer.h"

#include <QDebug>

#include "Pin.h"

using namespace plvgui;
using namespace plv;

DataRenderer::DataRenderer(QWidget *parent)
    : QWidget(parent)
{
}

DataRenderer::~DataRenderer()
{
    disconnect();
}

void DataRenderer::setPin( const plv::Pin* p )
{
    qDebug() << "attaching inspector to pin";
    disconnect();
    connect( p, SIGNAL( newData( RefPtr<Data> )),
             this, SLOT( newData( RefPtr<Data> )) );
}

void DataRenderer::newData( RefPtr<Data> )
{
    qDebug() << "WARNING: Abstract DataRenderer received newData signal.\n"
                << "Did you forget to implement newData(Data* data) in a subclass?";
}
