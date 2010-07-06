#ifndef PIPELINEVIEW_H
#define PIPELINEVIEW_H

#include <QGraphicsView>
#include "RefPtr.h"

namespace plv
{
    class Pipeline;
}

namespace plvgui
{
    class PipelineScene;

    class PipelineView : public QGraphicsView
    {
    public:
        PipelineView(QWidget *parent = 0);
        void setScene(PipelineScene* scene);

    protected:
        void dragEnterEvent(QDragEnterEvent *event);
        void dragMoveEvent(QDragMoveEvent *event);
        void dropEvent(QDropEvent *event);

    private:
        plv::RefPtr<plv::Pipeline> m_pipeline;
    };
}

#endif // PIPELINEVIEW_H
