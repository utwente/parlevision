#ifndef DUMMYPROCESSOR_H
#define DUMMYPROCESSOR_H

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {

    class Pipeline;
    class OpenCVImage;

    class DummyProcessor : public PipelineProcessor
    {
    public:
        DummyProcessor();
        ~DummyProcessor();

        virtual PlvPipelineElementState init();
        virtual PlvPipelineElementState checkConfig();
        virtual void process();

    private:
        TypedInputPin<OpenCVImage>* m_inputPin;
        TypedOutputPin<OpenCVImage>* m_outputPin;

    };

}

#endif // DUMMYPROCESSOR_H
