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
    };
}

#endif // LIBRARYWIDGET_H
