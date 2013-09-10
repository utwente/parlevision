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

#include "PipelineElementWidget.h"

#include <list>
#include <algorithm>
#include <QtGui>

#include <plvcore/PipelineElement.h>
#include <plvcore/DataConsumer.h>
#include <plvcore/DataProducer.h>
#include <plvcore/Pin.h>
#include <plvcore/IOutputPin.h>
#include <plvcore/IInputPin.h>
#include <plvcore/RefPtr.h>

#include "ConnectionLine.h"
#include "PinWidget.h"

using namespace plvgui;
using namespace plv;
using namespace std;

PipelineElementWidget::PipelineElementWidget(PipelineElement* e,
                                             QGraphicsItem* parent,
                                             Qt::WindowFlags wFlags) :
        QGraphicsObject(parent),
        element(e),
        maxWidth(0),
        leftColumnWidth(0)
{
    Q_UNUSED(wFlags);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);

    connect(this, SIGNAL(xChanged()), this, SLOT(savePositionProperties()));
    connect(this, SIGNAL(yChanged()), this, SLOT(savePositionProperties()));

    connect(this, SIGNAL( plePropertyChanged(const char*,QVariant)),
            element, SLOT(setProperty(const char*,QVariant)));

    //connect(element, SIGNAL(onStateChange(PipelineElement::State)), this, SLOT(onStateChange(PipelineElement::State)));
    connect( element, SIGNAL(onError(PlvErrorType,plv::PipelineElement*)), this, SLOT(onError(PlvErrorType,plv::PipelineElement*)));
    connect( element, SIGNAL(onProcessingTimeUpdate(int,int)), this, SLOT(onProcessingTimeUpdate(int,int)));

    titleLabel = new QGraphicsTextItem(element->getName(), this);
    this->addToGroup(titleLabel);
    this->leftColumnWidth = titleLabel->boundingRect().width()/2.0;

    avgProcessingTimeLabel  = new QGraphicsTextItem(this);
    lastProcessingTimeLabel = new QGraphicsTextItem(this);
    onProcessingTimeUpdate(0,0);
    avgProcessingTimeLabel->setPlainText(this->avgTimeString);
    lastProcessingTimeLabel->setPlainText(this->lastTimeString);
    this->addToGroup(avgProcessingTimeLabel);
    this->addToGroup(lastProcessingTimeLabel);

    // add all inputpins
    if( element->isDataConsumer() )
    {
        DataConsumer* dc = dynamic_cast<DataConsumer*>(element.getPtr());
        assert(dc != 0);
        if( dc != 0 )
        {
            const InputPinMap& inPins = dc->getInputPins();
            for( InputPinMap::const_iterator itr = inPins.begin()
                ; itr!=inPins.end(); ++itr)
            {
                this->addInputPin(itr.value());
            }
        }

        connect( element, SIGNAL(inputPinAdded(plv::IInputPin*)), this, SLOT(inputPinAdded(plv::IInputPin*)));
        connect( element, SIGNAL(inputPinRemoved(int)), this, SLOT(inputPinRemoved(int)) );
    }

    if( element->isDataProducer() )
    {
        DataProducer* dp = dynamic_cast<DataProducer*>(element.getPtr());
        assert(dp != 0);
        if( dp != 0 )
        {
            const OutputPinMap& outPins = dp->getOutputPins();
            for( OutputPinMap::const_iterator itr = outPins.begin()
                ; itr!=outPins.end(); ++itr)
            {
                this->addOutputPin(itr.value());
            }
        }

        connect( element, SIGNAL(outputPinAdded(plv::IOutputPin*)), this, SLOT(outputPinAdded(plv::IOutputPin*)));
        connect( element, SIGNAL(outputPinRemoved(int)), this, SLOT(outputPinRemoved(int)) );
    }
    setFiltersChildEvents(false);
    drawPinsAndInfo();
}

PinWidget* PipelineElementWidget::getWidgetFor(const plv::IInputPin* p) const
{
    if( !inputPinWidgets.contains(p->getId()))
    {
        qWarning() << "PipelineElementWidget::getWidgetFor(const plv::IInputPin* p) widget not found";
        return 0;
    }
    return inputPinWidgets.value(p->getId());
}

PinWidget* PipelineElementWidget::getWidgetFor(const plv::IOutputPin* p) const
{
    if( !outputPinWidgets.contains(p->getId()))
    {
        qWarning() << "PipelineElementWidget::getWidgetFor(const plv::IOutputPin* p) widget not found";
        return 0;
    }
    return outputPinWidgets.value(p->getId());
}

void PipelineElementWidget::addInputPin(IInputPin* in)
{
    assert(in!=0);
    PinWidget* pw = new InputPinWidget(this, in);
    this->inputPinWidgets[in->getId()] = pw;
    this->addToGroup(pw);
    this->leftColumnWidth = max(pw->boundingRect().width(), this->leftColumnWidth);
}

void PipelineElementWidget::removeInputPin(int id)
{
    assert( inputPinWidgets.contains(id) );
    PinWidget* pw = inputPinWidgets.value(id);
    pw->setParent(0);
    this->inputPinWidgets.remove(id);
    delete pw;
}

void PipelineElementWidget::addOutputPin(IOutputPin* out)
{
    assert(out!=0);
    PinWidget* pw = new OutputPinWidget(this, out);
    this->outputPinWidgets[out->getId()] = pw;
    this->addToGroup(pw);
    this->maxWidth = max(pw->boundingRect().width(), this->maxWidth);
}

void PipelineElementWidget::removeOutputPin(int id)
{
    this->outputPinWidgets.remove(id);
}

void PipelineElementWidget::drawPinsAndInfo()
{
    // draw all inputpins
    int y = 20;
    foreach( PinWidget* pw, this->inputPinWidgets )
    {
        pw->setPos(0, y);
        y+=20;
    }
    int maxy = y;

    // outputpins should be aligned right.
    // in order to do this, we first make all the widgets,
    // then add them with proper translation.

    y = 20;
    foreach(PinWidget* pw, outputPinWidgets)
    {
        qreal offset = leftColumnWidth + 20 + maxWidth - pw->boundingRect().width();
        pw->setPos(offset, y);
        y+=20;
    }
    if( y > maxy ) maxy = y;

    avgProcessingTimeLabel->setPlainText(avgTimeString);
    avgProcessingTimeLabel->setPos(0,maxy);
    maxy += 20;
    lastProcessingTimeLabel->setPlainText(lastTimeString);
    lastProcessingTimeLabel->setPos(0,maxy);

    // center the title
    qreal offset = (this->boundingRect().width()-titleLabel->boundingRect().width())/2.0;
    if(offset > 0)
        this->titleLabel->setPos(offset,0);
}

void PipelineElementWidget::addLine(ConnectionLine *line, QString pin)
{
    Q_UNUSED(pin);
    this->lines.append(line);
}

void PipelineElementWidget::removeLine(ConnectionLine *line, QString pin)
{
    Q_UNUSED(pin);
    this->lines.removeAll(line);
}

QRectF PipelineElementWidget::boundingRect() const
{
    return childrenBoundingRect();
}

QVariant PipelineElementWidget::itemChange(GraphicsItemChange change,
                     const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange)
    {
        foreach (ConnectionLine* line, lines)
        {
            line->updatePosition();
        }
    }
    return value;
}

void PipelineElementWidget::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    QGraphicsItem::mousePressEvent(event);
}

void PipelineElementWidget::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    foreach(QGraphicsItem* child, this->childItems())
    {
        if(child->contains(child->mapFromParent(event->pos())))
        {
            PinWidget* pw = dynamic_cast<PinWidget*> (child);
            if(pw)
            {
                pw->handleMouseDoubleClick();
            }
        }
    }
}

bool PipelineElementWidget::event(QEvent * event)
{
    return QGraphicsObject::event(event);
}

void PipelineElementWidget::addToGroup(QGraphicsItem* item)
{
    item->setParentItem(this);
    prepareGeometryChange();
    update();
}

void PipelineElementWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    // just repaint the changed part
    painter->setClipRect( option->exposedRect );

    if(this->isSelected())
    {
        //TODO move this outside paint
        QPen pen;
        pen.setStyle(Qt::DashDotLine);
        pen.setWidth(3);
        pen.setColor(Qt::black);
        painter->setPen(pen);
    }
    else
    {
        painter->setPen(Qt::black);
    }

    // use different colors to show user if processor is connected
    // or if required connections are missing.
    if( element->requiredPinsConnected() )
    {
        painter->setBrush(Qt::green);
    }
    else
    {
        painter->setBrush(Qt::red);
    }
    painter->drawRoundedRect(this->boundingRect(),8,8, Qt::AbsoluteSize);
}

void PipelineElementWidget::savePositionProperties()
{
    // causes an error Cannot send events to objects owned by a different thread
    //    this->element->setProperty("sceneCoordX", );
    //    this->element->setProperty("sceneCoordY", this->scenePos().y());

    emit plePropertyChanged("sceneCoordX", this->scenePos().x());
    emit plePropertyChanged("sceneCoordY", this->scenePos().y());
}

void PipelineElementWidget::onError(PlvErrorType,PipelineElement*)
{

}

void PipelineElementWidget::onProcessingTimeUpdate(int avg, int last)
{
    avgTimeString = tr("Average time: %1").arg(avg);
    lastTimeString = tr("Last time: %1").arg(last);
    avgProcessingTimeLabel->setPlainText(avgTimeString);
    lastProcessingTimeLabel->setPlainText(lastTimeString);
    this->update();
}

void PipelineElementWidget::inputPinAdded( plv::IInputPin* pin )
{
    this->addInputPin(pin);
    this->drawPinsAndInfo();
    this->update();
}

void PipelineElementWidget::outputPinAdded( plv::IOutputPin* pin )
{
    this->addOutputPin(pin);
    this->drawPinsAndInfo();
    this->update();
}

void PipelineElementWidget::inputPinRemoved( int id )
{
    this->removeInputPin(id);
    this->drawPinsAndInfo();
    this->update();
}

void PipelineElementWidget::outputPinRemoved( int id )
{
    this->removeOutputPin(id);
    this->drawPinsAndInfo();
    this->update();
}


