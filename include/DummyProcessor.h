#ifndef DUMMYPROCESSOR_H
#define DUMMYPROCESSOR_H

#include "PipelineProcessor.h"

namespace plv {

    class Pipeline;

    class DummyProcessor : public PipelineProcessor
    {
    public:
        DummyProcessor( Pipeline* parent );

        virtual PlvPipelineElementState init();
        virtual PlvPipelineElementState checkConfig();
        virtual void process();

    protected:
        ~DummyProcessor();
    };

}

#endif // DUMMYPROCESSOR_H
