#include "InspectorWidget.h"
#include "ui_inspectorwidget.h"

using namespace plvgui;

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
