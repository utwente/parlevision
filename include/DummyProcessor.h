#ifndef DUMMYPROCESSOR_H
#define DUMMYPROCESSOR_H

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {

    class Pipeline;
    class OpenCVImage;

    class DummyProcessor : public PipelineProcessor
    {
        Q_OBJECT
    public:
        DummyProcessor();
        ~DummyProcessor();

        virtual bool init();
        virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();


    private:
        InputPin<OpenCVImage>* m_inputPin;
        OutputPin<OpenCVImage>* m_outputPin;

    };

}

#endif // DUMMYPROCESSOR_H
