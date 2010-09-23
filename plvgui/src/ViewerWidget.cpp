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

#include "ViewerWidget.h"
#include "ui_viewerwidget.h"

//#include <QtGui>

#include <assert.h>
#include <plvcore/Pin.h>

#include "DataRenderer.h"
#include "RendererFactory.h"

using namespace plvgui;
using namespace plv;

ViewerWidget::ViewerWidget(IOutputPin* pin, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ViewerWidget),
    pin(pin)
{
    ui->setupUi(this);
    assert(this->pin.isNotNull());

    DataRenderer* renderer = RendererFactory::create(pin->getTypeInfo().name(), this);
    renderer->setPin(pin);

//    QVBoxLayout*
    this->setWidget(renderer);
    this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    this->setWindowTitle(pin->getOwner()->getName() + "-" + pin->getName());
}

ViewerWidget::~ViewerWidget()
{
    delete ui;
}

void ViewerWidget::changeEvent(QEvent *e)
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


RefPtr<IOutputPin> ViewerWidget::getPin()
{
    return this->pin;
}
