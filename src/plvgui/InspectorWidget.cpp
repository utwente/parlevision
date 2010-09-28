/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvgui module of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

#include "InspectorWidget.h"
#include "ui_inspectorwidget.h"

#include "CameraConfigFormBuilder.h"
#include "ElementConfigFormBuilder.h"
#include "utils.h"

#include <QtGui>
#include <list>
#include <assert.h>

#include <plvcore/PipelineElement.h>
#include <plvcore/Types.h>

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
    foreach( PlvEnumProxy* proxy, m_enumProxies )
    {
        delete proxy;
    }
    m_enumProxies.clear();
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

            int propIndex = element->metaObject()->indexOfProperty( propertyName.toAscii() );
            QMetaProperty property = element->metaObject()->property( propIndex );
            if( property.isEnumType() )
            {
                QMetaEnum metaEnum = property.enumerator();
                qDebug() << "Found enumerator with name " << metaEnum.name();
                for( int i=0; i < metaEnum.keyCount(); ++i )
                {
                    qDebug() << metaEnum.key( i );
                }
                //addRow( form, element, propertyName, metaEnum );
            }
            else
            {
                switch(value.type())
                {
                case QVariant::Int:
                    addRow(form, element, propertyName, value.toInt());
                    break;
                case QVariant::Double:
                    addRow(form, element, propertyName, value.toDouble());
                    break;
                case QVariant::Bool:
                    addRow(form, element, propertyName, value.toBool());
                    break;
                case QVariant::String:
                    addRow(form, element, propertyName, value.toString(), true);
                    break;
                case QVariant::UserType:
                    if( value.canConvert<plv::Enum>() )
                    {
                        plv::Enum e = value.value<plv::Enum>();
                        addRow( form, element, propertyName, e );
                        qDebug() << "Custom enum type " << propertyName << " found ";
                        qDebug() << e.toString();
                    }
                    break;
                default:
                    // substitute a non-editable string
                    addRow(form, element, propertyName, value.toString(), false);
                    break;
                }
            }
        }
    }

    //ui->statusMsg->setText(element->getName());
    //this->setWindowTitle(element->getName());
    ui->propertiesTab->layout()->addWidget(formContainer);
    ui->tabWidget->show();
    ui->infoBox->setText(Utils::elementInfoText(element));
}

void InspectorWidget::nothingSelected()
{
    clearSelection();
    ui->statusMsg->setText(tr("Nothing selected"));
    ui->statusMsg->show();
    ui->tabWidget->hide();
    ui->infoBox->setText("");
}

void InspectorWidget::multipleSelected()
{
    clearSelection();
    ui->statusMsg->setText(tr("Multiple items selected"));
    ui->statusMsg->show();
    ui->tabWidget->hide();
    ui->infoBox->setText("");
}

void InspectorWidget::clearSelection()
{
//    this->setWindowTitle("Inspector");
    this->element = 0;
    if(this->formContainer != 0)
    {
        ui->propertiesTab->layout()->removeWidget(this->formContainer);
        delete this->formContainer;
    }
    this->formContainer = 0;

    foreach( PlvEnumProxy* proxy, m_enumProxies )
    {
        delete proxy;
    }
    m_enumProxies.clear();
}

//void InspectorWidget::addRow(QFormLayout* form, RefPtr<PipelineElement> element, QString* name, QVariant* value)
//{
//    switch(value->type())
//    {
//    case QVariant::Int:
//        addRow(form, element, name, value->toInt());
//        break;
//    case QVariant::Double:
//        addRow(form, element, name, value->toDouble());
//        break;
//    case QVariant::Bool:
//        addRow(form, element, name, value->toBool());
//        break;
//    case QVariant::String:
//        addRow(form, element, name, value->toString(), true);
//        break;
//    default:
//        // substitute a non-editable string
//        addRow(form, element, name, value->toString(), false);
//        break;
//    }
//}

//void InspectorWidget::addRow(QFormLayout* form, RefPtr<PipelineElement> element, const QString& name, QMetaEnum metaEnum )
//{
//    QComboBox* comboBox = new QComboBox( form->parentWidget() );
//    QStringList items;
//    for( int i=0; i < metaEnum.keyCount(); ++i )
//    {
//        items.append( metaEnum.key( i ) );
//    }

//    comboBox->addItems( items );
//    comboBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
//    form->addRow(new QLabel( name, form->parentWidget()), comboBox);

//    QMetaProperty prop = element->metaObject()->property(
//                    element->metaObject()->indexOfProperty(name.toAscii()));

//    if( prop.hasNotifySignal() )
//    {
//        QString signature = prop.notifySignal().signature();
//        qDebug() << "connecting signal " << signature;
////        priorityChanged(Priority)
////        signature.replace( QRegExp("\\([^)]*\\)"), "(QString)" );
////        qDebug() << signature;
////        connect( element, QByteArray::number(QSIGNAL_CODE) + signature,
////                 comboBox, SLOT( setCurrentIndex(QString) ));
//    }
//    else
//    {
//        qWarning() << "WARNING: Property " << name << " does not nave NOTIFY signal!";
//    }

//    QString slot = QByteArray::number(QSLOT_CODE) + propertySlotSignature(element, name, "QString");
//    comboBox->setEnabled( connect(comboBox, SIGNAL( currentIndexChanged(QString) ), element, slot.toAscii()));
//}

void InspectorWidget::addRow(QFormLayout* form, RefPtr<PipelineElement> element, const QString& name, plv::Enum plvEnum )
{
    QComboBox* comboBox = new QComboBox( form->parentWidget() );
    comboBox->addItems( plvEnum.getItemNames() );
    comboBox->setCurrentIndex( plvEnum.getSelectedIndex() );
    comboBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    form->addRow(new QLabel( name, form->parentWidget()), comboBox);

    QMetaProperty prop = element->metaObject()->property(
                    element->metaObject()->indexOfProperty(name.toAscii()));

    // make a proxy object
    PlvEnumProxy* proxy = new PlvEnumProxy( plvEnum );
    m_enumProxies.insert( name, proxy );

    if( prop.hasNotifySignal() )
    {
        // connect the element and use this as proxy object to translate from
        // plv::Enum to
        connect( element.getPtr(), QByteArray::number(QSIGNAL_CODE) + prop.notifySignal().signature(),
                    proxy, SLOT( enumToInt( plv::Enum ) ));

        // connect this to the combo box for translation
        connect( proxy, SIGNAL( indexSet( int ) ),
                 comboBox, SLOT( setCurrentIndex( int ) ) );
    }
    else
    {
        qWarning() << "WARNING: Property " << name << " does not nave NOTIFY signal!";
    }

    QString slot = QByteArray::number(QSLOT_CODE) + propertySlotSignature( element, name );

    connect( comboBox, SIGNAL( currentIndexChanged(int) ),
             proxy, SLOT( intToEnum(int) ) );

    connect( proxy, SIGNAL( indexSet(plv::Enum) ),
             element.getPtr(), slot.toAscii() );

    comboBox->setEnabled( true );
}

void InspectorWidget::addRow(QFormLayout* form, RefPtr<PipelineElement> element, const QString& name, int value)
{
    QSpinBox* spinBox = new QSpinBox(form->parentWidget());
    spinBox->setRange(-2147483647,2147483647);
    spinBox->setValue(value);
    spinBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));

    QMetaProperty prop = element->metaObject()->property(
                    element->metaObject()->indexOfProperty(name.toAscii()));

    if(prop.hasNotifySignal())
    {
        qDebug() << "connecting signal " << prop.notifySignal().signature();;
        connect(element, QByteArray::number(QSIGNAL_CODE) + prop.notifySignal().signature(),
                spinBox, SLOT(setValue(int)));
    }
    else
    {
        qWarning() << "WARNING: Property " << name << " does not nave NOTIFY signal!";
    }

    QString slot = QByteArray::number(QSLOT_CODE) + propertySlotSignature(element, name);
    spinBox->setEnabled(connect(spinBox, SIGNAL(valueChanged(int)),
                        element, slot.toAscii()));

    form->addRow(new QLabel( name, form->parentWidget()), spinBox);
}

void InspectorWidget::addRow(QFormLayout* form, RefPtr<PipelineElement> element, const QString& name, double value)
{
    QDoubleSpinBox* spinBox = new QDoubleSpinBox(form->parentWidget());
    spinBox->setRange(-10000000.0,100000000.0);
    spinBox->setSingleStep(0.1);
    spinBox->setDecimals(8);
    spinBox->setValue(value);
    spinBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));

    QMetaProperty prop = element->metaObject()->property(
                    element->metaObject()->indexOfProperty(name.toAscii()));

    if(prop.hasNotifySignal())
    {
        qDebug() << "connecting signal " << prop.notifySignal().signature();;
        connect(element, QByteArray::number(QSIGNAL_CODE) + prop.notifySignal().signature(),
                spinBox, SLOT(setValue(double)));
    }
    else
    {
        qWarning() << "WARNING: Property " << name << " does not nave NOTIFY signal!";
    }

    QString slot = QByteArray::number(QSLOT_CODE) + propertySlotSignature(element, name);
    spinBox->setEnabled(connect(spinBox, SIGNAL(valueChanged(double)),
                        element, slot.toAscii()));

    form->addRow(new QLabel( name, form->parentWidget()), spinBox);
}


void InspectorWidget::addRow(QFormLayout* form, RefPtr<PipelineElement> element, const QString& name, bool value)
{
    QCheckBox* checkBox = new QCheckBox(this->formContainer);
    checkBox->setChecked(value);

    QMetaProperty prop = element->metaObject()->property(
                    element->metaObject()->indexOfProperty(name.toAscii()));

    if(prop.hasNotifySignal())
    {
        qDebug() << "connecting signal " << prop.notifySignal().signature();;
        connect(element, QByteArray::number(QSIGNAL_CODE) + prop.notifySignal().signature(),
                checkBox, SLOT(setChecked(bool)));
    }
    else
    {
        qWarning() << "WARNING: Property " << name << " does not nave NOTIFY signal!";
    }

    QString slot = QByteArray::number(QSLOT_CODE) + propertySlotSignature(element, name);
    checkBox->setEnabled(connect(checkBox, SIGNAL(toggled(bool)),
                        element, slot.toAscii()));

    form->addRow(new QLabel( name, form->parentWidget()), checkBox);
}

void InspectorWidget::addRow(QFormLayout* form, RefPtr<PipelineElement> element, const QString& name,
                             const QString& value, bool editable)
{
    if(!editable)
    {
        form->addRow(new QLabel( name, form->parentWidget()), new QLabel(value, form->parentWidget()));
    }
    else
    {
        QLineEdit* textField = new QLineEdit(this->formContainer);
        textField->setText(value);

        QMetaProperty prop = element->metaObject()->property(
                        element->metaObject()->indexOfProperty(name.toAscii()));

        if(prop.hasNotifySignal())
        {
            qDebug() << "connecting signal " << prop.notifySignal().signature();;
            connect(element, QByteArray::number(QSIGNAL_CODE) + prop.notifySignal().signature(),
                    textField, SLOT(setText(QString)));
        }
        else
        {
            qWarning() << "WARNING: Property " << name << " does not nave NOTIFY signal!";
        }

        QString slot = QByteArray::number(QSLOT_CODE) + propertySlotSignature(element, name);
        textField->setEnabled(connect(textField, SIGNAL(textChanged(QString)),
                            element, slot.toAscii()));

        form->addRow(new QLabel( name, form->parentWidget()), textField);
    }
}

QString InspectorWidget::propertySlotSignature(QObject* obj, QString propertyName, QString signature )
{
    assert(propertyName.length() > 0);
    if(! propertyName.length() > 0)
        return "UNKNOWN()";

    QVariant value = obj->property(propertyName.toAscii());
    QString methodName;

    if( !signature.isEmpty() )
    {
        methodName = "set"
                         + propertyName.replace(0, 1, propertyName.at(0).toUpper())
                         + "(" + signature + ")";
    }
    else
    {
        methodName = "set"
                         + propertyName.replace(0, 1, propertyName.at(0).toUpper())
                         + "(" + QString(value.typeName()) + ")";
    }
    return methodName;
}
