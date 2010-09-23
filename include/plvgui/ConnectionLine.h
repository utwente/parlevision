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

#ifndef CONNECTIONLINE_H
#define CONNECTIONLINE_H

#include <QGraphicsLineItem>

#include <plvcore/RefPtr.h>

using namespace plv;

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

namespace plv
{
    class PinConnection;
}

namespace plvgui
{
//    class PipelineElementWidget;
    class PinWidget;

    /** A line that represents the connection between two pins.
      */
    class ConnectionLine : public QGraphicsLineItem
    {
//        Q_OBJECT

    public:
        ConnectionLine(PinWidget* fromPin,
                              PinWidget* toPin,
                              PinConnection* connection,
                              QGraphicsItem *parent,
                              QGraphicsScene *scene);

        QPainterPath shape() const;
        QRectF boundingRect() const;
        RefPtr<PinConnection> getPinConnection() const { return connection; }

    public slots:
        void updatePosition();

    protected:
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget = 0);

    private:
        PinWidget* fromPin;
        PinWidget* toPin;
        RefPtr<PinConnection> connection;
    };
}
#endif // CONNECTIONLINE_H
