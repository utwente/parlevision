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
        for(int i = 0; i < 4; i++) {
        qDebug() << "LibraryWidget: Adding type " << *iter;
        QWidget* w = new QLabel(QString(*iter), this);
        w->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        w->setStyleSheet("background: red;");
        w->setMinimumSize(100,40);
        w->setMaximumSize(100,40);
//        w->adjustSize();
        w->resize(100,40);
        ui->container->addWidget(w);
        }
    }
}

LibraryWidget::~LibraryWidget()
{
    delete ui;
}
