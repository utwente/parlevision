#ifndef PIPELINEPROCESSOR_H
#define PIPELINEPROCESSOR_H

#include "PipelineElement.h"

namespace plv {

    class PipelineProcessor : public PipelineElement
    {
    public:
        PipelineProcessor();
        ~PipelineProcessor();

        // pure virtual methods inherited from PipelineElement
        // need to be implemented in derivations of this class
        virtual void init() throw (PipelineException) = 0;
        virtual bool isReadyForProcessing() const = 0;
        virtual void process() = 0;

        //virtual bool isBootstrapped() const = 0;
    };

}

#endif // PIPELINEPROCESSOR_H
