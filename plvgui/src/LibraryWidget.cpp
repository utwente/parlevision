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

#include "LibraryWidget.h"
#include "ui_librarywidget.h"
#include "LibraryElement.h"
#include "MainWindow.h"
#include "utils.h"

#include <plvcore/PipelineElement.h>
#include <plvcore/RefPtr.h>
#include <plvcore/Pin.h>

#include <QDebug>
#include <QtGui>
#include <QStringBuilder>

using namespace plvgui;
using namespace plv;

LibraryWidget::LibraryWidget(MainWindow* parent) :
    QDockWidget(parent),
    ui(new Ui::LibraryWidget),
    draggedElement(0)
{
    connect(this, SIGNAL(errorOccurred(QString)),
            parent, SLOT(criticalError(QString)));

    ui->setupUi(this);

    std::list<QString> types = plv::PipelineElement::types();

    for(std::list<QString>::iterator iter = types.begin();
        iter != types.end(); iter++)
    {
        createItem(*iter);
    }
    // add sorted by name
    foreach(LibraryElement* lw, this->allElements.values())
    {
        ui->container->addWidget(lw);
    }
}

LibraryWidget::~LibraryWidget()
{
    delete ui;
}

void LibraryWidget::createItem(QString typeName)
{
    try
    {
        int id = QMetaType::type(typeName.toAscii());
        if(!QMetaType::isRegistered(id))
        {
            qWarning() << "Ignoring unknown element " << typeName;
            return;
        }

        RefPtr<PipelineElement> element = static_cast<PipelineElement*>(QMetaType::construct(id));

        LibraryElement* w = new LibraryElement(element, this);
        connect(w, SIGNAL(pressed()), this, SLOT(elementPressed()));
        connect(w, SIGNAL(moved()), this, SLOT(elementMoved()));
        connect(w, SIGNAL(released()), this, SLOT(elementReleased()));
    //    ui->container->addWidget(w);
        this->allElements.insert(w->getElement()->getName().toLower(), w);
    }
    catch( PipelineException& pe )
    {
        emit(errorOccurred(typeName + ": " + pe.what()));
        return;
    }
    catch( IllegalAccessException& iae )
    {
        emit(errorOccurred(typeName + ": " + iae.what()));
        return;
    }
    catch( PlvException& e )
    {
        emit(errorOccurred(typeName + ": " + e.what()));
        return;
    }
    catch( std::runtime_error& err )
    {
        emit(errorOccurred(typeName + ": " + err.what()));
        return;
    }
    catch( ... )
    {
        qDebug() << "Uncaught exception in LibraryWidget"
                 << "of unknown type.";
        emit(errorOccurred(typeName + ": failed to load processor"));
        return;
    }
}

void LibraryWidget::mousePressEvent(QMouseEvent *event)
{
    QDockWidget::mousePressEvent(event);
}

void LibraryWidget::elementPressed()
{
    // Currently not needed
}

void LibraryWidget::elementMoved()
{
    if(this->draggedElement != 0)
        return;

    // start drag
    this->draggedElement = qobject_cast<LibraryElement*>(sender());

    QPoint hotSpot = QPoint(0,0);
    QString elementName = draggedElement->getElement()->metaObject()->className();
//    qDebug() << "starting drag of " << elementName << " hotspot: "<< hotSpot;

    QByteArray itemData;
    itemData.append(elementName);

    QMimeData* mimeData = new QMimeData();
    mimeData->setData("x-plv-element-name", itemData);

    QPixmap pixmap(draggedElement->size());
    draggedElement->render(&pixmap);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(hotSpot);
    drag->exec(Qt::CopyAction, Qt::CopyAction);
}

void LibraryWidget::elementReleased()
{
    LibraryElement* element = qobject_cast<LibraryElement*>(sender());
    if (!element) return;

    if(this->draggedElement == 0)
    {
        // this was a click, not the end of a drag
        ui->infoBox->setText(Utils::elementInfoText(element->getElement()));
    }
    this->draggedElement = 0;
}
