/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvgui module of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

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

    this->circle = new PinCircle(0,0,20,20,this);

    this->circle->translate(0, 4.0);

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
//    qDebug() << "SceneEvent: " << event;
    return QGraphicsItemGroup::sceneEvent(event);
}

void PinWidget::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
//    qDebug() << "hovering above " << this->getPin()->getName();
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
//        qDebug() << "clicked on PinWidget, but not inside circle ";
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

void PinWidget::mouseDoubleClickEvent(QGraphicsSceneMouseEvent*)
{
    // This will never get triggered due to a quirk in Qt
    // that does not allow us to have both default behaviour
    // (selectable parent) and doubleclickevents.
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

PinCircle::PinCircle(qreal x, qreal y, qreal width, qreal height, PinWidget* parent)
    : QGraphicsEllipseItem(x+(width-7)/2.0, y+(height-7)/2.0, 7, 7, parent),
    m_pin(parent->getPin()),
    m_width(width),
    m_height(height)
{
}

QPainterPath PinCircle::shape() const
{
    QPainterPath path;
    path.addRect(0, 0, m_width,m_height);
    return path;
}

QPointF PinCircle::getCenter() const
{
    return QPointF(m_width/2.0, m_height/2.0);
}
