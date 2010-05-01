#include "MainWindow.h"
#include "ui_mainwindow.h"

#include "LibraryWidget.h"

#include "Pipeline.h"

#include <QDebug>
#include <QSettings>
#include <QtGui>



using namespace plvgui;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_settings(new QSettings("UTwente", "ParleVision"))
{
    initGUI();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_controls_toolbar;
    delete m_settings;
}

void MainWindow::initGUI()
{
    // Load design from Form mainwindow.ui
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

    createLibraryWidget();

    // Restore window geometry and state
    loadSettings();
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "Saving geometry info to " << m_settings->fileName();
    m_settings->setValue("MainWindow/geometry", saveGeometry());
    m_settings->setValue("MainWindow/windowState", saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::loadSettings()
{
    qDebug() << "Reading settings from " << m_settings->fileName();
    qDebug() << restoreGeometry(m_settings->value("MainWindow/geometry").toByteArray());
    qDebug() << restoreState(m_settings->value("MainWindow/windowState").toByteArray());
}

void MainWindow::addWidget(QWidget *widget)
{
    ui->layout->addWidget(widget);
}

void MainWindow::createLibraryWidget()
{
    m_libraryWidget = new LibraryWidget(this);
    this->addDockWidget(Qt::LeftDockWidgetArea, m_libraryWidget);
    #ifdef Q_OS_MAC
    // Show LibraryWidget as floating window on Mac OS X
    m_libraryWidget->setFloating(true);
    #endif
}

void MainWindow::setPipeline(plv::Pipeline* pipeline)
{
    //TODO think about what to do if we already have a pipeline.
    this->m_pipeline = pipeline;

    //TODO find a way to connect to widgets here.
}

//void MainWindow::addCamera(plv::OpenCVCamera* camera)
//{
//    // note that signals are not yet ever disconnected.
//    // this will probably change anyway as we want the whole pipeline to stop
//    // and not just the cameras.
//    connect(ui->actionStop, SIGNAL(triggered()),
//            camera, SLOT(release()));
//    connect(this->m_stopAction, SIGNAL(triggered()),
//            camera, SLOT(release()));
//
//    connect(ui->actionStart, SIGNAL(triggered()),
//            camera, SLOT(start()));
//    connect(this->m_startAction, SIGNAL(triggered()),
//            camera, SLOT(start()));
//
//    connect(ui->actionPause, SIGNAL(triggered()),
//            camera, SLOT(pause()));
//    connect(this->m_pauseAction, SIGNAL(triggered()),
//            camera, SLOT(pause()));
//}
