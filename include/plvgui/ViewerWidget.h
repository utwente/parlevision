#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include <QDockWidget>
#include "RefPtr.h"

namespace Ui
{
    class ViewerWidget;
}

namespace plv
{
    class IOutputPin;
}

using namespace plv;

namespace plvgui
{
    class ViewerWidget : public QDockWidget
    {
        Q_OBJECT

    public:
        explicit ViewerWidget(IOutputPin* pin, QWidget *parent = 0);
        ~ViewerWidget();

    protected:
        void changeEvent(QEvent *e);

    private:
        Ui::ViewerWidget *ui;
        RefPtr<IOutputPin> pin;
    };
}
#endif // VIEWERWIDGET_H
