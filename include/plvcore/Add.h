#ifndef ADD_H
#define ADD_H

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {
    class Pipeline;
    class OpenCVImage;

    /**
      * Addtwo images.
      */
    class Add : public PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Ported from old version by Dennis")
        Q_CLASSINFO("name", "A add B")
        Q_CLASSINFO("description", "A simple processor to add two images. Optionally, the result is normalized.");

        Q_PROPERTY( bool normalizeAfterAdd READ getNormalizeAfterAdd WRITE setNormalizeAfterAdd NOTIFY normalizeAfterAddChanged  )

    public:
        Add();
        ~Add();

        virtual void init() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        bool getNormalizeAfterAdd() { return m_normalize; }


    signals:
        void normalizeAfterAddChanged(bool newValue);

    public slots:
        void setNormalizeAfterAdd(bool b) {m_normalize = b; emit(normalizeAfterAddChanged(b));}

    private:

        InputPin<OpenCVImage>* m_inputPin1;
        InputPin<OpenCVImage>* m_inputPin2;
        OutputPin<OpenCVImage>* m_outputPin;

        bool m_normalize;





    };
}
#endif // ADD_H
