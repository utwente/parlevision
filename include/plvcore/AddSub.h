#ifndef ADDSUB_H
#define ADDSUB_H

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {
    class Pipeline;
    class OpenCVImage;

    /**
      * Add / sub / diff of two images.
      * Actual operation determined by method parameter. See const values for avaliable methods
      */
    class AddSub : public PipelineProcessor
    {
        Q_OBJECT
        Q_PROPERTY( int method READ getMethod WRITE setMethod NOTIFY methodChanged )
        Q_PROPERTY( bool normalize READ getNormalize WRITE setNormalize NOTIFY normalizeChanged  )

    public:
        AddSub();
        ~AddSub();

        virtual void init() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        int getMethod() { return m_method; }
        bool getNormalize() { return m_normalize; }

        static const int METHOD_NOTHING = 0;
        static const int METHOD_ADD = 1;
        static const int METHOD_SUB = 2;
        static const int METHOD_DIFF = 3;
        static const int METHOD_MAX = 3;

    signals:
        void methodChanged(int newValue);
        void normalizeChanged(bool newValue);

    public slots:
        void setMethod(int i);
        void setNormalize(bool b) {m_normalize = b; emit(normalizeChanged(b));}

    private:

        InputPin<OpenCVImage>* m_inputPin1;
        InputPin<OpenCVImage>* m_inputPin2;
        OutputPin<OpenCVImage>* m_outputPin;

        int m_method;
        bool m_normalize;





    };
}
#endif // ADDSUB_H
