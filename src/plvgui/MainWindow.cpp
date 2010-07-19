#include "MainWindow.h"
#include "ui_mainwindow.h"

#include "LibraryWidget.h"
#include "InspectorWidget.h"
#include "DataRenderer.h"
#include "RendererFactory.h"

#include "Pipeline.h"
#include "PipelineScene.h"
#include "PipelineElement.h"
#include "Pin.h"
#include "PinConnection.h"
#include "PipelineLoader.h"

#include <QDebug>
#include <QSettings>
#include <QtGui>

#include <list>

using namespace plvgui;
using namespace plv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_documentChanged(false),
    m_fileName("")
{
    setAttribute(Qt::WA_DeleteOnClose);
    initGUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initGUI()
{
    // Load design from Form mainwindow.ui
    ui->setupUi(this);
    setUnifiedTitleAndToolBarOnMac(true);

    ui->view->setAcceptDrops(true);

    createLibraryWidget();
    createInspectorWidget();

    // Restore window geometry and state
    loadSettings();

    createRecentFileActs();
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    case QEvent::ActivationChange:
        if(this->isActiveWindow() || this->m_libraryWidget->isActiveWindow())
        {
//            qDebug() << this << " activated";
        }
        else
        {
//            qDebug() << this << " went to background";
        }
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if( m_pipeline.isNotNull() )
    {
        qDebug() << "Stopping pipeline...";
        m_pipeline->stop();
        m_pipeline->clear();
    }
    QSettings settings;
    qDebug() << "Saving geometry info to " << settings.fileName();
    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/windowState", saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::loadSettings()
{
    QSettings settings;
    qDebug() << "Reading settings from " << settings.fileName();
    try
    {
        restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
        restoreState(settings.value("MainWindow/windowState").toByteArray());
    }
    catch(...)
    {
        qWarning() << "Settings corrupt. Clearing.";
        settings.clear();
    }
}

void MainWindow::setCurrentFile(QString fileName)
{
    this->m_fileName = fileName;

    if(fileName.isEmpty())
    {
        return;
    }

    setWindowFilePath(fileName);

    // Load, update and save the list of recent files
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
    {
        files.removeLast();
    }

    settings.setValue("recentFileList", files);

    // update all windows
    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin)
        {
            mainWin->updateRecentFileActions();
        }
    }
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i)
    {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
    {
        recentFileActs[j]->setVisible(false);
    }

    m_recentFilesSeparator->setVisible(numRecentFiles > 0);
}

void MainWindow::addWidget(QWidget *widget)
{
    widget->setMaximumSize(320,240);
    ui->utilityContainer->addWidget(widget);
}

void MainWindow::createLibraryWidget()
{
    m_libraryWidget = new LibraryWidget(this);
    this->addDockWidget(Qt::LeftDockWidgetArea, m_libraryWidget);
    #ifdef Q_OS_MAC
    // Show LibraryWidget as floating window on Mac OS X
    m_libraryWidget->setFloating(true);
    #endif

    ui->actionShow_Library->setChecked(m_libraryWidget->isVisible());
    connect(m_libraryWidget, SIGNAL(visibilityChanged(bool)),
                                    this, SLOT(updateLibraryVisibility(bool)));
}

void MainWindow::createInspectorWidget()
{
    m_inspectorWidget = new InspectorWidget(this);
    this->addDockWidget(Qt::RightDockWidgetArea, m_inspectorWidget);
    #ifdef Q_OS_MAC
    // Show LibraryWidget as floating window on Mac OS X
    m_inspectorWidget->setFloating(true);
    #endif
}

void MainWindow::createRecentFileActs()
{
    m_recentFilesSeparator = ui->menuFile->addSeparator();

    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        ui->menuFile->addAction(recentFileActs[i]);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    updateRecentFileActions();
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        loadFile(action->data().toString());
}

void MainWindow::setPipeline(plv::Pipeline* pipeline)
{
    //TODO throw exception as well
    assert(this->m_pipeline.isNull());

    this->m_pipeline = pipeline;
    m_documentChanged = true;

    assert (ui->view != 0);
    this->m_scene = new PipelineScene(pipeline, ui->view);

    connect(m_scene, SIGNAL(selectionChanged()),
            this, SLOT(sceneSelectionChanged()));

    ui->view->setScene(m_scene);

    connect(ui->actionStop, SIGNAL(triggered()),
            pipeline, SLOT(stop()));

    connect(ui->actionStart, SIGNAL(triggered()),
            pipeline, SLOT(start()));

    connect(pipeline, SIGNAL(elementAdded(plv::RefPtr<plv::PipelineElement>)),
            this, SLOT(addRenderersForPins(plv::RefPtr<plv::PipelineElement>)));

    connect(pipeline, SIGNAL(elementAdded(plv::RefPtr<plv::PipelineElement>)),
            this, SLOT(documentChanged()));
    connect(pipeline, SIGNAL(elementChanged(plv::RefPtr<plv::PipelineElement>)),
            this, SLOT(documentChanged()));
    connect(pipeline, SIGNAL(elementRemoved(plv::RefPtr<plv::PipelineElement>)),
            this, SLOT(documentChanged()));
    connect(pipeline, SIGNAL(connectionAdded(plv::RefPtr<plv::PinConnection>)),
            this, SLOT(documentChanged()));
    connect(pipeline, SIGNAL(connectionChanged(plv::RefPtr<plv::PinConnection>)),
            this, SLOT(documentChanged()));
    connect(pipeline, SIGNAL(connectionRemoved(plv::RefPtr<plv::PinConnection>)),
            this, SLOT(documentChanged()));

    connect(m_scene, SIGNAL(changed(QList<QRectF>)),
            this, SLOT(documentChanged()));

    // add renderers for all elements in the pipeline
    std::list< RefPtr<PipelineElement> > elements = pipeline->getChildren();
    for( std::list< RefPtr<PipelineElement> >::iterator itr = elements.begin()
        ; itr != elements.end(); ++itr )
    {
        this->addRenderersForPins(*itr);
    }

}

void MainWindow::loadFile(QString fileName)
{

    if(this->m_pipeline)
    {
        // already had an open pipeline, open new window
        MainWindow* other = newWindow();
        other->loadFile(fileName);
        return;
    }

    try
    {
        // this window did not yet have a pipeline loaded yet
        RefPtr<Pipeline> pl = PipelineLoader::parsePipeline(fileName);
        bool state = pl->init();
        assert(state);
        this->setCurrentFile(fileName);
        this->setPipeline(pl);

    }
    catch( std::runtime_error& e )
    {
        qDebug() << "Pipeline loading failed with: " << e.what();
        return;
    }
    catch( ... )
    {
        qDebug() << "Caught unknown exception.";
        return;
    }
}

MainWindow* MainWindow::newWindow()
{
    MainWindow *other = new MainWindow();
    other->move(x() + 40, y() + 40);
    other->show();
    return other;
}

void MainWindow::addRenderersForPins(plv::RefPtr<plv::PipelineElement> element)
{
    qDebug() << "Adding renderers for " << element->getName();
    //this is temporary
    std::list< RefPtr<IOutputPin> >* outPins = element->getOutputPins();

    for(std::list< RefPtr<IOutputPin> >::iterator itr = outPins->begin();
        itr != outPins->end();
        ++itr)
    {
        RefPtr<IOutputPin> pin = *itr;

        assert(pin.isNotNull());
        qDebug() << "Adding renderer for Pin " << pin->getName();

        DataRenderer* renderer = RendererFactory::create(pin->getTypeInfo().name(), this);
        renderer->setPin(pin);

        this->addWidget(renderer);
    }

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

void plvgui::MainWindow::on_actionShow_Library_toggled(bool on)
{
    if(on)
    {
        this->m_libraryWidget->show();
    }
    else
    {
        this->m_libraryWidget->hide();
    }
}

void MainWindow::updateLibraryVisibility(bool visible)
{
    ui->actionShow_Library->setChecked(visible);
}


void plvgui::MainWindow::on_actionLoad_triggered()
{
    /*
    QString fileName = QFileDialog::getOpenFileName(this,
                            tr("Open Pipeline"),
                            "",
                            tr("ParleVision Pipeline (*.plv *.pipeline)"));
    if(filenName != "")
    {
        qDebug() << "User selected " << fileName;
        loadFile(fileName);
    }
    */

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("ParleVision Pipeline (*.plv *.pipeline)"));

    if (dialog.exec())
    {
        QString fileName = dialog.selectedFiles().first();

        qDebug() << "User selected " << fileName;
        loadFile(fileName);
    }
}

void plvgui::MainWindow::on_actionNew_triggered()
{
    newWindow();
}

void MainWindow::documentChanged()
{
    m_documentChanged = true;
    ui->actionSave->setEnabled(true);
}

void plvgui::MainWindow::on_actionDelete_triggered()
{
    if(this->m_scene)
    {
        this->m_scene->deleteSelected();
    }
}

void plvgui::MainWindow::sceneSelectionChanged()
{
    ui->actionDelete->setEnabled(this->m_scene->selectedItems().size() > 0);
}
