#include "LibraryElement.h"
#include <QtGui>

#include "PipelineElement.h"

using namespace plv;
using namespace plvgui;

LibraryElement::LibraryElement(QString typeName, QWidget* parent)
    : QWidget(parent),
    typeName(typeName),
    name(PipelineElement::nameForType(typeName))
{
    QSizePolicy sp = QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    sp.setHeightForWidth(true);
    this->setSizePolicy(sp);
    this->setStyleSheet("background: lightgreen;");
    this->setMinimumSize(100,40);
    this->setMaximumSize(100,40);
//        this->adjustSize();
    this->resize(100,40);
    this->setFixedHeight(40);
    this->layout = new QVBoxLayout();
    this->setLayout(layout);
    this->layout->addWidget(new QLabel(this->name, this));
}

QSize LibraryElement::sizeHint() const
{
    return QSize(100,40);
}

int LibraryElement::heightForWidth(int w) const
{
    return 40;
}


