#ifndef CAMERAPRODUCER_H
#define CAMERAPRODUCER_H

#include "PipelineProducer.h"

namespace plv {

    class CameraProducer : public PipelineProducer
    {
    public:
        CameraProducer( Pipeline* parent );

        virtual void produce();

    protected:
        virtual ~CameraProducer();
    };

}

#endif // CAMERAPRODUCER_H
