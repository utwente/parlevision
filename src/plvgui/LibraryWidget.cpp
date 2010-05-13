#include "LibraryWidget.h"
#include "ui_librarywidget.h"

using namespace plvgui;

LibraryWidget::LibraryWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LibraryWidget)
{
    ui->setupUi(this);
}

LibraryWidget::~LibraryWidget()
{
    delete ui;
}
