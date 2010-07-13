#include "PinWidget.h"
#include <QtGui>
#include "PipelineElementWidget.h"
#include "Pin.h"
#include "PinClickedEvent.h"

using namespace plvgui;
using namespace plv;

PinWidget::PinWidget(PipelineElementWidget *parent, RefPtr<IInputPin> pin)
    : QObject(parent),
    QGraphicsItemGroup(parent),
    m_parent(parent),
    m_pin(pin)
{
    assert(m_pin.isNotNull());
    init(true);
}

PinWidget::PinWidget(PipelineElementWidget *parent, RefPtr<IOutputPin> pin)
    : QObject(parent),
    QGraphicsItemGroup(parent),
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

    this->circle = new QGraphicsEllipseItem(0,10,7,7,this);
    this->addToGroup(circle);
}

void PinWidget::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    event->accept();
}

void PinWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    // only accept clicks if they are on the circle
    // ignore clicks on the label, as accepting those clicks
    // would make dragging the parent element very hard.
    if(this->circle->contains(event->pos()))
    {
        dumpObjectTree();
        event->accept();
        qDebug() << PinClickedEvent::user_type();

        assert(this->scene() != 0);
        if(this->scene() != 0)
        {
            qDebug() << "posting event";
            QCoreApplication::postEvent(this->scene(), new PinClickedEvent(this));
        }
    }
    else
    {
        event->ignore();
    }
}
