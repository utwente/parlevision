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

QT_FORWARD_DECLARE_CLASS( QGraphicsPolygonItem )
QT_FORWARD_DECLARE_CLASS( QGraphicsLineItem )
QT_FORWARD_DECLARE_CLASS( QGraphicsScene )
QT_FORWARD_DECLARE_CLASS( QRectF )
QT_FORWARD_DECLARE_CLASS( QGraphicsSceneMouseEvent )
QT_FORWARD_DECLARE_CLASS( QPainter )
QT_FORWARD_DECLARE_CLASS( QPainterPath )
QT_FORWARD_DECLARE_CLASS( QStyleOptionGraphicsItem )

namespace plv
{
    class PinConnection;
}

namespace plvgui
{
    class PinWidget;

    /** A line that represents the connection between two pins.
      */
    class ConnectionLine : public QGraphicsLineItem
    {

    public:
        ConnectionLine(PinWidget* fromPin, PinWidget* toPin,
                       plv::PinConnection* connection,
                       QGraphicsItem *parent,
                       QGraphicsScene *scene);
        virtual ~ConnectionLine();

        QPainterPath shape() const;
        QRectF boundingRect() const;

        inline plv::RefPtr<plv::PinConnection> getPinConnection() const { return connection; }

        void updatePosition();

    protected:
        void paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget = 0);

    private:
        PinWidget* fromPin;
        PinWidget* toPin;
        plv::RefPtr<plv::PinConnection> connection;
    };
}
#endif // CONNECTIONLINE_H
