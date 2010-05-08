#ifndef PIPELINESCENE_H
#define PIPELINESCENE_H

#include <QGraphicsScene>
#include "RefPtr.h"

class QObject;

namespace plv {
    class Pipeline;
}

namespace plvgui {

    /** A QGraphicsScene that wraps a Pipeline
      * to provide a graphical canvas for all the elements to live on.
      * This is only the model, it needs a QGraphicsView to render.
      */

    class PipelineScene : public QGraphicsScene
    {
    public:
        PipelineScene(plv::Pipeline* pipeline, QObject* parent);

    private:
        plv::RefPtr<plv::Pipeline> m_pipeline;
    };
}

#endif // PIPELINESCENE_H
