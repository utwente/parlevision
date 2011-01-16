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

#ifndef IMAGEFLIP_H
#define IMAGEFLIP_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Enum.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    class ImageFlip : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( ImageFlip )
        Q_CLASSINFO("author", "Ported from old version by Wim & Dennis")
        Q_CLASSINFO("name", "Flip")
        Q_CLASSINFO("description", "Flip image. FlipX means \"flip around x-axis\". Same for FlipY.");

        Q_PROPERTY( plv::Enum method READ getMethod WRITE setMethod NOTIFY methodChanged  )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        ImageFlip();
        virtual ~ImageFlip();

        /** propery methods */
        plv::Enum getMethod() const;

    signals:
        void methodChanged(plv::Enum e);

    public slots:
        void setMethod(plv::Enum e);

    private:
        plv::CvMatDataInputPin* m_inputPin;
        plv::CvMatDataOutputPin* m_outputPin;

        plv::Enum m_method;
    };
}
#endif // IMAGEFLIP_H
