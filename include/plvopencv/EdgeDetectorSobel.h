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

#ifndef EDGEDETECTORSOBEL_H
#define EDGEDETECTORSOBEL_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Pin.h>

namespace plv
{
    class OpenCVImage;
}

namespace plvopencv
{
    class EdgeDetectorSobel : public plv::PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Ported from old version by Wim & Dennis")
        Q_CLASSINFO("name", "Edge detector Sobel")
        Q_CLASSINFO("description", "Edge detection using the Sobel method.");

        Q_PROPERTY( int apertureSize READ getApertureSize WRITE setApertureSize NOTIFY apertureSizeChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        EdgeDetectorSobel();
        virtual ~EdgeDetectorSobel();

        /** propery methods */
        int getApertureSize() { return m_apertureSize; }
    signals:
        void apertureSizeChanged(int newValue);

    public slots:
        void setApertureSize(int i);

    private:
        plv::InputPin<plv::OpenCVImage>* m_inputPin;
        plv::OutputPin<plv::OpenCVImage>* m_outputPin;
        int m_apertureSize;

    };
}
#endif // EDGEDETECTORSOBEL_H
