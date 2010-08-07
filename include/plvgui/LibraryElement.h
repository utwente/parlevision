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

#ifndef LIBRARYELEMENT_H
#define LIBRARYELEMENT_H

#include <QString>
#include <QWidget>
#include <QHBoxLayout>
#include "RefPtr.h"
#include "PipelineElement.h"

class QVBoxLayout;
class QHBoxLayout;

namespace plv
{
//    class PipelineElement;
}

namespace plvgui {
    class LibraryElement : public QWidget
    {
        Q_OBJECT

    public:
        LibraryElement(plv::RefPtr<plv::PipelineElement> element, QWidget* parent);
        plv::RefPtr<plv::PipelineElement> getElement() { return element; }

    signals:
        void pressed();
        void moved();
        void released();

    protected:
        QSize sizeHint() const;
        void mousePressEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
        void paintEvent(QPaintEvent*);

    private:
        plv::RefPtr<plv::PipelineElement> element;

        QVBoxLayout* outerContainer;
        QHBoxLayout* innerContainer;
        QVBoxLayout* inPinContainer;
        QVBoxLayout* outPinContainer;
    };
}

#endif // LIBRARYELEMENT_H
