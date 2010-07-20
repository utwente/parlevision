#include "InspectorWidget.h"
#include "ui_inspectorwidget.h"

#include <QtGui>
#include <list>

#include "PipelineElement.h"

using namespace plvgui;
using namespace plv;

InspectorWidget::InspectorWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::InspectorWidget)
{
    ui->setupUi(this);
    nothingSelected();
}

InspectorWidget::~InspectorWidget()
{
    delete ui;
}

void InspectorWidget::changeEvent(QEvent *e)
{
    QDockWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}    

void InspectorWidget::setTarget(plv::RefPtr<plv::PipelineElement> element)
{
    this->element = element;
    qDebug() << "Now inspecting " << element;
    std::list<QString> propertyNames;
    element->getConfigurablePropertyNames(propertyNames);
    for(std::list<QString>::iterator itr = propertyNames.begin();
        itr != propertyNames.end(); ++itr)
    {
        QString propertyName = *itr;
        qDebug() << "  property: " << propertyName;
        QVariant value = element->property(propertyName.toAscii());
        qDebug() << "  value: " << value;

        QSpinBox* spinBox;

        switch(value.type())
        {
        case QVariant::Int:
            spinBox = new QSpinBox(this);
            spinBox->setValue(value.toInt());
            ui->verticalLayout->addWidget(spinBox);
            break;
        default:
            ui->verticalLayout->addWidget(new QLabel(value.toString()));
        }
    }
    ui->statusMsg->setText(element->getName());
}

void InspectorWidget::nothingSelected()
{
    clearSelection();
    ui->statusMsg->setText(tr("Nothing selected"));
}

void InspectorWidget::multipleSelected()
{
    clearSelection();
    ui->statusMsg->setText(tr("Multiple items selected"));
}

void InspectorWidget::clearSelection()
{
    this->element = 0;
}
