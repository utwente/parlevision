#ifndef PIPELINEELEMENTWIDGET_H
#define PIPELINEELEMENTWIDGET_H

#include <QGraphicsWidget>
#include <QString>

class QVariant;

namespace plv {
    class PipelineElement;
}

namespace plvgui {
    class ConnectionLine;

    class PipelineElementWidget : public QGraphicsWidget
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
    };
}

#endif // PIPELINEELEMENTWIDGET_H
