#ifndef EDGEDETECTORSOBEL_H
#define EDGEDETECTORSOBEL_H

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {
    class Pipeline;
    class OpenCVImage;

    class EdgeDetectorSobel : public PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Ported from old version by Wim")
        Q_CLASSINFO("name", "Edge detector Sobel")
        Q_CLASSINFO("description", "Edge detection using the Sobel method.");


    public:
        EdgeDetectorSobel();
        ~EdgeDetectorSobel();

        virtual void init() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();


    signals:

    public slots:

    private:

        InputPin<OpenCVImage>* m_inputPin;
        OutputPin<OpenCVImage>* m_outputPin;
    };
}
#endif // EDGEDETECTORSOBEL_H
