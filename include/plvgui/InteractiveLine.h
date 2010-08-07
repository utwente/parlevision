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

#ifndef INTERACTIVELINE_H
#define INTERACTIVELINE_H

#include <QGraphicsLineItem>
#include <QObject>


//QT_BEGIN_NAMESPACE
class QGraphicsPolygonItem;
class QGraphicsLineItem;
class QGraphicsScene;
class QRectF;
class QGraphicsSceneMouseEvent;
class QPainter;
class QPainterPath;
class QStyleOptionGraphicsItem;
//QT_END_NAMESPACE


namespace plvgui
{
//    class PipelineElementWidget;
    class PinWidget;

    /** A line that represents a connection being made
      * between a Pin and the mouse.
      */
    class InteractiveLine : public QObject, public QGraphicsLineItem
    {
        Q_OBJECT

    public:
        InteractiveLine(PinWidget* fromPin,
                              QGraphicsItem *parent,
                              QGraphicsScene *scene);

        PinWidget* getFromPin() const {return this->fromPin;}

    public slots:
        void updateTarget(QPointF pos);

    protected:
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget = 0);


    private:
        PinWidget* fromPin;
        QPointF targetPos;
    };
}

#endif // INTERACTIVELINE_H
