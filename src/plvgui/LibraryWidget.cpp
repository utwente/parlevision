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
        addItem(QString(*iter));
    }
}

LibraryWidget::~LibraryWidget()
{
    delete ui;
}

void LibraryWidget::addItem(QString name)
{
    QWidget* w = new QLabel(name, this);
    w->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    w->setStyleSheet("background: red;");
    w->setMinimumSize(100,40);
    w->setMaximumSize(100,40);
//        w->adjustSize();
    w->resize(100,40);
    ui->container->addWidget(w);
}

void plvgui::LibraryWidget::on_pushButton_clicked()
{
    addItem("test");
}
