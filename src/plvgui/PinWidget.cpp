#include "PinWidget.h"
#include <QtGui>
#include "PipelineElementWidget.h"
#include "Pin.h"

using namespace plvgui;
using namespace plv;

PinWidget::PinWidget(PipelineElementWidget *parent, RefPtr<IInputPin> pin)
    : QGraphicsItemGroup(parent),
    m_parent(parent),
    m_pin(pin)
{
    m_pin = ref_ptr_static_cast<Pin>(pin);
    assert(m_pin.isNotNull());
    init(true);
}

PinWidget::PinWidget(PipelineElementWidget *parent, RefPtr<IOutputPin> pin)
    : QGraphicsItemGroup(parent),
    m_parent(parent),
    m_pin(pin)
{
    init(false);
}

void PinWidget::init(bool isInput=true)
{
    if(isInput)
    {
        //TODO
    }
    assert(m_pin.isNotNull());
    QGraphicsTextItem* label = new QGraphicsTextItem(m_pin->getName());
    this->addToGroup(label);
}
