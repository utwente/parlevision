#ifndef IMAGEFLIP_H
#define IMAGEFLIP_H

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {
    class Pipeline;
    class OpenCVImage;

    class ImageFlip : public PipelineProcessor
    {
        Q_OBJECT
        Q_PROPERTY( int apertureSize READ getApertureSize WRITE setApertureSize NOTIFY apertureSizeChanged )
        Q_PROPERTY( int someInt READ getSomeInt WRITE setSomeInt NOTIFY someIntChanged  )
        Q_PROPERTY( bool someBool READ getSomeBool WRITE setSomeBool NOTIFY someBoolChanged  )

    public:
        ImageFlip();
        ~ImageFlip();

        virtual void init() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        int getApertureSize() { return m_apertureSize; }
        int getSomeInt() { return m_someInt; }
        bool getSomeBool() { return m_someBool; }

    signals:
        void apertureSizeChanged(int newValue);
        void someIntChanged(int newValue);
        void someBoolChanged(bool newValue);

    public slots:
        void setApertureSize(int i);
        void setSomeInt(int i) {m_someInt = i; emit(someIntChanged(i));}
        void setSomeBool(bool b) {m_someBool = b; emit(someBoolChanged(b));}

    private:
        int nearestOdd(int i);

        InputPin<OpenCVImage>* m_inputPin;
        OutputPin<OpenCVImage>* m_outputPin;

        int m_apertureSize;
        int m_someInt;
        bool m_someBool;
    };
}
#endif // IMAGEFLIP_H
