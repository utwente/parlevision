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

#ifndef LIBRARYWIDGET_H
#define LIBRARYWIDGET_H

#include <QDockWidget>
#include <QMultiMap>

namespace Ui {
    class LibraryWidget;
}

namespace plv
{
    class PipelineElement;
}

namespace plvgui
{
    class LibraryElement;
    class MainWindow;

    class LibraryWidget : public QDockWidget
    {
        Q_OBJECT

    public:
        explicit LibraryWidget(MainWindow* parent = 0);
        ~LibraryWidget();

    signals:
        void errorOccurred(QString);

    protected:
//         void dragEnterEvent(QDragEnterEvent *event);
//         void dragMoveEvent(QDragMoveEvent *event);
        void mousePressEvent(QMouseEvent *event);

    private slots:
        void elementPressed();
        void elementMoved();
        void elementReleased();

    private:
        Ui::LibraryWidget *ui;
        // create an item and add it to allElements
        void createItem(QString name);
        QString infoFor(plv::PipelineElement*);
        LibraryElement* draggedElement;
        QMultiMap<QString,LibraryElement*> allElements;
    };
}

#endif // LIBRARYWIDGET_H
