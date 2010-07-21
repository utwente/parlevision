#ifndef DUMMYPROCESSOR_H
#define DUMMYPROCESSOR_H

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {

    class Pipeline;
    class OpenCVImage;

    class DummyProcessor : public PipelineProcessor
    {
        Q_OBJECT
        Q_PROPERTY( int someInt READ getSomeInt WRITE setSomeInt NOTIFY someIntChanged  )
        Q_PROPERTY( QString someString READ getSomeString WRITE setSomeString NOTIFY someStringChanged )

    public:
        DummyProcessor();
        ~DummyProcessor();

        virtual bool init();
        virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        int getSomeInt();
        void setSomeInt( int i );
        void setSomeString( QString s );
        QString getSomeString();

    signals:
        void someIntChanged(int newValue);
        void someStringChanged(QString newValue);


    private:
        InputPin<OpenCVImage>* m_inputPin;
        OutputPin<OpenCVImage>* m_outputPin;

        QString m_someString;
        int m_someInt;

    };

}

#endif // DUMMYPROCESSOR_H
