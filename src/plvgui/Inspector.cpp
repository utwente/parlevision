#include "Inspector.h"

#include <QDebug>

#include "Pin.h"

using namespace plvgui;
using namespace plv;

Inspector::Inspector(QWidget *parent)
    : QWidget(parent)
{
}

Inspector::~Inspector()
{
    disconnect();
}

void Inspector::setPin( const plv::Pin* p )
{
    qDebug() << "attaching inspector to pin";
    disconnect();
    connect( p, SIGNAL( newData( RefPtr<Data> )),
             this, SLOT( newData( RefPtr<Data> )) );
}

void Inspector::newData( RefPtr<Data> )
{
    qDebug() << "WARNING: Abstract Inspector received newData signal.\n"
                << "Did you forget to implement newData(Data* data) in a subclass?";
}
