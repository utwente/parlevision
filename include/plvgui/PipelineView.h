#ifndef PIPELINEVIEW_H
#define PIPELINEVIEW_H

#include <QGraphicsView>

namespace plvgui
{
    class PipelineView : public QGraphicsView
    {
    public:
        PipelineView(QWidget *parent = 0);

    protected:
        void dragEnterEvent(QDragEnterEvent *event);
        void dragMoveEvent(QDragMoveEvent *event);
        void dropEvent(QDropEvent *event);
    };
}

#endif // PIPELINEVIEW_H
