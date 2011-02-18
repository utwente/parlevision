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

#include <plvcore/RefPtr.h>
#include <plvcore/PipelineElement.h>

QT_BEGIN_NAMESPACE
class QGraphicsSceneDragDropEvent;
class QVariant;
class QLabel;
class QGraphicsTextItem;
QT_END_NAMESPACE

namespace plv
{
    class Pin;
    class PipelineElement;
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

        inline PinWidget* getWidgetFor(const plv::IInputPin* p) const { return inputPinWidgets[p]; }
        inline PinWidget* getWidgetFor(const plv::IOutputPin* p) const { return outputPinWidgets[p]; }
        inline plv::RefPtr<plv::PipelineElement> getElement() const { return element; }

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

    private:
        QHash<const plv::IInputPin*, PinWidget*> inputPinWidgets;
        QHash<const plv::IOutputPin*, PinWidget*> outputPinWidgets;
        qreal maxWidth;
        qreal leftColumnWidth;
        QGraphicsTextItem* avgProcessingTimeLabel;
        QGraphicsTextItem* lastProcessingTimeLabel;
        QString avgTimeString;
        QString lastTimeString;
        QGraphicsTextItem* titleLabel;

        void addToGroup(QGraphicsItem* item);
        void addInputPin(plv::IInputPin* in);
        void addOutputPin(plv::IOutputPin* out);
        void drawPinsAndInfo();

    private slots:
        void savePositionProperties();
        void onError(PlvErrorType,plv::PipelineElement*);
        void onProcessingTimeUpdate(int,int);
    };
}

#endif // PIPELINEELEMENTWIDGET_H
