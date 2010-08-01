#ifndef DIFF_H
#define DIFF_H

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {
    class Pipeline;
    class OpenCVImage;

    /**
      * Absolute diff of two images.
      */
    class Diff : public PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Ported from old version by Dennis")
        Q_CLASSINFO("name", "AbsDiff(A, B)")
        Q_CLASSINFO("description", "Calculate absolute difference of two images.");


    public:
        Diff();
        ~Diff();

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
#endif // DIFF_H
