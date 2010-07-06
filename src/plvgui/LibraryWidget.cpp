#include "LibraryWidget.h"
#include "ui_librarywidget.h"
#include "PipelineElement.h"
#include "LibraryElement.h"
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
    LibraryElement* w = new LibraryElement(name, this);
    ui->container->addWidget(w);
}

void plvgui::LibraryWidget::on_pushButton_clicked()
{
    addItem("test");
}
