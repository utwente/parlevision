#ifndef PIPELINE_H
#define PIPELINE_H

#include <map>
#include "RefPtr.h"
#include "RefCounted.h"

namespace plv
{
    class PipelineElement;

    class Pipeline : public RefCounted
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

    protected:
        PipelineElementMap m_children;

        ~Pipeline();
    private:
        int m_idCounter;
    };
}
#endif // PIPELINE_H
