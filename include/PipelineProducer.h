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
    };

}

#endif // PIPELINEPRODUCER_H
