#include "PipelineView.h"
#include <QtGui>
#include <QDebug>

using namespace plvgui;

PipelineView::PipelineView(QWidget *parent) : QGraphicsView(parent)
{
    setAcceptDrops(true);
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
    }
}
