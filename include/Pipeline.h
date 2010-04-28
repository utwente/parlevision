#ifndef PIPELINE_H
#define PIPELINE_H

#include <map>
#include <list>
#include <QThread>
#include "RefPtr.h"
#include "RefCounted.h"
#include "PinConnection.h"

namespace plv
{
    class PipelineElement;
    class Pin;
    class InputPin;
    class OutputPin;

    class Pipeline : public QThread, public RefCounted
    {
        typedef std::map<int , RefPtr<PipelineElement> > PipelineElementMap;

    public:
        Pipeline();

        /** Add the PipelineElement to this Pipeline.
          * This results in the Pipeline calling setPipeline on the element
          * and m_children containing the element.
          * @return a unique ID for this element within this pipeline.
          */
        int add( PipelineElement* child );
        void remove( PipelineElement* child );
        void remove( int id );
        void init();

        /** Create a PinConnnection between the given InputPin and outputPin
          */
        void connectPins(OutputPin* outputPin, InputPin* inputPin);

    protected:
        PipelineElementMap m_children;
        std::list< RefPtr<PinConnection> > m_connections;

        ~Pipeline();

        /**
          * The QThread run loop
          */
        virtual void run();
    private:
        int m_idCounter;

    public slots:
        void start();
        void stop();

    };
}
#endif // PIPELINE_H
