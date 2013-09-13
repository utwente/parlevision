#include "AboutDialog.h"
#include "ui_aboutdialog.h"

using namespace plvgui;

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::setText(const QString& text)
{
    this->ui->textLabel->setText(text);
}
