#include "InspectorWidget.h"
#include "ui_inspectorwidget.h"

#include <QtGui>
#include <list>
#include <assert.h>

#include "PipelineElement.h"
#include "CameraProducer.h"
#include "CameraConfigFormBuilder.h"
#include "ElementConfigFormBuilder.h"

using namespace plvgui;
using namespace plv;

InspectorWidget::InspectorWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::InspectorWidget),
    formContainer(0)
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
    clearSelection();
    this->element = element;
    qDebug() << "Now inspecting " << element;

    ui->statusMsg->hide();

    std::list<QString> propertyNames;
    element->getConfigurablePropertyNames(propertyNames);

    if(ElementConfigFormBuilder::existsFor(element->metaObject()->className()))
    {
        // use that
        ElementConfigFormBuilder* b = ElementConfigFormBuilder::getBuilderFor(element->metaObject()->className());
        formContainer = b->buildForm(element, this);
        delete b;
    }
    else
    {
        this->formContainer = new QWidget(this);
        QFormLayout* form = new QFormLayout(this->formContainer);
        formContainer->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
        this->formContainer->setLayout(form);

        for(std::list<QString>::iterator itr = propertyNames.begin();
            itr != propertyNames.end(); ++itr)
        {
            QString propertyName = *itr;
            QVariant value = element->property(propertyName.toAscii());
            addRow(form, element, &propertyName, &value);
        }
    }

    //ui->statusMsg->setText(element->getName());
    //this->setWindowTitle(element->getName());
    ui->verticalLayout->addWidget(formContainer);
}

void InspectorWidget::nothingSelected()
{
    clearSelection();
    ui->statusMsg->setText(tr("Nothing selected"));
    ui->statusMsg->show();
}

void InspectorWidget::multipleSelected()
{
    clearSelection();
    ui->statusMsg->setText(tr("Multiple items selected"));
    ui->statusMsg->show();
}

void InspectorWidget::clearSelection()
{
//    this->setWindowTitle("Inspector");
    this->element = 0;
    if(this->formContainer != 0)
    {
        ui->verticalLayout->removeWidget(this->formContainer);
        delete this->formContainer;
    }
    this->formContainer = 0;
}

void InspectorWidget::addRow(QFormLayout* form, RefPtr<PipelineElement> element, QString* name, QVariant* value)
{
    switch(value->type())
    {
    case QVariant::Int:
        addRow(form, element, name, value->toInt());
        break;
    case QVariant::Double:
        addRow(form, element, name, value->toDouble());
        break;
    case QVariant::Bool:
        addRow(form, element, name, value->toBool());
        break;
    case QVariant::String:
        addRow(form, element, name, value->toString(), true);
    default:
        break;
        // substitute a non-editable string
        addRow(form, element, name, value->toString(), false);
    }
}

void InspectorWidget::addRow(QFormLayout* form, RefPtr<PipelineElement> element, QString* name, int value)
{
    QSpinBox* spinBox = new QSpinBox(form->parentWidget());
    spinBox->setRange(-2147483647,2147483647);
    spinBox->setValue(value);
    spinBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));

    QMetaProperty prop = element->metaObject()->property(
                    element->metaObject()->indexOfProperty(name->toAscii()));

    if(prop.hasNotifySignal())
    {
        qDebug() << "connecting signal " << prop.notifySignal().signature();;
        connect(element, QByteArray::number(QSIGNAL_CODE) + prop.notifySignal().signature(),
                spinBox, SLOT(setValue(int)));
    }
    else
    {
        qWarning() << "WARNING: Property " << *name << " does not nave NOTIFY signal!";
    }

    QString slot = QByteArray::number(QSLOT_CODE) + propertySlotSignature(element, *name);
    spinBox->setEnabled(connect(spinBox, SIGNAL(valueChanged(int)),
                        element, slot.toAscii()));

    form->addRow(new QLabel(*name, form->parentWidget()), spinBox);
}

void InspectorWidget::addRow(QFormLayout* form, RefPtr<PipelineElement> element, QString* name, double value)
{
    QDoubleSpinBox* spinBox = new QDoubleSpinBox(form->parentWidget());
    spinBox->setRange(-10000000.0,100000000.0);
    spinBox->setSingleStep(0.1);
    spinBox->setDecimals(8);
    spinBox->setValue(value);
    spinBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));

    QMetaProperty prop = element->metaObject()->property(
                    element->metaObject()->indexOfProperty(name->toAscii()));

    if(prop.hasNotifySignal())
    {
        qDebug() << "connecting signal " << prop.notifySignal().signature();;
        connect(element, QByteArray::number(QSIGNAL_CODE) + prop.notifySignal().signature(),
                spinBox, SLOT(setValue(double)));
    }
    else
    {
        qWarning() << "WARNING: Property " << *name << " does not nave NOTIFY signal!";
    }

    QString slot = QByteArray::number(QSLOT_CODE) + propertySlotSignature(element, *name);
    spinBox->setEnabled(connect(spinBox, SIGNAL(valueChanged(double)),
                        element, slot.toAscii()));

    form->addRow(new QLabel(*name, form->parentWidget()), spinBox);
}


void InspectorWidget::addRow(QFormLayout* form, RefPtr<PipelineElement> element, QString* name, bool value)
{
    QCheckBox* checkBox = new QCheckBox(this->formContainer);
    checkBox->setChecked(value);

    QMetaProperty prop = element->metaObject()->property(
                    element->metaObject()->indexOfProperty(name->toAscii()));

    if(prop.hasNotifySignal())
    {
        qDebug() << "connecting signal " << prop.notifySignal().signature();;
        connect(element, QByteArray::number(QSIGNAL_CODE) + prop.notifySignal().signature(),
                checkBox, SLOT(setChecked(bool)));
    }
    else
    {
        qWarning() << "WARNING: Property " << *name << " does not nave NOTIFY signal!";
    }

    QString slot = QByteArray::number(QSLOT_CODE) + propertySlotSignature(element, *name);
    checkBox->setEnabled(connect(checkBox, SIGNAL(toggled(bool)),
                        element, slot.toAscii()));

    form->addRow(new QLabel(*name, form->parentWidget()), checkBox);
}

void InspectorWidget::addRow(QFormLayout* form, RefPtr<PipelineElement> element, QString* name, QString value, bool editable)
{
    if(!editable)
    {
        form->addRow(new QLabel(*name, form->parentWidget()), new QLabel(value, form->parentWidget()));
    }
    else
    {
        QLineEdit* textField = new QLineEdit(this->formContainer);
        textField->setText(value);

        QMetaProperty prop = element->metaObject()->property(
                        element->metaObject()->indexOfProperty(name->toAscii()));

        if(prop.hasNotifySignal())
        {
            qDebug() << "connecting signal " << prop.notifySignal().signature();;
            connect(element, QByteArray::number(QSIGNAL_CODE) + prop.notifySignal().signature(),
                    textField, SLOT(setText(QString)));
        }
        else
        {
            qWarning() << "WARNING: Property " << *name << " does not nave NOTIFY signal!";
        }

        QString slot = QByteArray::number(QSLOT_CODE) + propertySlotSignature(element, *name);
        textField->setEnabled(connect(textField, SIGNAL(textChanged(QString)),
                            element, slot.toAscii()));

        form->addRow(new QLabel(*name, form->parentWidget()), textField);
    }
}

const QString InspectorWidget::propertySlotSignature(QObject* obj, QString propertyName)
{
    assert(propertyName.length() > 0);
    if(! propertyName.length() > 0)
        return "UNKNOWN()";

    QVariant value = obj->property(propertyName.toAscii());
    QString methodName = "set"
                         + propertyName.replace(0, 1, propertyName.at(0).toUpper())
                         + "(" + QString(value.typeName()) + ")";
    return methodName;
}
