#ifndef PIPELINESCENE_H
#define PIPELINESCENE_H

#include <QGraphicsScene>

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
        void PipelineScene(plv::Pipeline* pipeline, QObject* parent);

    };
}

#endif // PIPELINESCENE_H
