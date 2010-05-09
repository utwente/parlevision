#include "PipelineScene.h"

#include <QtGui>

#include "Pipeline.h"

using namespace plvgui;
using namespace plv;

PipelineScene::PipelineScene(plv::Pipeline* pipeline, QObject* parent) :
        QGraphicsScene(parent),
        m_pipeline(pipeline)
{
    std::list< RefPtr<PipelineElement> > elements = m_pipeline->getChildren();

    // add all elements from the pipeline to this scene
    for( std::list< RefPtr<PipelineElement> >::iterator itr = elements.begin()
        ; itr != elements.end(); ++itr )
    {
        this->add(*itr);
    }

    // make sure future additions to pipeline get added as well
    connect(m_pipeline, SIGNAL(elementAdded(plv::RefPtr<plv::PipelineElement>)),
            this, SLOT(add(plv::RefPtr<plv::PipelineElement>)));

}

void PipelineScene::add(plv::PipelineElement *e)
{
    add(RefPtr<PipelineElement>(e));
}

void PipelineScene::add(plv::RefPtr<plv::PipelineElement> e)
{
    //TODO
    qDebug() << "PipelineScene: adding element " << e->metaObject()->className();
}
