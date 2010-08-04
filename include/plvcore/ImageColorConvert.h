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
        //Q_PROPERTY( int conversionType READ getConversionType WRITE setConversionType NOTIFY conversionType )
        Q_PROPERTY( plv::Enum conversionType READ getConversionType WRITE setConversionType NOTIFY conversionTypeChanged )

    public:
        ImageColorConvert();
        ~ImageColorConvert();

        virtual void init() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        plv::Enum getConversionType() { return m_conversionType; }

    signals:
        void conversionTypeChanged(plv::Enum newValue);

    public slots:
        void setConversionType(plv::Enum e) { m_conversionType = e; emit(conversionTypeChanged(e)); }

    private:
        int nearestOdd(int i);

        InputPin<OpenCVImage>* m_inputPin;
        OutputPin<OpenCVImage>* m_outputPin;

        plv::Enum m_conversionType;
    };
}
#endif // IMAGECOLORCONVERT_H
