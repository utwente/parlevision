#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

namespace plvgui {

    class MainWindow : public QMainWindow {
        Q_OBJECT
    public:
        MainWindow(QWidget *parent = 0);
        ~MainWindow();
        void addWidget(QWidget *widget);

    protected:
        void changeEvent(QEvent *e);

    private:
        Ui::MainWindow *ui;
    };

} // namespace plvgui

#endif // MAINWINDOW_H
