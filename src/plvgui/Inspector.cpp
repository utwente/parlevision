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

void Inspector::setPin(const plv::Pin* p)
{
    disconnect();
    connect( p, SIGNAL(newData(Data*)),
             this, SLOT(newData(Data*)) );
}

void Inspector::newData(Data*)
{
    qDebug() << "WARNING: Abstract Inspector received newData signal.\n"
                << "Did you forget to implement newData(Data* data) in a subclass?";
}
