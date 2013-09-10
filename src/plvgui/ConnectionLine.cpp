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

#include "ConnectionLine.h"

#include <QtGui>
#include <QDebug>

#include "PinWidget.h"
#include <plvcore/PinConnection.h>

const qreal Pi = 3.14;

using namespace plv;
using namespace plvgui;

ConnectionLine::ConnectionLine(PinWidget* fromPin,
                               PinWidget* toPin,
                               PinConnection* connection,
                               QGraphicsItem *parent,
                               QGraphicsScene *scene)
                                   : QGraphicsLineItem(parent, scene),
                                   fromPin(fromPin),
                                   toPin(toPin),
                                   connection(connection)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    updatePosition();
}

ConnectionLine::~ConnectionLine()
{
}

QPainterPath ConnectionLine::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
//    path.addPolygon(arrowHead);
    return path;
}

void ConnectionLine::updatePosition()
{
    QLineF line(mapFromItem(fromPin, 0, 0), mapFromItem(toPin, 0, 0));
    setLine(line);
}

QRectF ConnectionLine::boundingRect() const
{
    qreal extra = (pen().width() - 10) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
 }


void ConnectionLine::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem* style,
                           QWidget * w)
{
    Q_UNUSED(style)
    Q_UNUSED(w)

    const PinCircle* fromCircle = fromPin->getCircle();
    const PinCircle* toCircle = toPin->getCircle();

    QLineF line(fromCircle->mapToScene( fromCircle->getCenter() ),
                toCircle->mapToScene( toCircle->getCenter() ));

    // draw queue length at middle of line position
    QPointF middle = line.pointAt( 0.5 );
    int size = connection->size();
    QString sizeStr;
    sizeStr.setNum( size );
    painter->drawText( middle, sizeStr );

    setLine(line);

    if(isSelected())
    {
        QPen dashPen = this->pen();
        dashPen.setStyle(Qt::DotLine);
        dashPen.setWidth(5);
        painter->setPen(dashPen);
        painter->drawLine(line);

        QPen whitePen = this->pen();
        whitePen.setColor(Qt::white);
        painter->setPen(whitePen);
        painter->drawLine(line);
    }
    else
    {
        painter->setPen(this->pen());
    }

    painter->drawLine(line);
}
