/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvopencv module of ParleVision.
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
#ifndef XOR_H
#define XOR_H

#include <plvcore/PipelineProcessor.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    /** Take a bitwise XOR of two images. */
    class Xor : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_CLASSINFO("author", "Richard Loos")
        Q_CLASSINFO("name", "A XOR B")
        Q_CLASSINFO("description", "A processor that does a bitwise XOR operation on the elements of two images.");

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_PROCESSOR

    public:
        Xor();
        virtual ~Xor();

    private:
        plv::CvMatDataInputPin* m_inputPin1; // image input A
        plv::CvMatDataInputPin* m_inputPin2; // image input B
        plv::CvMatDataOutputPin* m_outputPin; // image output
    };
}

#endif // XOR_H
