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

#ifndef PIPELINEELEMENTWIDGET_H
#define PIPELINEELEMENTWIDGET_H

#include <QGraphicsObject>
#include <QString>

#include "RefPtr.h"
#include "PipelineElement.h"

class QGraphicsSceneDragDropEvent;
class QVariant;

using namespace plv;

namespace plv
{
    class Pin;
}

namespace plvgui
{
    class ConnectionLine;
    class PinWidget;

    class PipelineElementWidget : public QGraphicsObject
    {
        Q_OBJECT
    public:
        PipelineElementWidget(plv::PipelineElement* element,
                              QGraphicsItem* parent = 0,
                              Qt::WindowFlags wFlags = 0);

        void addLine(ConnectionLine* line, QString pin);
        PinWidget* getWidgetFor(const Pin* p) const { return pinWidgets[p]; }
        plv::RefPtr<plv::PipelineElement> getElement() const { return element; }

        virtual QRectF boundingRect() const;
        virtual bool event(QEvent * event);

    protected:
        QVariant itemChange(GraphicsItemChange change,
                             const QVariant &value);

        QList<ConnectionLine*> lines;
        plv::RefPtr<plv::PipelineElement> element;

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
        virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
        virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
        void addToGroup(QGraphicsItem* item);
        QHash<const plv::Pin*, PinWidget*> pinWidgets;
    private slots:
        void savePositionProperties();
    };
}

#endif // PIPELINEELEMENTWIDGET_H
