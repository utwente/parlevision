#ifndef PIPELINEPROCESSOR_H
#define PIPELINEPROCESSOR_H

#include "PipelineElement.h"

namespace plv {

    class PipelineProcessor : public PipelineElement
    {
    public:
        PipelineProcessor();

        void process() throw (PlvException);

        virtual PlvPipelineElementState checkConfig();


    protected:
        ~PipelineProcessor();
    };

}

#endif // PIPELINEPROCESSOR_H
