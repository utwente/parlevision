#ifndef PIPELINEELEMENTWIDGET_H
#define PIPELINEELEMENTWIDGET_H

#include<QGraphicsWidget>

namespace plv {
    class PipelineElement;
}

namespace plvgui {
    class PipelineElementWidget : public QGraphicsWidget
    {
    public:
        PipelineElementWidget(plv::PipelineElement* element,
                              QGraphicsItem* parent = 0,
                              Qt::WindowFlags wFlags = 0);
    };
}

#endif // PIPELINEELEMENTWIDGET_H
