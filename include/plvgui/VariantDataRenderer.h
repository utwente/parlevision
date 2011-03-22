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

#ifndef VARIANTDATARENDERER_H
#define VARIANTDATARENDERER_H

#include "DataRenderer.h"
#include "plvgui_global.h"

#include <qglobal.h>
QT_FORWARD_DECLARE_CLASS( QVBoxLayout )
QT_FORWARD_DECLARE_CLASS( QTextEdit )

namespace plv
{
    class Data;
}

namespace plvgui
{
    class ImageWidget;

    class PLVGUI_EXPORT VariantDataRenderer : public DataRenderer
    {
        Q_OBJECT

    public:
        VariantDataRenderer( QWidget* parent );
        virtual ~VariantDataRenderer();

    private:
        QVBoxLayout* m_layout;
        QTextEdit*   m_textEdit;

    public slots:
        virtual void newData( unsigned int serial, QVariant data );

    };
}

#endif // UNKNOWNDATARENDERER_H
