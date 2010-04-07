#ifndef DUMMYPROCESSOR_H
#define DUMMYPROCESSOR_H

#include "PipelineProcessor.h"

namespace plv {

class DummyProcessor : public PipelineProcessor
{
public:
    DummyProcessor();
    ~DummyProcessor();
};

}

#endif // DUMMYPROCESSOR_H
