#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include "OpenCVCamera.h"
#include "RefPtr.h"

class QToolbar;
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

    protected:
        void changeEvent(QEvent* e);
        void closeEvent(QCloseEvent *event);

        /** Loads Window geometry and state of docked widgets etc.
          * And restores it.
          */
        void loadSettings();
        QToolBar* m_controls_toolbar;
        QAction* m_startAction;
        QAction* m_stopAction;
        QAction* m_pauseAction;
        LibraryWidget* m_libraryWidget;
        plv::RefPtr<plv::Pipeline> m_pipeline;


    private:
        Ui::MainWindow* ui;
        QSettings* m_settings;
        void initGUI();
        void createLibraryWidget();
    };

} // namespace plvgui

#endif // MAINWINDOW_H
