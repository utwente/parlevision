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
#include <QTimer>
#include <QFuture>
#include <QtConcurrentRun>

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
    class PipelineProducer;
    class PipelineProcessor;

    class RunItem
    {
    public:
        PipelineElement* m_element;
        unsigned int m_serial;
        QFuture<bool> m_result;

        RunItem( PipelineElement* element, unsigned int serial ) :
                m_element(element), m_serial(serial) {}

        inline unsigned int getSerial() const { return m_serial; }
        inline PipelineElement* getElement() const { return m_element; }
        QFuture<bool> getFuture() const { return m_result; }

        bool operator ==(const RunItem& other) const { return other.m_serial == m_serial && other.m_element == m_element; }
        bool operator < (const RunItem& other) const { return m_serial < other.m_serial; }

        void dispatch()
        {
            assert( m_element->getState() == PipelineElement::STARTED );
            m_element->setState( PipelineElement::DISPATCHED );
            m_result = QtConcurrent::run( m_element, &PipelineElement::run, m_serial );
        }
    };

    /** Helper class for a QThread to run its own event loop */
    class PLVCORE_EXPORT QThreadEx : public QThread
    {
    protected:
        void run() { exec(); }
    };

    class PLVCORE_EXPORT Pipeline : public QObject, public RefCounted
    {
        Q_OBJECT

    public:
        typedef QMap< int , RefPtr<PipelineElement> > PipelineElementMap;
        typedef QMap< int , RefPtr<PinConnection> >   PipelineConnectionMap;

        Pipeline();
        virtual ~Pipeline();

        /** Initialise this Pipeline. */
        bool init();
        void deinit();

        /** Removes all PipelineElements and Connections from this pipeline */
        void clear();

        /** Returs true if this pipeline has no elements */
        bool isEmpty();

        /** Add the PipelineElement to this Pipeline.
          * This results in the Pipeline calling setPipeline and setId on the element
          * and m_children containing the element.
          * @emits elementAdded(child)
          * @return a unique ID for this element within this pipeline which is also
          * set within the child element if the child element does not already contain an id.
          * Logs warning when child has an ID which is in use in this pipeline.
          */
        int addElement( PipelineElement* child );

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
        void removeElement(int id);

        /** @returns the PipelineElement with the given id or 0 if it does not exist */
        PipelineElement* getElement(int id);

        PinConnection* getConnection(int id);

        /** Get all the PipelineElements that make up this Pipeline. */
        const PipelineElementMap& getChildren() const;

        /** Get all the PinConnections that make up this Pipeline. */
        const PipelineConnectionMap& getConnections() const;

        /** returns true if pins can be connected. Returns false if not. If not
          * possible reason contains a message stating the reason for failure
          */
        bool canConnectPins( IOutputPin* outputPin, IInputPin* inputPin, QString& reason );

        /** Create a PinConnnection between the given InputPin and outputPin
            returns the connection id. Emits connectionAdded(id). */
        int connectPins( IOutputPin* outputPin, IInputPin* inputPin )
            throw(PinConnection::IllegalConnectionException,
                  PinConnection::IncompatibleTypeException,
                  PinConnection::DuplicateConnectionException);

        /** Disconnects and removes a single connection. Emits connectionRemoved(id) */
        void pinConnectionDisconnect(int id);

        inline bool isRunning() const { QMutexLocker lock( &m_pipelineMutex ); return m_running; }

    private:
        PipelineElementMap m_children;
        PipelineConnectionMap m_connections;
        mutable QMutex m_pipelineMutex;

        //QThreadEx m_pipelineThread;
        unsigned int m_serial;
        bool m_running; /** true when pipeline is running */
        QList<RunItem> m_runQueue;
        int m_runQueueThreshold;

        QTimer m_heartbeat;
        bool m_stepwise;
        bool m_producersReady;

        QTime m_timeSinceLastFPSCalculation;
        int m_numFramesSinceLastFPSCalculation;
        float m_fps; /** running avg of fps */

        QList<PipelineElement*> m_ordering;
        QMap<unsigned int, int> m_pipelineStages;
        //int m_i;
        QMap<int, PipelineProducer* > m_producers;
        QMap<int, PipelineProcessor* > m_processors;

        bool m_changed;
        QString m_filename;

        inline bool isChanged() const { return m_changed; }
        inline void setChanged(bool changed) {  m_changed = changed; }

        /** does a disconnect on PinConnection with id. Private thread unsafe function
          * use disconnect( int ) public function when calling
          * from outside this class */
        void threadUnsafeDisconnect( int id);

        /** Disconnects and removes all connections for this element */
        void removeConnectionsForElement( PipelineElement* element );

        /** Disconnects and removes all connections. */
        void removeAllConnections();

        /** Removes all elements from the pipeline. not thread safe. */
        void removeAllElements();

        int getNewPipelineElementId();
        int getNewPinConnectionId();

        bool generateGraphOrdering( QList<PipelineElement*>& ordering );

    signals:
        void elementAdded(int);
        void elementRemoved(int);
        void elementChanged(int);

        void elementAdded(const plv::RefPtr<plv::PipelineElement>&);
        void elementRemoved(const plv::RefPtr<plv::PipelineElement>&);
        void elementChanged(const plv::RefPtr<plv::PipelineElement>&);

        void connectionAdded(int);
        void connectionRemoved(int);
        void connectionChanged(int);

        void connectionAdded(const plv::RefPtr<plv::PinConnection>&);
        void connectionRemoved(const plv::RefPtr<plv::PinConnection>&);
        void connectionChanged(const plv::RefPtr<plv::PinConnection>&);

        void pipelineMessage(QtMsgType type, const QString& msg);
        //void pipelineMessage(PlvMessageType type, const QString& msg);

        void pipelineStarted();
        void pipelineStopped();
        void finished();

        void stepTaken(unsigned int serial);
        void producersAreReady();
        void framesPerSecond(float);

        void pipelineLoaded(const QString&);
        void pipelineSaved(const QString&);
        void pipelineChanged(bool);

    public slots:
        void start();
        void stop();
        void finish();
        void schedule();
        void pipelineElementError( PlvErrorType type, plv::PipelineElement* ple );

        void handleMessage(QtMsgType type, const QString& msg);
        void handleMessage(PlvMessageType type, const QString& msg);

        bool load(const QString& filename);
        bool save();
        bool saveAs(const QString& filename);
    };
}
#endif // PIPELINE_H
