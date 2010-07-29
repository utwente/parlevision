#ifndef IMAGESMOOTH_H
#define IMAGESMOOTH_H

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {
    class Pipeline;
    class OpenCVImage;

    class ImageSmooth : public PipelineProcessor
    {
        Q_OBJECT
        Q_PROPERTY( int apertureSize READ getApertureSize WRITE setApertureSize NOTIFY apertureSizeChanged )

    public:
        ImageSmooth();
        ~ImageSmooth();

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
#endif // IMAGEFLIP_H
