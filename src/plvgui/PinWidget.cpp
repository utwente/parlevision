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
    rect.adjust(-7,7,0,0);
    return rect;
}

void PinWidget::init(bool isInput=true)
{
    setAcceptHoverEvents(true);

    assert(m_pin.isNotNull());
    QGraphicsTextItem* label = new QGraphicsTextItem(m_pin->getName());
    this->addToGroup(label);
    label->translate(6,0);

    this->circle = new QGraphicsEllipseItem(0,0,7,7,this);
    this->addToGroup(circle);
    this->circle->translate(0, 10);

    if(!isInput)
    {
        qDebug() << label->textWidth();
        this->circle->translate(100, 0);
    }
}

void PinWidget::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    qDebug() << "hovering above " << this->getPin()->getName();
    event->accept();
}

void PinWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    // only accept clicks if they are on the circle
    // ignore clicks on the label, as accepting those clicks
    // would make dragging the parent element very hard.
    if(!this->circle->contains(this->circle->mapFromParent(event->pos())))
    {
        qDebug() << "clicked on PinWidget, but not inside circle "
                << circle->pos() << " != " << event->pos();
        event->ignore();
        return;
    }

    event->accept();

    assert(this->scene() != 0);
    if(this->scene() != 0)
    {
        QCoreApplication::postEvent(this->scene(), new PinClickedEvent(this));
    }
}

void PinWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    // Mouse releases are a bit more liberal,
    // they don't have to be above the circle;
    // anywhere on the widget is OK
    if(!this->contains(event->pos()))
    {
        qDebug() << "PinWidget["<< this->getPin()->getName() << "]: "
                <<"mouse released elsewhere: " << this->pos() << "+"<< this->boundingRect()
                << " <> " << event->pos();

        event->ignore();
        return;
    }

    event->accept();

    assert(this->scene() != 0);
    if(this->scene() != 0)
    {
        qDebug() << "posting event";
        QCoreApplication::postEvent(this->scene(), new PinReleasedEvent(this));
    }
}
