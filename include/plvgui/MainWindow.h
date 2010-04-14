#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include "OpenCVCamera.h"

namespace Ui {
    class MainWindow;
}

namespace plvgui {

    class MainWindow : public QMainWindow {
        Q_OBJECT
    public:
        MainWindow(QWidget* parent = 0);
        ~MainWindow();
        void addWidget(QWidget* widget);
        void addCamera(plv::OpenCVCamera* camera);

    protected:
        void changeEvent(QEvent* e);
        QToolBar* m_controls_toolbar;
        QAction* m_startAction;
        QAction* m_stopAction;
        QAction* m_pauseAction;

    private:
        Ui::MainWindow* ui;
        void initGUI();
    };

} // namespace plvgui

#endif // MAINWINDOW_H
