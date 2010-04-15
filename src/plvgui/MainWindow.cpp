#include "MainWindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

using namespace plvgui;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    initGUI();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_controls_toolbar;
}

void MainWindow::initGUI()
{
    ui->setupUi(this);
    setUnifiedTitleAndToolBarOnMac(true);

    QPixmap startpix = QPixmap(":/icons/play.png");
    QPixmap pausepix(":/icons/pause.png");
    QPixmap stoppix(":/icons/stop.png");


    m_startAction = new QAction(startpix, "&Start", this);
    m_startAction->setStatusTip(tr("Starts the pipeline"));

    m_pauseAction = new QAction(pausepix, "&Pause", this);
    m_pauseAction->setStatusTip(tr("Pauses the pipeline"));

    m_stopAction = new QAction(stoppix, "S&top", this);
    m_stopAction->setStatusTip(tr("Stops the pipeline"));



    m_controls_toolbar = this->addToolBar("Controls");
    m_controls_toolbar->addAction(m_startAction);
    m_controls_toolbar->addAction(m_pauseAction);
    m_controls_toolbar->addAction(m_stopAction);
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

void MainWindow::addWidget(QWidget *widget)
{
    ui->layout->addWidget(widget);
}

void MainWindow::addCamera(plv::OpenCVCamera* camera)
{
    // note that signals are not yet ever disconnected.
    // this will probably change anyway as we want the whole pipeline to stop
    // and not just the cameras.
    connect(ui->actionStop, SIGNAL(triggered()),
            camera, SLOT(release()));
    connect(this->m_stopAction, SIGNAL(triggered()),
            camera, SLOT(release()));

    connect(ui->actionStart, SIGNAL(triggered()),
            camera, SLOT(start()));
    connect(this->m_startAction, SIGNAL(triggered()),
            camera, SLOT(start()));

    connect(ui->actionPause, SIGNAL(triggered()),
            camera, SLOT(pause()));
    connect(this->m_pauseAction, SIGNAL(triggered()),
            camera, SLOT(pause()));
}
