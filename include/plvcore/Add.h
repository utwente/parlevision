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

#ifndef ADD_H
#define ADD_H

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv
{
    class Pipeline;
    class OpenCVImage;

    /**
      * Add two images.
      */
    class Add : public PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Ported from old version by Dennis")
        Q_CLASSINFO("name", "A add B")
        Q_CLASSINFO("description", "A simple processor to add two images. Optionally, the result is normalized.");

        Q_PROPERTY( bool normalizeAfterAdd READ getNormalizeAfterAdd WRITE setNormalizeAfterAdd NOTIFY normalizeAfterAddChanged  )

    public:
        Add();
        ~Add();

        virtual void init() throw (PipelineException);
        virtual void start() throw (PipelineException);
        virtual void stop() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        bool getNormalizeAfterAdd() { return m_normalize; }

    signals:
        void normalizeAfterAddChanged(bool newValue);

    public slots:
        void setNormalizeAfterAdd(bool b) {m_normalize = b; emit(normalizeAfterAddChanged(b));}

    private:
        InputPin<OpenCVImage>* m_inputPin1;
        InputPin<OpenCVImage>* m_inputPin2;
        OutputPin<OpenCVImage>* m_outputPin;
        bool m_normalize;
    };
}
#endif // ADD_H
