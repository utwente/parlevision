#ifndef PIPELINESCENE_H
#define PIPELINESCENE_H

#include <QGraphicsScene>
#include <QHash>

#include "RefPtr.h"
#include "Pipeline.h"
#include "PinConnection.h"


class QObject;

namespace plv {
    class PipelineElement;
}

namespace plvgui {
    class PipelineElementWidget;

    /** A QGraphicsScene that wraps a Pipeline
      * to provide a graphical canvas for all the elements to live on.
      * This is only the model, it needs a QGraphicsView to render.
      */
    class PipelineScene : public QGraphicsScene
    {
        Q_OBJECT

    public:
        PipelineScene(plv::Pipeline* pipeline, QObject* parent);
        plv::RefPtr<plv::Pipeline> getPipeline() { return m_pipeline; }
        virtual bool event(QEvent * event);

    public slots:
        void add(plv::PipelineElement* e);
        void add(plv::RefPtr<plv::PipelineElement> e);

        void add(plv::PinConnection* c);
        void add(plv::RefPtr<plv::PinConnection> c);

    private:
        plv::RefPtr<plv::Pipeline> m_pipeline;
        QHash<plv::PipelineElement*, PipelineElementWidget*> elementWidgets;

        PipelineElementWidget* getWidgetFor(plv::PipelineElement* e);

    };

}

#endif // PIPELINESCENE_H
