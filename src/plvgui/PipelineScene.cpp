#include "PipelineScene.h"

#include <QtGui>

#include "Pipeline.h"

using namespace plvgui;

PipelineScene::PipelineScene(plv::Pipeline *pipeline, QObject *parent) :
        QGraphicsScene(parent),
        m_pipeline(pipeline)
{
}
