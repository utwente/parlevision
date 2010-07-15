#ifndef PIPELINE_H
#define PIPELINE_H

#include <map>
#include <list>
#include <QThread>
#include "RefPtr.h"
#include "RefCounted.h"
#include "PinConnection.h"
#include "PipelineElement.h"

namespace plv
{
    class Pin;
    class IInputPin;
    class IOutputPin;

    class Pipeline : public QThread, public RefCounted
    {
        Q_OBJECT
        typedef std::map<int , RefPtr<PipelineElement> > PipelineElementMap;
        typedef std::list< RefPtr<PinConnection> > PipelineConnectionsList;

    public:
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
          * This results in the Pipeline calling setPipeline on the element
          * and m_children containing the element.
          * @emits elementAdded(child)
          * @return a unique ID for this element within this pipeline.
          */
        int add( PipelineElement* child );

        /** Remove PipelineElement child from this pipeline.
          * It will no longer be updated.
          * Caller should ensure the parent of the child is updated.
          * @emits elementAdded(child)
          */
        void remove( PipelineElement* child );

        /** Removes the PipelineElement with the given internal id from this pipeline.
          * The ID should be the one returned by add( PipelineElement* child );
          * @see remove( PipelineElement* child );
          */
        void remove( int id );

        void removeAllElements();

        PipelineElement* getElement( int id );

        /** Get all the PipelineElements that make up this Pipeline.
          * This method is slow and uncached.
          */
        std::list< RefPtr<PipelineElement> > getChildren() const;

        /** Get all the PinConnections that make up this Pipeline.
          */
        const PipelineConnectionsList& getConnections() const;

        /** Create a PinConnnection between the given InputPin and outputPin
          */
        void connectPins( IOutputPin* outputPin, IInputPin* inputPin );

        /** Removes a single connection. Quite slow O(N) since it traverses a linked list.
          * TODO make faster?
          */
        void removeConnection( PinConnection* connection );

        void removeConnectionsForElement( PipelineElement* element );

        /** Removes all connections. First signals there removal to any signal listeners using
          * the connectionRemoved signal. Then removes the connections from the pipeline
          */
        void removeAllConnections();

    protected:
        PipelineElementMap m_children;
        PipelineConnectionsList m_connections;

        /**
          * The QThread run loop
          */
        virtual void run();
    private:
        int m_idCounter;
        bool m_stopRequested;
        bool m_running;

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
}
#endif // PIPELINE_H
