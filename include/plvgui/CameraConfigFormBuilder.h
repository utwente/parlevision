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

#ifndef CAMERACONFIGFORMBUILDER_H
#define CAMERACONFIGFORMBUILDER_H

#include "ElementConfigFormBuilder.h"
#include <QWidget>
#include "plvcore/RefPtr.h"

namespace Ui
{
    class CameraConfigForm;
}

namespace plv
{
    class CameraProducer;
}

namespace plvgui
{
    class CameraForm : public QWidget
    {
    public:
        CameraForm(CameraProducer* producer, QWidget* parent=0);
    private:
        Ui::CameraConfigForm* ui;
        RefPtr<CameraProducer> producer;
    };

    class CameraConfigFormBuilder : public ElementConfigFormBuilder
    {
        Q_OBJECT

    public:
        virtual QWidget* buildForm(PipelineElement* element, QWidget* parent=0);
    };
}

#endif // CAMERACONFIGFORMBUILDER_H
