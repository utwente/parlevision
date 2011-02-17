/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvcore module of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

#ifndef PIPELINEPRODUCER_H
#define PIPELINEPRODUCER_H

#include "PipelineElement.h"

/** Utility macro for implemented pure abstract methods in sub classes */
#define PLV_PIPELINE_PRODUCER \
public: \
    virtual bool readyToProduce() const; \
    virtual bool produce();

namespace plv
{
    class PLVCORE_EXPORT PipelineProducer : public PipelineElement
    {
        Q_OBJECT

    public:
        friend class Pipeline;

        PipelineProducer();
        virtual ~PipelineProducer();

        /** returns true when producer can produce. Will be polled by the pipeline */
        virtual bool readyToProduce() const = 0;

        /** does the actual producing */
        virtual bool produce() = 0;

    protected:
        /** calls pre and post of output pins and calls produce of implementation */
        virtual bool __ready( unsigned int& serial );

        /** calls the produce method of the implementation and sets the serial number
            correctly */
        virtual bool __process( unsigned int serial );
    };

}

#endif // PIPELINEPRODUCER_H
