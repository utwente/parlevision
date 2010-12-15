#ifndef PLVTESTPRODUCER_H
#define PLVTESTPRODUCER_H

#include <plvcore/PipelineProducer.h>
#include <plvcore/Pin.h>

class TestProducer : public plv::PipelineProducer
{
    Q_OBJECT
    Q_DISABLE_COPY( TestProducer )

    Q_CLASSINFO("author", "Richard Loos")
    Q_CLASSINFO("name", "TestProducer")
    Q_CLASSINFO("description", "A producer to test stuff with")

    /** required standard method declaration for plv::PipelineElement */
    PLV_PIPELINE_ELEMENT

public:
    TestProducer();
    virtual ~TestProducer();

    virtual bool isReadyForProcessing() const { return true; }

signals:

public slots:

private:
    plv::OutputPin<int>* m_intOut;
    plv::OutputPin<QString>* m_stringOut;
    plv::OutputPin<float>* m_floatOut;
    plv::OutputPin<double>* m_doubleOut;
};

#endif // PLVTESTPRODUCER_H
