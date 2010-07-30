#ifndef IMAGECOLORCONVERT_H
#define IMAGECOLORCONVERT_H

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {
    class Pipeline;
    class OpenCVImage;

    class ImageColorConvert : public PipelineProcessor
    {
        Q_OBJECT
        Q_PROPERTY( int conversionType READ getConversionType WRITE setConversionType NOTIFY conversionType )

    public:
        ImageColorConvert();
        ~ImageColorConvert();

        virtual void init() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        int getConversionType() { return m_conversionType; }

    signals:
        void conversionTypeChanged(int newValue);

    public slots:
        void setConversionType(int i) { m_conversionType = i; emit(conversionTypeChanged(i)); }

    private:
        int nearestOdd(int i);

        InputPin<OpenCVImage>* m_inputPin;
        OutputPin<OpenCVImage>* m_outputPin;

        int m_conversionType;
    };
}
#endif // IMAGECOLORCONVERT_H
