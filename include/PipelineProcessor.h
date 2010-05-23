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
        // need to be implemented in this class
        virtual PlvPipelineElementState init() = 0;
        virtual PlvPipelineElementState checkConfig() = 0;

    };

}

#endif // PIPELINEPROCESSOR_H
