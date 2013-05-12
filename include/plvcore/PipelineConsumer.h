/**
  * Copyright (C)2012 by Richard Loos
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

#ifndef PIPELINECONSUMER_H
#define PIPELINECONSUMER_H

#include "IPipelineConsumer.h"

namespace plv
{
    class PipelineConsumer : public IPipelineConsumer
    {
    protected:
        PipelineConsumerDelegate m_consumerDelegate;

        /** adds the methods and implementation necessary for the IPipelineConsumer interface */
        PLV_CONSUMER_INTERFACE_IMP_BY_DELEGATE(m_consumerDelegate)

    public:
        PipelineConsumer();
        virtual ~PipelineConsumer();

        /** implementation for definition in PipelineElement */
        virtual bool requiredInputPinsConnected() const;
    };
}

#endif
