/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvcore module of ParleVision.
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

#ifndef PIPELINE_H
#define PIPELINE_H

#include <map>
#include <list>

#include <QMap>
#include <QList>
#include <QSet>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QTime>

#include "RefPtr.h"
#include "RefCounted.h"
#include "PinConnection.h"
#include "PipelineElement.h"

namespace plv
{
    class Pin;
    class IInputPin;
    class IOutputPin;
    class Scheduler;

    class PLVCORE_EXPORT Pipeline : public QThread, public RefCounted
    {
        Q_OBJECT
    public:
        typedef QMap< int , RefPtr<PipelineElement> > PipelineElementMap;
        typedef QList< RefPtr<PinConnection> > PipelineConnectionsList;

        Pipeline();

        /** Refcounted so only call this directly if you know what you are
          * doing
          */
        virtual ~Pipeline();

        /** Initialise this Pipeline. Not yet reentrant
          */
        //bool init();

        /** Removes all PipelineElements and Connections from this pipeline */
        void clear();

        /** Add the PipelineElement to this Pipeline.
          * This results in the Pipeline calling setPipeline and setId on the element
          * and m_children containing the element.
          * @emits elementAdded(child)
          * @return a unique ID for this element within this pipeline which is also
          * set within the child element if the child element does not already contain an id.
          * @throws IllegalArgumentException when child already has an ID which is
          * in use in this pipeline.
          */
        int addElement( PipelineElement* child ) throw( IllegalArgumentException );

        /** This method checks if the pipeline element can be added to this pipeline
          * by checking its ID member field. If this ID is -1 or and ID which is not
          * in use in this pipeline it will return true, else false.
          * @returns true if the PipelineElement can be added, false if not.
          */
        bool canAddElement( PipelineElement* child );

       /** Removes the PipelineElement with the given internal id from this pipeline.
          * The ID should be the one returned by add( PipelineElement* child );
          * @see remove( PipelineElement* child );
          */
        void removeElement( int id );

        /** @returns the PipelineElement with the given id or 0 if it does not exist */
        PipelineElement* getElement( int id );

        /** Get all the PipelineElements that make up this Pipeline. */
        const PipelineElementMap& getChildren() const;

        /** Get all the PinConnections that make up this Pipeline. */
        const PipelineConnectionsList& getConnections() const;

        /** Create a PinConnnection between the given InputPin and outputPin
          * @emits connectionAdded(connection)
          */
        void connectPins( IOutputPin* outputPin, IInputPin* inputPin )
            throw(PinConnection::IllegalConnectionException,
                  PinConnection::IncompatibleTypeException,
                  PinConnection::DuplicateConnectionException);

        /** Disconnects and removes a single connection.
          * Quite slow O(N) since it traverses a linked list.
          * TODO make faster?
          * @emits connectionRemoved(connection)
          */
        void disconnect( PinConnection* connection );

    protected:
        PipelineElementMap m_children;
        PipelineConnectionsList m_connections;
        QMutex m_pipelineMutex;

        /**
          * The QThread run loop
          */
        virtual void run();

    private:
        bool m_stopRequested;
        bool m_running;
        Scheduler* m_scheduler;

        /** does a disconnect on PinConnection. Private thread unsafe function
          * use disconnect( PinConnection* ) public function when calling
          * from outside this class
          */
        void threadUnsafeDisconnect( PinConnection* connection );

        /** Disconnects and removes all connections for this element
          */
        void removeConnectionsForElement( PipelineElement* element );

        /** Disconnects and removes all connections.
          */
        void removeAllConnections();

        /** Removes all elements from the pipeline */
        void removeAllElements();

        int getNewPipelineElementId();

    signals:
        void elementAdded(plv::RefPtr<plv::PipelineElement>);
        void elementRemoved(plv::RefPtr<plv::PipelineElement>);
        void elementChanged(plv::RefPtr<plv::PipelineElement>);

        void connectionAdded(plv::RefPtr<plv::PinConnection>);
        void connectionRemoved(plv::RefPtr<plv::PinConnection>);
        void connectionChanged(plv::RefPtr<plv::PinConnection>);

        void errorOccurred( QString errorStr );

        void started();
        void stopped();

        void tick();

    public slots:
        void start();
        void stop();

    };
}
#endif // PIPELINE_H
