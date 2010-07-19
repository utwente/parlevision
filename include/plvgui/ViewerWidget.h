#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include <QDockWidget>

namespace Ui {
    class ViewerWidget;
}

class ViewerWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ViewerWidget(QWidget *parent = 0);
    ~ViewerWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ViewerWidget *ui;
};

#endif // VIEWERWIDGET_H
