#ifndef SUB_H
#define SUB_H

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {
    class Pipeline;
    class OpenCVImage;

    /**
      * Sub of two images.
      */
    class Sub : public PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Ported from old version by Dennis")
        Q_CLASSINFO("name", "A sub B")
        Q_CLASSINFO("description", "A simple processor to subtract two images.");

    public:
        Sub();
        ~Sub();

        virtual void init() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */

    signals:

    public slots:

    private:

        InputPin<OpenCVImage>* m_inputPin1;
        InputPin<OpenCVImage>* m_inputPin2;
        OutputPin<OpenCVImage>* m_outputPin;


    };
}
#endif // SUB_H
