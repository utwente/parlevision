#include "LibraryElement.h"

#include <QtGui>

#include "RefPtr.h"
#include "PipelineElement.h"

using namespace plv;
using namespace plvgui;

LibraryElement::LibraryElement(RefPtr<PipelineElement> element, QWidget* parent)
    : QLabel(parent),
    element(element)
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
    this->setText(this->element->getName());
}

QSize LibraryElement::sizeHint() const
{
    return QSize(100,40);
}

int LibraryElement::heightForWidth(int) const
{
    return 40;
}

