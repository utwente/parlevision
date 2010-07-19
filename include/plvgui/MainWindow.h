#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include "PipelineElement.h"
#include "RefPtr.h"

class QToolbar;
class QGraphicsView;

namespace Ui {
    class MainWindow;
}

namespace plv {
    class Pipeline;
}

namespace plvgui {
    class LibraryWidget;
    class InspectorWidget;
    class PipelineScene;

    class MainWindow : public QMainWindow {
        Q_OBJECT
    public:
        MainWindow(QWidget* parent = 0);
        ~MainWindow();
        void addWidget(QWidget* widget);
        /** Set the pipeline for this window
          * This can only be done once for every MainWindow.
          * If you need to load another pipeline, make a new MainWindow.
          */
        void setPipeline(plv::Pipeline* pipeline);
        /** Loads pipeline from fileName and opens it in this window
          * or a new window if this window already had a pipeline
          */
        void loadFile(QString fileName);

    public slots:
        void addRenderersForPins(plv::RefPtr<plv::PipelineElement>);

    protected:
        void changeEvent(QEvent* e);
        void closeEvent(QCloseEvent *event);

        /** Loads Window geometry and state of docked widgets etc.
          * And restores it.
          */
        void loadSettings();
        void updateRecentFileActions();
        LibraryWidget* m_libraryWidget;
        InspectorWidget* m_inspectorWidget;
        plv::RefPtr<plv::Pipeline> m_pipeline;

    private:
        void initGUI();
        void createLibraryWidget();
        void createInspectorWidget();
        void createRecentFileActs();
        // set the file belonging to the active pipeline;
        // empty string means no file
        void setCurrentFile(QString fileName);
        // create a new window
        MainWindow* newWindow();

        Ui::MainWindow* ui;
        PipelineScene* m_scene;
        bool m_documentChanged;
        QString m_fileName;
        QAction* m_recentFilesSeparator;
        enum { MaxRecentFiles = 5 };
        QAction* recentFileActs[MaxRecentFiles];

    private slots:
        // indicate the active document has changed
        void documentChanged();
        void openRecentFile();
        void on_actionDelete_triggered();
        void on_actionNew_triggered();
        void on_actionLoad_triggered();
        void on_actionShow_Library_toggled(bool);
        void sceneSelectionChanged();
};

} // namespace plvgui

#endif // MAINWINDOW_H
