#ifndef INSPECTORWIDGET_H
#define INSPECTORWIDGET_H

#include <QDockWidget>
#include "RefPtr.h"

namespace Ui
{
    class InspectorWidget;
}

namespace plv
{
//    class PipelineElement;
}

namespace plvgui
{
    class InspectorWidget : public QDockWidget
    {
        Q_OBJECT

    public:
        explicit InspectorWidget(QWidget *parent = 0);
        ~InspectorWidget();

    public slots:
        void selectionChanged();

    protected:
        void changeEvent(QEvent *e);

    private:
        Ui::InspectorWidget *ui;
    };
}
#endif // INSPECTORWIDGET_H
