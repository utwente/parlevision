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

#ifndef PIPELINESCENE_H
#define PIPELINESCENE_H

#include <QGraphicsScene>
#include <QHash>

#include "RefPtr.h"
#include "Pipeline.h"
#include "PinConnection.h"
#include "Exceptions.h"

class QObject;

namespace plv
{
    class PipelineElement;
    class Pin;
    class PinConnection;
}

namespace plvgui
{
    class PipelineElementWidget;
    class PinWidget;
    class InteractiveLine;
    class ConnectionLine;
    class MainWindow;

    /** A QGraphicsScene that wraps a Pipeline
      * to provide a graphical canvas for all the elements to live on.
      * This is only the model, it needs a QGraphicsView to render.
      *
      * The PipelineScene also handles all interaction with its contents,
      * such as adding, removing, selecting and changing elements and connections.
      */
    class PipelineScene : public QGraphicsScene
    {
        Q_OBJECT

    public:
        PipelineScene(plv::Pipeline* pipeline, QObject* parent);
        plv::RefPtr<plv::Pipeline> getPipeline() { return m_pipeline; }
        virtual bool event(QEvent * event);

    public slots:
        /** Add the given element to this scene
          * and to the underlying pipeline
          */
        void add(plv::PipelineElement* e);
        void add(plv::RefPtr<plv::PipelineElement> e);

        /** Add the given connection to this scene
          * and to the underlying pipeline
          */
        void add(plv::PinConnection* c);
        void add(plv::RefPtr<plv::PinConnection> c);

        /** Deletes the currently selected items (elements, connections etc.)
          * Has no effect if nothing is selected.
          */
        void deleteSelected();


        /** Remove the given element from this scene
          * and from the underlying pipeline
          */
        void remove(plv::PipelineElement* e);
        void remove(plv::RefPtr<plv::PipelineElement> e);

        /** Remove the given connection from this scene
          * and from the underlying pipeline
          */
        void remove(plv::PinConnection* c);
        void remove(plv::RefPtr<plv::PinConnection> c);

        /** Remove the given element from this scene
          * but not from the underlying pipeline
          */
        void handleRemove(plv::PipelineElement* e);
        void handleRemove(plv::RefPtr<plv::PipelineElement> e);

        /** Remove the given connection from this scene
          * but not from the underlying pipeline
          */
        void handleRemove(plv::PinConnection* c);
        void handleRemove(plv::RefPtr<plv::PinConnection> c);

        /** Recalculates the scene rect based on its contents.
          * This is a relatively expensive method that ensures
          * everything fits, but the scene has a default size as well.
          */
        void recalculateSceneRect();

        /** Indicate something has changed to the underlying model
          * @emits contentsChanged()
          */
        void setChanged();

    signals:
        void contentsChanged();

    protected:
//        virtual void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
        virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);
        virtual void dragEnterEvent ( QGraphicsSceneDragDropEvent * event );
        virtual void dragMoveEvent ( QGraphicsSceneDragDropEvent * event );
        virtual void dropEvent(QGraphicsSceneDragDropEvent *event);
        QRectF	sceneRect () const;

    private:
        void clearLine();
        void handleConnectionCreation(PinWidget* source, PinWidget* target)
                throw (NonFatalException);
        MainWindow* getMainWindow();
        // make sure everything fits inside the scene
        void ensureFit();

        plv::RefPtr<plv::Pipeline> m_pipeline;
        QHash<plv::PipelineElement*, PipelineElementWidget*> elementWidgets;
        QHash<plv::PinConnection*, ConnectionLine*> connectionLines;

        PipelineElementWidget* getWidgetFor(plv::PipelineElement* e) const;
        PinWidget* getWidgetFor(const plv::Pin* pw) const;
        ConnectionLine* getWidgetFor(plv::PinConnection* c) const;
        InteractiveLine* line;
    };

}

#endif // PIPELINESCENE_H
