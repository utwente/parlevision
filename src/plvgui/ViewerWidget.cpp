#include "include/plvgui/ViewerWidget.h"
#include "ui_viewerwidget.h"

ViewerWidget::ViewerWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ViewerWidget)
{
    ui->setupUi(this);
}

ViewerWidget::~ViewerWidget()
{
    delete ui;
}

void ViewerWidget::changeEvent(QEvent *e)
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
