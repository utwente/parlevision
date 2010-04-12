#ifndef CAMERAPRODUCER_H
#define CAMERAPRODUCER_H

#include "PipelineProducer.h"

namespace plv {

    class CameraProducer : public PipelineProducer
    {
    public:
        CameraProducer();

        virtual void produce() throw (std::exception);

    protected:
        virtual ~CameraProducer();
    };

}

#endif // CAMERAPRODUCER_H
