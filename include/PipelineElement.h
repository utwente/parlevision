#ifndef PIPELINEELEMENT_H
#define PIPELINEELEMENT_H

#include <map>
#include <QString>

#include "RefPtr.h"

namespace plv {

//class RefCounted;
class Pin;

typedef enum _PlvPipelineElementState {
    PROCESSOR_OK,
    PROCESSOR_FUCKED_UP
} PlvPipelineElementState;

class PipelineElement : public RefCounted
{
    typedef std::map< QString, RefPtr< Pin > > InputPinMap;
    typedef std::map< QString, RefPtr< Pin > > OutputPinMap;

protected:
    InputPinMap  m_inputPins;
    OutputPinMap m_outputPins;

public:
    PipelineElement();

    virtual PlvPipelineElementState checkConfig() = 0;


//    inline const InputPinMap& getInputPins() const
//    {
//        return m_inputPins;
//    }
//
//    inline const OutputPinMap& getOutputPins() const
//    {
//        return m_outputPins;
//    }

protected:
    ~PipelineElement();
};

}

#endif // PIPELINEELEMENT_H
