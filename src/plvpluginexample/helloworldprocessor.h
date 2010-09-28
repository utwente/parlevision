#ifndef HELLOWORLDPROCESSOR_H
#define HELLOWORLDPROCESSOR_H

#include <QObject>
#include <plvcore/PipelineElement.h>
#include <plvcore/RefPtr.h>
#include <plvcore/Pin.h>

namespace plvopencv
{
    class OpenCVImage;
}

using namespace plv;
using namespace plvopencv;

class HelloWorldProcessor : public PipelineElement
{
    Q_OBJECT
    Q_PROPERTY( int someInt READ getSomeInt WRITE setSomeInt NOTIFY someIntChanged  )
    Q_PROPERTY( double someDouble READ getSomeDouble WRITE setSomeDouble NOTIFY someDoubleChanged  )
    Q_PROPERTY( bool someBool READ getSomeBool WRITE setSomeBool NOTIFY someBoolChanged  )
    Q_PROPERTY( QString someString READ getSomeString WRITE setSomeString NOTIFY someStringChanged )

public:
    HelloWorldProcessor();
    virtual ~HelloWorldProcessor();

    virtual void init() throw (PipelineException);
    virtual void deinit() throw ();
    virtual void start() throw (PipelineException);
    virtual void stop() throw (PipelineException);
    virtual bool isReadyForProcessing() const;
    virtual void process();

    /** propery methods */
    int getSomeInt() { return m_someInt; }
    double getSomeDouble() { return m_someDouble; }
    bool getSomeBool() { return m_someBool; }
    QString getSomeString() { return m_someString; }

signals:
    void someIntChanged(int newValue);
    void someDoubleChanged(double newValue);
    void someBoolChanged(bool newValue);
    void someStringChanged(QString newValue);

public slots:
    void setSomeInt(int i) {m_someInt = i; emit(someIntChanged(i));}
    void setSomeDouble(double d) {m_someDouble = d; emit(someDoubleChanged(d));}
    void setSomeBool(bool b) {m_someBool = b; emit(someBoolChanged(b));}
    void setSomeString(QString s) {m_someString = s; emit(someStringChanged(s));}

private:
    RefPtr< InputPin<OpenCVImage> > m_inputPin;
    RefPtr< OutputPin<OpenCVImage> > m_outputPin;

    int m_someInt;
    double m_someDouble;
    bool m_someBool;
    QString m_someString;

};

#endif // HELLOWORLDPROCESSOR_H
