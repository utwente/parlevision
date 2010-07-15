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
        void setPipeline(plv::Pipeline* pipeline);

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
        void documentChanged();

    private:
        void initGUI();
        void createLibraryWidget();
        // saves state of library and inspector views (e.g. whether they are shown etc.)
        void storeViewState();
        // restores state of views saved during storeViewState()
        void restoreViewState();
        // set the file belonging to the active pipeline;
        // empty string means no file
        void setCurrentFile(QString fileName) { m_fileName = fileName; }
        // indicate the document has changed

        Ui::MainWindow* ui;
        QSettings* m_settings;
        bool m_documentChanged;
        QString m_fileName;
        struct ViewState
        {
            bool libraryVisible;
            bool inspectorVisible;
        } viewState ;


private slots:
    void on_actionNew_triggered();
    void on_actionLoad_triggered();
    void on_actionShow_Library_toggled(bool);
};

} // namespace plvgui

#endif // MAINWINDOW_H
