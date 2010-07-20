#include "InspectorWidget.h"
#include "ui_inspectorwidget.h"

#include <QtGui>

#include "PipelineElement.h"

using namespace plvgui;
using namespace plv;

InspectorWidget::InspectorWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::InspectorWidget)
{
    ui->setupUi(this);
}

InspectorWidget::~InspectorWidget()
{
    delete ui;
}

void InspectorWidget::changeEvent(QEvent *e)
{
    QDockWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void InspectorWidget::setTarget(plv::RefPtr<plv::PipelineElement> element)
{
    this->element = element;
    qDebug() << "Now inspecting " << element;
}
