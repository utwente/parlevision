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

#ifndef MULTIPLY_H
#define MULTIPLY_H

#include <plvcore/PipelineProcessor.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    /**
      * Does a per-element multiplication of two images and a scale factor.
      */
    class Multiply : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_CLASSINFO("author", "Niek Hoeijmakers")
        Q_CLASSINFO("name", "A multiply B")
        Q_CLASSINFO("description", "A processor that does a per-element multiplication of two images and a scale factor. "
                    "Input image B requires the same properties as input image A. The generated output wil have the same "
                    "properties as input image A. "
                    "See OpenCV reference for more information. "
                    "<a href='http://opencv.willowgarage.com/documentation/cpp/core_operations_on_arrays.html#cv-multiply'>"
                    "http://opencv.willowgarage.com/documentation/cpp/core_operations_on_arrays.html#cv-multiply"
                    "</a>");

        Q_PROPERTY( double scale READ getScale WRITE setScale NOTIFY scaleChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_PROCESSOR

    public:
        Multiply();
        virtual ~Multiply();

        /** propery methods */
        double getScale();

    signals:
        void scaleChanged(double newValue);

    public slots:
        void setScale(double d);

    private:
        plv::CvMatDataInputPin* m_inputPin1; // image input A
        plv::CvMatDataInputPin* m_inputPin2; // image input B
        plv::CvMatDataOutputPin* m_outputPin; // image output

        double m_scale;
    };
}

#endif // MULTIPLY_H
