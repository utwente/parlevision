#include "LibraryElement.h"
#include <QtGui>

#include "PipelineElement.h"

using namespace plv;
using namespace plvgui;

LibraryElement::LibraryElement(QString typeName, QWidget* parent)
    : QLabel(PipelineElement::nameForType(typeName), parent),
    typeName(typeName),
    name(PipelineElement::nameForType(typeName))
{
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->setStyleSheet("background: red;");
    this->setMinimumSize(100,40);
    this->setMaximumSize(100,40);
//        this->adjustSize();
    this->resize(100,40);
}
