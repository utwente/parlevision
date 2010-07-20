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
        void addToGroup(QGraphicsItem* item);
        QHash<const plv::Pin*, PinWidget*> pinWidgets;
    };
}

#endif // PIPELINEELEMENTWIDGET_H
