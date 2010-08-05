#ifndef LIBRARYWIDGET_H
#define LIBRARYWIDGET_H

#include <QDockWidget>
#include <QMultiMap>

namespace Ui {
    class LibraryWidget;
}

namespace plv
{
    class PipelineElement;
}

namespace plvgui
{
    class LibraryElement;
    class MainWindow;

    class LibraryWidget : public QDockWidget
    {
        Q_OBJECT

    public:
        explicit LibraryWidget(MainWindow* parent = 0);
        ~LibraryWidget();

    signals:
        void errorOccurred(QString);

    protected:
//         void dragEnterEvent(QDragEnterEvent *event);
//         void dragMoveEvent(QDragMoveEvent *event);
        void mousePressEvent(QMouseEvent *event);

    private slots:
        void elementPressed();
        void elementMoved();
        void elementReleased();

    private:
        Ui::LibraryWidget *ui;
        // create an item and add it to allElements
        void createItem(QString name);
        QString infoFor(plv::PipelineElement*);
        LibraryElement* draggedElement;
        QMultiMap<QString,LibraryElement*> allElements;
    };
}

#endif // LIBRARYWIDGET_H
