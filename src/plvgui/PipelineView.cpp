#include "PipelineView.h"
#include <QtGui>
#include <QDebug>

using namespace plvgui;

PipelineView::PipelineView(QWidget *parent) : QGraphicsView(parent)
{
    setAcceptDrops(true);
    // Comment the line below, and drop starts working
}

void PipelineView::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
    qDebug() << "Enter";
}

void PipelineView::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
    qDebug() << "Move";
}

void PipelineView::dropEvent(QDropEvent *event)
{
    qDebug() << event->mimeData()->formats();
}
