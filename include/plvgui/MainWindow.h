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
        void updateLibraryVisibility(bool);

    protected:
        void changeEvent(QEvent* e);
        void closeEvent(QCloseEvent *event);

        /** Loads Window geometry and state of docked widgets etc.
          * And restores it.
          */
        void loadSettings();
        LibraryWidget* m_libraryWidget;
        plv::RefPtr<plv::Pipeline> m_pipeline;

    private slots:
        // indicate the active document has changed
        void documentChanged();

    private:
        void initGUI();
        void createLibraryWidget();
        // set the file belonging to the active pipeline;
        // empty string means no file
        void setCurrentFile(QString fileName) { m_fileName = fileName; }
        // create a new window
        MainWindow* newWindow();

        Ui::MainWindow* ui;
        QSettings* m_settings;
        bool m_documentChanged;
        QString m_fileName;

private slots:
    void on_actionNew_triggered();
    void on_actionLoad_triggered();
    void on_actionShow_Library_toggled(bool);
};

} // namespace plvgui

#endif // MAINWINDOW_H
