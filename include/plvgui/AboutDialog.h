#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
    class AboutDialog;
}

namespace plvgui {

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

    void setText(const QString& text);

private:
    Ui::AboutDialog *ui;
};

}

#endif // ABOUTDIALOG_H
