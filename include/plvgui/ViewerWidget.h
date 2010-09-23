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

#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include <QtGui/QDockWidget>
#include <plvcore/RefPtr.h>

namespace Ui
{
    class ViewerWidget;
}

namespace plv
{
    class IOutputPin;
}

using namespace plv;

namespace plvgui
{
    class ViewerWidget : public QDockWidget
    {
        Q_OBJECT

    public:
        explicit ViewerWidget(IOutputPin* pin, QWidget *parent = 0);
        ~ViewerWidget();
        RefPtr<IOutputPin> getPin();

    protected:
        void changeEvent(QEvent *e);

    private:
        Ui::ViewerWidget *ui;
        RefPtr<IOutputPin> pin;
    };
}
#endif // VIEWERWIDGET_H
