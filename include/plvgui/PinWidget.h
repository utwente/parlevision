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

#ifndef PINWIDGET_H
#define PINWIDGET_H

#include <QtGui/QGraphicsItemGroup>
#include <plvcore/RefPtr.h>
#include <plvcore/Pin.h>


class QGraphicsItem;

using namespace plv;

//namespace plv {
//    class Pin;
//    class IInputPin;
//    class IOutputPin;
//}

namespace plvgui
{
    class MainWindow;
    class PipelineElementWidget;
    class PinCircle;

    class PinWidget : public QObject, public QGraphicsItemGroup
    {
        Q_OBJECT

    public:
        PinWidget(PipelineElementWidget* parent, RefPtr<IInputPin> p);
        PinWidget(PipelineElementWidget* parent, RefPtr<IOutputPin> p);
        virtual QRectF boundingRect() const;
        RefPtr<Pin> getPin() const {return m_pin;}
        const PinCircle* getCircle() const {return circle;}
        void handleMouseDoubleClick();

    protected:
        virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
        virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
        virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
//        virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
        virtual bool sceneEvent ( QEvent * event );
//        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    private:
        PipelineElementWidget* m_parent;
        RefPtr<Pin> m_pin;
        void init(bool isInput);
        PinCircle* circle;
//        MainWindow* findMainWindow(QWidget*);
    };


    class PinCircle : public QGraphicsEllipseItem
    {
    public:
        PinCircle(qreal x, qreal y, qreal width, qreal height, PinWidget* parent);
        virtual QPainterPath shape() const;
        QPointF getCenter() const;
    private:
        RefPtr<Pin> m_pin;
        qreal m_width, m_height;
    };
}

#endif // PINWIDGET_H
