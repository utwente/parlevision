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

#ifndef PIPELINEPROCESSOR_H
#define PIPELINEPROCESSOR_H

#include "PipelineElement.h"

/** Utility macro for implemented pure abstract methods in sub classes */
#define PLV_PIPELINE_PROCESSOR \
public: \
    virtual void init(); \
    virtual void deinit() throw (); \
    virtual void start(); \
    virtual void stop(); \
    virtual void process();

namespace plv
{
    class PLVCORE_EXPORT PipelineProcessor : public PipelineElement
    {
        Q_OBJECT

    public:
        friend class Pipeline;

        PipelineProcessor();
        virtual ~PipelineProcessor();

        /** This method can be overridden in PipelineProcessor derived classes to
          * change the default behaviour that the processor is ready to process when
          * there is input available on all connected synchronous pins. Warning:
          * use with caution! */
        //virtual bool readyToProcess() const { return true; }

        /** does the actual processing */
        virtual void process() = 0;

    private:
        virtual bool __ready( unsigned int& nextSerial );
        virtual void __process( unsigned int serial );
    };

}

#endif // PIPELINEPROCESSOR_H
