#include "LibraryElement.h"

#include <QtGui>

#include "RefPtr.h"
#include "PipelineElement.h"
#include "Pin.h"

using namespace plv;
using namespace plvgui;

LibraryElement::LibraryElement(RefPtr<PipelineElement> element, QWidget* parent)
    : QWidget(parent),
    element(element)
{
    QSizePolicy sp = QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    sp.setHeightForWidth(true);
    this->setSizePolicy(sp);
    this->setStyleSheet("background: lightgreen; padding: 0; margin: 0;");
//    this->setMinimumSize(100,40);
//    this->setMaximumSize(100,40);
//        this->adjustSize();
//    this->resize(100,40);
//    this->setFixedHeight(40);

    this->setObjectName("Library Element " + element->getName());

    this->outerContainer = new QVBoxLayout(this);
    this->outerContainer->setContentsMargins(4,4,4,4);

    QWidget* pinWrapper = new QWidget(this);
    pinWrapper->setStyleSheet("background: red; padding: 0; margin: 0;");
    this->innerContainer = new QHBoxLayout(pinWrapper);
    this->innerContainer->setAlignment(Qt::AlignTop);
    this->innerContainer->setContentsMargins(0,0,0,0);
    QWidget* inPinWrapper = new QWidget(this);
    inPinWrapper->setStyleSheet("background: pink;");
    this->inPinContainer = new QVBoxLayout(inPinWrapper);
    this->inPinContainer->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->inPinContainer->setContentsMargins(0,0,0,0);
    QWidget* outPinWrapper = new QWidget(this);
    outPinWrapper->setStyleSheet("background: yellow;");
    this->outPinContainer = new QVBoxLayout(outPinWrapper);
    this->outPinContainer->setAlignment(Qt::AlignTop | Qt::AlignRight);
    this->outPinContainer->setContentsMargins(0,0,0,0);

    this->innerContainer->addWidget(inPinWrapper, Qt::AlignLeft | Qt::AlignTop);
    this->innerContainer->addWidget(outPinWrapper, Qt::AlignRight | Qt::AlignTop);
    QLabel* title = new QLabel(this->element->getName(), this);
    title->setStyleSheet("background: blue;");
    this->outerContainer->addWidget(title, Qt::AlignHCenter);
    this->outerContainer->addWidget(pinWrapper);


    const PipelineElement::InputPinMap& inPins = element->getInputPins();
    for( PipelineElement::InputPinMap::const_iterator itr = inPins.begin()
        ; itr!=inPins.end(); ++itr)
    {
        RefPtr<IInputPin> pin = itr->second;
        assert(pin.isNotNull());
        QWidget* label = new QLabel(pin->getName());
        label->setStyleSheet("background: orange;");
        inPinContainer->addWidget(label, Qt::AlignLeft | Qt::AlignTop);
    }

    const PipelineElement::OutputPinMap& outPins = element->getOutputPins();
    for( PipelineElement::OutputPinMap::const_iterator itr = outPins.begin()
        ; itr!=outPins.end(); ++itr)
    {
        RefPtr<IOutputPin> pin = itr->second;
        assert(pin.isNotNull());
        QWidget* label = new QLabel(pin->getName());
        label->setStyleSheet("background: orange;");
        outPinContainer->addWidget(label, Qt::AlignRight | Qt::AlignTop);
    }
}

QSize LibraryElement::sizeHint() const
{
//    return QSize(100,40);
    return QWidget::sizeHint();
}

int LibraryElement::heightForWidth(int w) const
{
    return QWidget::heightForWidth(w);
}

void LibraryElement::mousePressEvent(QMouseEvent*)
{
    emit(pressed());
}

void LibraryElement::mouseMoveEvent(QMouseEvent*)
{
    emit(moved());
}

void LibraryElement::mouseReleaseEvent(QMouseEvent*)
{
    emit(released());
}
