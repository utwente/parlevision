#ifndef PIPELINEELEMENTWIDGET_H
#define PIPELINEELEMENTWIDGET_H

#include <QGraphicsObject>
#include <QString>

#include "RefPtr.h"

class QGraphicsSceneDragDropEvent;
class QVariant;

namespace plv {
    class PipelineElement;
}

namespace plvgui {
    class ConnectionLine;

    class PipelineElementWidget : public QGraphicsObject
    {
    public:
        PipelineElementWidget(plv::PipelineElement* element,
                              QGraphicsItem* parent = 0,
                              Qt::WindowFlags wFlags = 0);

        void addLine(ConnectionLine* line, QString pin);

        virtual QRectF boundingRect() const;

    protected:
        QVariant itemChange(GraphicsItemChange change,
                             const QVariant &value);

        QList<ConnectionLine*> lines;
        plv::RefPtr<plv::PipelineElement> element;

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
        virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
        void addToGroup(QGraphicsItem* item);
    };
}

#endif // PIPELINEELEMENTWIDGET_H
