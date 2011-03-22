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

#include "VariantDataRenderer.h"

#include <QDebug>
#include <plvgui/ImageWidget.h>

#include <QVBoxLayout>
#include <QPainter>
#include <QTextEdit>

using namespace plvgui;
using namespace plv;

VariantDataRenderer::VariantDataRenderer(QWidget *parent)
    : DataRenderer(parent),
    m_layout( new QVBoxLayout(this) ),
    m_textEdit( new QTextEdit(this) )
{
    m_layout->addWidget( m_textEdit );

    // TODO make minimum configurable somewhere
    m_textEdit->setMinimumSize( 160, 120 );

    this->setLayout( m_layout );
}

VariantDataRenderer::~VariantDataRenderer()
{
}

void VariantDataRenderer::newData( unsigned int serial, const QVariant data )
{
    QString dataString = data.toString();
    if( dataString.isEmpty() )
    {
        dataString = tr("Error: cannot convert data to a string representation.");
    }
    QString message = tr("%1: %2").arg(serial).arg(dataString);
    m_textEdit->append(message);
}
