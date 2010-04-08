#include "MainWindow.h"
#include "ui_mainwindow.h"

using namespace plvgui;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::addWidget(QWidget *widget) {
    ui->layout->addWidget(widget);
//    ui->resize( m_camera->getWidth(), m_camera->getHeight() );
}
