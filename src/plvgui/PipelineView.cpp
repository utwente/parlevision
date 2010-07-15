#include "PipelineView.h"
#include <QtGui>
#include <QDebug>
#include <QMetaType>
#include "PipelineElement.h"
#include "PipelineScene.h"
#include "Pipeline.h"

using namespace plvgui;
using namespace plv;

PipelineView::PipelineView(QWidget *parent) : QGraphicsView(parent)
{
    setAcceptDrops(true);
    setRenderHint(QPainter::Antialiasing,true);
}

void PipelineView::setPipeline(Pipeline* pipeline)
{
    qDebug() << "setting pipeline to " << pipeline;
    this->m_pipeline = pipeline;
}

void PipelineView::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->provides("x-plv-element-name"))
    {
        qDebug() << "Enter";
        event->accept();
    }
}

void PipelineView::dragMoveEvent(QDragMoveEvent *event)
{
    if(event->provides("x-plv-element-name"))
    {
        qDebug() << "Move";
        event->accept();
    }
}

void PipelineView::dropEvent(QDropEvent *event)
{
    qDebug() << event->mimeData()->formats();
    if(event->provides("x-plv-element-name"))
    {
    //    qDebug() << event->mimeData()->data("x-plv-element-name");
        QString elementName = QString(event->mimeData()->data("x-plv-element-name"));
        qDebug() << elementName;

        int typeId = QMetaType::type(elementName.toAscii());

        if(typeId == 0)
            throw ElementCreationException(
                    QString("Tried to create unknown element "+elementName).toStdString());

        RefPtr<PipelineElement> pe = static_cast<PipelineElement*>(QMetaType::construct(typeId));

        if(m_pipeline.isNotNull())
        {
            m_pipeline->add(pe);
        }
    }
}
