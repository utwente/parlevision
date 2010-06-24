#ifndef LIBRARYWIDGET_H
#define LIBRARYWIDGET_H

#include <QDockWidget>

namespace Ui {
    class LibraryWidget;
}

namespace plvgui {
    class LibraryWidget : public QDockWidget
    {
        Q_OBJECT

    public:
        explicit LibraryWidget(QWidget *parent = 0);
        ~LibraryWidget();

    private:
        Ui::LibraryWidget *ui;
        void addItem(QString name);

private slots:
    void on_pushButton_clicked();
};
}

#endif // LIBRARYWIDGET_H
