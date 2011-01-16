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

#ifndef DATARENDERER_H
#define DATARENDERER_H

#include <QtGui/QWidget>
#include <QtCore/QObject>

#include <plvcore/RefPtr.h>
#include <plvcore/Types.h>

#include "plvgui_global.h"

namespace plv
{
    class Pin;
}

namespace plvgui
{

    /** Interface for DataRenderers that observe or monitor pins of a certain dataType.
      * Each implementation must register itself with the DataRendererFactory
      * and implement the methods needed to render the datatype.
      */
    class PLVGUI_EXPORT DataRenderer : public QWidget
    {
        Q_OBJECT

    public:
        DataRenderer();
        DataRenderer( QWidget* parent );
        virtual ~DataRenderer();

        /** Start inspecting this pin
          * Disconnects all previous connections
          */
        virtual void setPin(const plv::Pin* pin);

    public slots:
        virtual void newData( QVariant v ) = 0;
    };
}

#endif // DATARENDERER_H
