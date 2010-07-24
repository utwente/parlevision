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

#include "RefPtr.h"
#include "RefCounted.h"
#include "PinConnection.h"
#include "PipelineElement.h"

namespace plv
{
    class Pin;
    class IInputPin;
    class IOutputPin;
    class ScheduleInfo;

    class Pipeline : public QThread, public RefCounted
    {
        Q_OBJECT
    public:
        typedef QMap< int , RefPtr<PipelineElement> > PipelineElementMap;
        typedef QList< RefPtr<PinConnection> > PipelineConnectionsList;

        Pipeline();

        /** Refcounted so only call this directly if you know what you are
          * doing
          */
        ~Pipeline();

        /** Initialise this Pipeline. Not yet reentrant
          */
        bool init();

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

        /** Remove PipelineElement child from this pipeline.
          * It will no longer be updated.
          * Caller should ensure the parent of the child is updated.
          * @emits elementAdded(child)
          */
        //void removeElement( PipelineElement* child );

        /** Removes the PipelineElement with the given internal id from this pipeline.
          * The ID should be the one returned by add( PipelineElement* child );
          * @see remove( PipelineElement* child );
          */
        void removeElement( int id );

        void removeAllElements();

        PipelineElement* getElement( int id );

        /** Get all the PipelineElements that make up this Pipeline. */
        const PipelineElementMap& getChildren() const;

        /** Get all the PinConnections that make up this Pipeline. */
        const PipelineConnectionsList& getConnections() const;

        /** Create a PinConnnection between the given InputPin and outputPin
          * @emits connectionAdded(connection)
          */
        void connectPins( IOutputPin* outputPin, IInputPin* inputPin );

        /** Disconnects and removes a single connection.
          * Quite slow O(N) since it traverses a linked list.
          * TODO make faster?
          * @emits connectionRemoved(connection)
          */
        void disconnect( PinConnection* connection );

        /** Disconnects and removes all connections for this element
          */
        void removeConnectionsForElement( PipelineElement* element );

        /** Disconnects and removes all connections.
          */
        void removeAllConnections();

        /** Removes the connection from the pipeline
          * @requires connection is disconnected from any pins
          */
        void removeConnection(PinConnection* connection);

    protected:
        PipelineElementMap m_children;
        PipelineConnectionsList m_connections;
        QSet<int> m_processing;
        QMutex m_schedulerMutex;

        /**
          * The QThread run loop
          */
        virtual void run();

        int getNewPipelineElementId();

        void schedule( QMap< int, ScheduleInfo* >& schedule );
        void runProcessor( ScheduleInfo* info );

    private:
        bool m_stopRequested;
        bool m_running;
        QSet<int> m_initialized;

    signals:
        void elementAdded(plv::RefPtr<plv::PipelineElement>);
        void elementRemoved(plv::RefPtr<plv::PipelineElement>);
        void elementChanged(plv::RefPtr<plv::PipelineElement>);

        void connectionAdded(plv::RefPtr<plv::PinConnection>);
        void connectionRemoved(plv::RefPtr<plv::PinConnection>);
        void connectionChanged(plv::RefPtr<plv::PinConnection>);

    public slots:
        void start();
        void stop();

    };

    class ScheduleInfo
    {
    protected:
        RefPtr<PipelineElement> m_element;
        int m_staticPriority;
        int m_dynamicPriority;
        int m_avgProcessingTime;

    public:
        ScheduleInfo( PipelineElement* pl, int priority = 0 ) :
            m_element( pl ), m_staticPriority( priority )
        {
        }

        void setStaticPriority( int priority ) { m_staticPriority = priority; }
        int getStaticPriority() const { return m_staticPriority; }

        void setDynamicPriority( int priority ) { m_dynamicPriority = priority; }
        int getDynamicPriority() const { return m_dynamicPriority; }

        PipelineElement* getElement() const { return m_element.getPtr(); }

        int getAvgProcessingTime() const { return m_avgProcessingTime; }
        void setAvgProcessingTime( int time ) { m_avgProcessingTime = time; }

    private:
        ScheduleInfo( const ScheduleInfo&) {}
    };
}
#endif // PIPELINE_H
