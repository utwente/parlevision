#ifndef PIPELINEPROCESSOR_H
#define PIPELINEPROCESSOR_H

#include "PipelineElement.h"

namespace plv {

    class Pipeline;

    class PipelineProcessor : public PipelineElement
    {
    public:
        PipelineProcessor( Pipeline* parent );

        /** Process function needs to be implemented for
          * PipelineProcessor implementations. This method
          * gets called when inputs on all pins are ready.
          */
        virtual void process() = 0;

        // pure virtual methods inherited from PipelineElement
        // need to be implemented in this class
        virtual PlvPipelineElementState init() = 0;
        virtual PlvPipelineElementState checkConfig() = 0;

    protected:
        ~PipelineProcessor();
    };

}

#endif // PIPELINEPROCESSOR_H
