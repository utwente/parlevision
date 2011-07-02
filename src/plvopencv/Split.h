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
#ifndef SPLIT_H
#define SPLIT_H

#include <plvcore/PipelineProcessor.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    /** Takes one multi channel image and splits it in multiple single channel images. */
    class Split : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_CLASSINFO("author", "Niek Hoeijmakers")
        Q_CLASSINFO("name", "Split Channels")
        Q_CLASSINFO("description", "A processor that splits a mutli-channel image into several single channel images. "
                        "The number it will be split in is dependent on the number of channels in the image. "
                        "Connect the outputs for the required number of images. Starting in sequence from the first output."
                        "See OpenCV reference for more information. "
                        "<a href='http://opencv.willowgarage.com/documentation/cpp/core_operations_on_arrays.html#cv-split'>"
                        "http://opencv.willowgarage.com/documentation/cpp/core_operations_on_arrays.html#cv-split"
                        "</a>");

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_PROCESSOR

    public:
        Split();
        virtual ~Split();

    private:
        plv::CvMatDataInputPin* m_inputPin;    // multi channel image input
        plv::CvMatDataOutputPin* m_outputPin0; // single channel image output 0
        plv::CvMatDataOutputPin* m_outputPin1; // single channel image output 1
        plv::CvMatDataOutputPin* m_outputPin2; // single channel image output 2
        plv::CvMatDataOutputPin* m_outputPin3; // single channel image output 3
    };
}

#endif // SPLIT_H
