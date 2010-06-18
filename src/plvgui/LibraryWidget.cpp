#include "LibraryWidget.h"
#include "ui_librarywidget.h"
#include "PipelineElement.h"
#include <QDebug>
#include <QtGui>

using namespace plvgui;

LibraryWidget::LibraryWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LibraryWidget)
{
    ui->setupUi(this);
    std::list<QString> types = plv::PipelineElement::types();

    for(std::list<QString>::iterator iter = types.begin();
        iter != types.end(); iter++)
    {
        qDebug() << "LibraryWidget: Adding type " << *iter;
        QWidget* w = new QLabel(QString(*iter), this);
        ui->container->addWidget(w);
    }
}

LibraryWidget::~LibraryWidget()
{
    delete ui;
}
