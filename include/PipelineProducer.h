#ifndef PIPELINEPRODUCER_H
#define PIPELINEPRODUCER_H

#include <map>
#include <exception>
#include "RefPtr.h"
#include "PipelineElement.h"

namespace plv {

    class PipelineProducer : public PipelineElement
    {
    public:
        PipelineProducer();

        virtual void produce() throw (std::exception) = 0;

    protected:
        virtual ~PipelineProducer();
    };

}

#endif // PIPELINEPRODUCER_H
