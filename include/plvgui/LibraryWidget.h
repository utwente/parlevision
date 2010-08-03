#ifndef LIBRARYWIDGET_H
#define LIBRARYWIDGET_H

#include <QDockWidget>

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

    class LibraryWidget : public QDockWidget
    {
        Q_OBJECT

    public:
        explicit LibraryWidget(QWidget *parent = 0);
        ~LibraryWidget();

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
        void addItem(QString name);
        QString infoFor(plv::PipelineElement*);
        LibraryElement* draggedElement;

    };
}

#endif // LIBRARYWIDGET_H
