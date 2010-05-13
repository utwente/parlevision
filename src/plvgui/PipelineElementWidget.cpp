#include "PipelineElementWidget.h"

#include <QtGui>

#include "PipelineElement.h"

using namespace plvgui;
using namespace plv;

PipelineElementWidget::PipelineElementWidget(PipelineElement* element,
                                             QGraphicsItem* parent,
                                             Qt::WindowFlags wFlags) :
        QGraphicsWidget(parent,wFlags)
{
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout;

    QGraphicsProxyWidget* proxy = new QGraphicsProxyWidget(this);
    proxy->setWidget(new QLabel(element->metaObject()->className()));
    layout->addItem(proxy);

    this->setLayout(layout);
}
