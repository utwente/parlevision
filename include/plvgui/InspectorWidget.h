#ifndef INSPECTORWIDGET_H
#define INSPECTORWIDGET_H

#include <QDockWidget>

namespace Ui {
    class InspectorWidget;
}

namespace plvgui
{
    class InspectorWidget : public QDockWidget
    {
        Q_OBJECT

    public:
        explicit InspectorWidget(QWidget *parent = 0);
        ~InspectorWidget();

    protected:
        void changeEvent(QEvent *e);

    private:
        Ui::InspectorWidget *ui;
    };
}
#endif // INSPECTORWIDGET_H
