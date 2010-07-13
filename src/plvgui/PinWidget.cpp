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

QRectF PinWidget::boundingRect() const
{
    QRectF rect = QGraphicsItemGroup::boundingRect();
    rect.adjust(-7,0,0,0);
    return rect;
}

void PinWidget::init(bool isInput=true)
{
    setAcceptHoverEvents(true);

    if(isInput)
    {
        //TODO
    }
    assert(m_pin.isNotNull());
    QGraphicsTextItem* label = new QGraphicsTextItem(m_pin->getName());
    this->addToGroup(label);
    label->translate(6,0);

    QGraphicsEllipseItem* ellipse = new QGraphicsEllipseItem(0,10,7,7,this);
    this->addToGroup(ellipse);
}

void PinWidget::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    qDebug() << "Pin hover " << m_pin->getName();
    event->accept();
}

void PinWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "Pin click " << m_pin->getName();
    event->accept();
}
