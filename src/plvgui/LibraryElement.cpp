#include "LibraryElement.h"
#include <QtGui>

using namespace plvgui;

LibraryElement::LibraryElement(QString name, QWidget* parent)
    : QLabel(name, parent), name(name)
{
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->setStyleSheet("background: red;");
    this->setMinimumSize(100,40);
    this->setMaximumSize(100,40);
//        this->adjustSize();
    this->resize(100,40);
}
