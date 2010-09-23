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

#include "LibraryElement.h"

#include <QtGui>

#include <plvcore/PipelineElement.h>
#include <plvcore/Pin.h>
#include <plvcore/RefPtr.h>

using namespace plv;
using namespace plvgui;

LibraryElement::LibraryElement(RefPtr<PipelineElement> element, QWidget* parent)
    : QWidget(parent),
    element(element)
{
    this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    this->setContentsMargins(0,0,0,0);

    this->setObjectName("Library Element " + element->getName());

    this->outerContainer = new QVBoxLayout(this);
    this->outerContainer->setContentsMargins(8,2,8,4);
    this->outerContainer->setSpacing(2);

    QWidget* pinWrapper = new QWidget(this);

    this->innerContainer = new QHBoxLayout(pinWrapper);
    this->innerContainer->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    this->innerContainer->setContentsMargins(0,0,0,0);

    QWidget* inPinWrapper = new QWidget(this);
    inPinWrapper->setContentsMargins(0,0,0,0);
    this->inPinContainer = new QVBoxLayout(inPinWrapper);
    this->inPinContainer->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->inPinContainer->setContentsMargins(0,0,0,0);
    this->inPinContainer->setSpacing(0);

    QWidget* outPinWrapper = new QWidget(this);
    outPinWrapper->setContentsMargins(0,0,0,0);
    this->outPinContainer = new QVBoxLayout(outPinWrapper);
    this->outPinContainer->setAlignment(Qt::AlignTop | Qt::AlignRight);
    this->outPinContainer->setContentsMargins(0,0,0,0);
    this->outPinContainer->setSpacing(0);

    this->innerContainer->addWidget(inPinWrapper, Qt::AlignLeft | Qt::AlignTop);
    this->innerContainer->addWidget(outPinWrapper, Qt::AlignRight | Qt::AlignTop);

    QLabel* title = new QLabel(this->element->getName(), this);
    title->setContentsMargins(0,0,0,0);
    title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    title->setAlignment(Qt::AlignCenter);
    this->outerContainer->addWidget(title);
    this->outerContainer->addWidget(pinWrapper);

    QString pinStyle = "font-size: 10px;";
    const PipelineElement::InputPinMap& inPins = element->getInputPins();
    for( PipelineElement::InputPinMap::const_iterator itr = inPins.begin()
        ; itr!=inPins.end(); ++itr)
    {
        RefPtr<IInputPin> pin = itr->second;
        assert(pin.isNotNull());
        QWidget* label = new QLabel(pin->getName());
        label->setStyleSheet(pinStyle);
        label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        inPinContainer->addWidget(label, Qt::AlignLeft | Qt::AlignTop);
    }

    const PipelineElement::OutputPinMap& outPins = element->getOutputPins();
    for( PipelineElement::OutputPinMap::const_iterator itr = outPins.begin()
        ; itr!=outPins.end(); ++itr)
    {
        RefPtr<IOutputPin> pin = itr->second;
        assert(pin.isNotNull());
        QWidget* label = new QLabel(pin->getName());
        label->setStyleSheet(pinStyle);
        label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        outPinContainer->addWidget(label, Qt::AlignRight | Qt::AlignTop);
    }
}

void LibraryElement::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    QPainterPath path;
    painter.setRenderHint(QPainter::Antialiasing);
    path.addRoundedRect(0, 0, this->width()-1, this->height()-1, 8.0, 8.0, Qt::AbsoluteSize);
    painter.fillPath(path, Qt::green);
    painter.strokePath(path, painter.pen());
    QWidget::paintEvent(event);
}

QSize LibraryElement::sizeHint() const
{
    QSize size = QWidget::sizeHint();
    size.setWidth(200);
    return size;
}

void LibraryElement::mousePressEvent(QMouseEvent*)
{
    emit(pressed());
}

void LibraryElement::mouseMoveEvent(QMouseEvent*)
{
    emit(moved());
}

void LibraryElement::mouseReleaseEvent(QMouseEvent*)
{
    emit(released());
}
