#include "PinWidget.h"
#include <QtGui>
#include "PipelineElementWidget.h"
#include "Pin.h"
#include "PinClickedEvent.h"
#include "MainWindow.h"

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
    rect.adjust(-6,0,6,0);
    return rect;
}

void PinWidget::init(bool isInput=true)
{
    setAcceptHoverEvents(true);

    assert(m_pin.isNotNull());
    QGraphicsTextItem* label = new QGraphicsTextItem(m_pin->getName());

    this->circle = new QGraphicsEllipseItem(0,0,7,7,this);

    this->circle->translate(0, 10);

    if(isInput)
    {
//        this->circle->translate(6,0);
        label->translate(12,0);
    }
    else
    {
        label->translate(-12,0);
        this->circle->translate(label->boundingRect().width(), 0);
    }

    this->addToGroup(circle);
    this->addToGroup(label);
}

bool PinWidget::sceneEvent ( QEvent * event )
{
    qDebug() << "SceneEvent: " << event;
    return QGraphicsItemGroup::sceneEvent(event);
}

void PinWidget::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    qDebug() << "hovering above " << this->getPin()->getName();
    event->accept();
}

/*
void PinWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF diff = event->scenePos()-event->lastScenePos();
    this->parentItem()->moveBy(diff.x(), diff.y());
}
*/

void PinWidget::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    // only accept clicks if they are on the circle
    // ignore clicks on the label, as accepting those clicks
    // would make dragging the parent element very hard.
    if(!this->circle->contains(this->circle->mapFromParent(event->pos())))
    {
        qDebug() << "clicked on PinWidget, but not inside circle ";
//                << circle->pos() << " != " << event->pos();
        // pass it on to the parent, so it can set us to selected
        QGraphicsItemGroup::mousePressEvent(event);
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
    qDebug() << "mouseReleased";
    QGraphicsItemGroup::mouseReleaseEvent(event);
}

void PinWidget::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    // This will never get triggered due to a quirk in Qt
    // that does not allow us to have both default behaviour
    // (selectable parent) and doubleclickevents.
    qDebug() << "doubleclickEvent";
    handleMouseDoubleClick();
}

void PinWidget::handleMouseDoubleClick()
{
    assert(this->scene() != 0);
    if(this->scene() == 0)
    {
        return;
    }

    foreach(QWidget* tlw, QApplication::topLevelWidgets())
    {
        MainWindow* mw = qobject_cast<MainWindow*>(tlw);
        if(mw != 0 && mw->isAncestorOf(this->scene()->views().first()))
        {
            qDebug() << "posting PinDoubleClickEvent to " << mw;
            QCoreApplication::postEvent(mw, new PinDoubleClickedEvent(this));
        }
    }
}

/*
void PinWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);


    painter->setPen(Qt::blue);

    painter->setBrush(Qt::red);
    painter->drawRect(this->boundingRect());
}
*/
