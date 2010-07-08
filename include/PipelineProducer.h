#ifndef PIPELINEPRODUCER_H
#define PIPELINEPRODUCER_H

#include <map>
#include "RefPtr.h"
#include "PipelineElement.h"

namespace plv {

    class PipelineProducer : public PipelineElement
    {
    public:
        PipelineProducer();
        virtual ~PipelineProducer();
        PipelineProducer(const PipelineProducer&);

        virtual void start() = 0;
        virtual void stop() = 0;
    };

}

#endif // PIPELINEPRODUCER_H
