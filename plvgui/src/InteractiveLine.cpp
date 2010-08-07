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

#include "InteractiveLine.h"

#include <QtGui>

#include <QDebug>

#include "PinWidget.h"

using namespace plv;
using namespace plvgui;

InteractiveLine::InteractiveLine(PinWidget* fromPin,
                               QGraphicsItem *parent,
                               QGraphicsScene *scene)
                                   : QObject(scene),
                                   QGraphicsLineItem(parent, scene),
                                   fromPin(fromPin),
                                   targetPos(fromPin->getCircle()->mapToScene(fromPin->getCircle()->getCenter()))
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

void InteractiveLine::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem* style,
                           QWidget * w)
{
    QLineF line(fromPin->getCircle()->mapToScene(fromPin->getCircle()->getCenter()),
                this->targetPos);
    setLine(line);
    QGraphicsLineItem::paint(painter, style, w);

}

void InteractiveLine::updateTarget(QPointF pos)
{
    this->targetPos = pos;
    update();
}
