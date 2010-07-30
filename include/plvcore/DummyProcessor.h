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

        Q_CLASSINFO("author", "Michel & Richard")
        Q_CLASSINFO("name", "Dummy")
        Q_CLASSINFO("description", "A simple processor to demonstrate how to implement your own processors. It doesn't do much, but flip your images.");

        Q_PROPERTY( int someInt READ getSomeInt WRITE setSomeInt NOTIFY someIntChanged  )
        Q_PROPERTY( double someDouble READ getSomeDouble WRITE setSomeDouble NOTIFY someDoubleChanged  )
        Q_PROPERTY( bool someBool READ getSomeBool WRITE setSomeBool NOTIFY someBoolChanged  )
        Q_PROPERTY( QString someString READ getSomeString WRITE setSomeString NOTIFY someStringChanged )
        Q_PROPERTY( int someVarWithNr1 READ getSomeVarWithNr1 WRITE setSomeVarWithNr1 NOTIFY someVarWithNr1Changed )
        Q_PROPERTY( int someVarWithNr2 READ getSomeVarWithNr2 WRITE setSomeVarWithNr2 NOTIFY someVarWithNr2Changed )
        //Q_PROPERTY(Priority priority READ getPriority WRITE setPriority)
        //Q_ENUMS(Priority)

//        enum Priority { High, Low, VeryHigh, VeryLow };

//        void setPriority(Priority priority)
//        {
//            m_priority = priority;
//            emit( priorityChanged );
//        }

//        Priority priority() const
//        {
//            return m_priority;
//        }


    public:
        DummyProcessor();
        ~DummyProcessor();

        virtual void init() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        int getSomeInt() { return m_someInt; }
        double getSomeDouble() { return m_someDouble; }
        bool getSomeBool() { return m_someBool; }
        QString getSomeString() { return m_someString; }

        int getSomeVarWithNr1() { return m_someVarWithNr1; }
        int getSomeVarWithNr2() { return m_someVarWithNr2; }

    signals:
        void someIntChanged(int newValue);
        void someDoubleChanged(double newValue);
        void someBoolChanged(bool newValue);
        void someStringChanged(QString newValue);
        void someVarWithNr1Changed(int var);
        void someVarWithNr2Changed(int var);

    public slots:
        void setSomeInt(int i) {m_someInt = i; emit(someIntChanged(i));}
        void setSomeDouble(double d) {m_someDouble = d; emit(someDoubleChanged(d));}
        void setSomeBool(bool b) {m_someBool = b; emit(someBoolChanged(b));}
        void setSomeString(QString s) {m_someString = s; emit(someStringChanged(s));}
        void setSomeVarWithNr1(int var) { m_someVarWithNr1 = var; emit( someVarWithNr1Changed(var) ); }
        void setSomeVarWithNr2(int var) { m_someVarWithNr2 = var; emit( someVarWithNr2Changed(var) ); }

    private:
        InputPin<OpenCVImage>* m_inputPin;
        InputPin<OpenCVImage>* m_inputPinOptional;
        OutputPin<OpenCVImage>* m_outputPin;

        int m_someInt;
        double m_someDouble;
        bool m_someBool;
        QString m_someString;
        int m_someVarWithNr1;
        int m_someVarWithNr2;
    };

}

#endif // DUMMYPROCESSOR_H
