#ifndef PIPELINEPRODUCER_H
#define PIPELINEPRODUCER_H

#include <map>
#include "RefPtr.h"
#include "PipelineElement.h"

namespace plv {

    class Pipeline;

    class PipelineProducer : public PipelineElement
    {
    public:
        PipelineProducer( Pipeline* parent );

        virtual void produce() = 0;

        //Pin<MyType> in = getInputPin( "MyPin" );

    protected:
        virtual ~PipelineProducer();
    };

}

#endif // PIPELINEPRODUCER_H
