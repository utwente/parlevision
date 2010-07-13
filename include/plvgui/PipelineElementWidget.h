#ifndef PIPELINEELEMENTWIDGET_H
#define PIPELINEELEMENTWIDGET_H

#include <QGraphicsItemGroup>
#include <QString>

#include "RefPtr.h"

class QGraphicsSceneDragDropEvent;
class QVariant;

namespace plv {
    class PipelineElement;
}

namespace plvgui {
    class ConnectionLine;

    class PipelineElementWidget : public QGraphicsItemGroup
    {
    public:
        PipelineElementWidget(plv::PipelineElement* element,
                              QGraphicsItem* parent = 0,
                              Qt::WindowFlags wFlags = 0);

        void addLine(ConnectionLine* line, QString pin);

    protected:
        QVariant itemChange(GraphicsItemChange change,
                             const QVariant &value);

        QList<ConnectionLine*> lines;
        plv::RefPtr<plv::PipelineElement> element;

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
        virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    };
}

#endif // PIPELINEELEMENTWIDGET_H
