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

#ifndef PIXELSUM_H
#define PIXELSUM_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Pin.h>
#include <opencv/cv.h>

Q_DECLARE_METATYPE( cv::Scalar );

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    /**
      * PixelSum of two images.
      */
    class PixelSum : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( PixelSum )
        Q_CLASSINFO("author", "Richard Loos")
        Q_CLASSINFO("name", "Pixel Sum")
        Q_CLASSINFO("description", "Does a pixel sum");

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        PixelSum();
        virtual ~PixelSum();

    private:
        plv::CvMatDataInputPin* m_inputPin;
        plv::OutputPin<cv::Scalar>* m_outputPin;
    };
}
#endif // PIXELSUM_H
