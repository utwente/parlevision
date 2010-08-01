#ifndef EDGEDETECTORLAPLACE_H
#define EDGEDETECTORLAPLACE_H

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {

    class Pipeline;
    class OpenCVImage;

    class EdgeDetectorLaplace : public PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Ported from old version by Wim")
        Q_CLASSINFO("name", "Edge detector Laplace")
        Q_CLASSINFO("description", "Edge detection using the Laplace method.");

        Q_PROPERTY( int apertureSize READ getApertureSize WRITE setApertureSize NOTIFY apertureSizeChanged )

    public:
        EdgeDetectorLaplace();
        ~EdgeDetectorLaplace();

        virtual void init() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        int getApertureSize() { return m_apertureSize; }

    signals:
        void apertureSizeChanged(int newValue);

    public slots:
        void setApertureSize(int i);

    private:
        int nearestOdd(int i);

        InputPin<OpenCVImage>* m_inputPin;
        OutputPin<OpenCVImage>* m_outputPin;


        int m_apertureSize;
    };

}

#endif // EDGEDETECTORLAPLACE_H
