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
#include <plvcore/Pin.h>

namespace plv
{
    class OpenCVImage;
}

namespace plvopencv
{
    class ImageFlip : public plv::PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Ported from old version by Wim & Dennis")
        Q_CLASSINFO("name", "Flip")
        Q_CLASSINFO("description", "Flip image. FlipX means \"flip around x-axis\". Same for FlipY.");

        Q_PROPERTY( bool flipX READ getFlipX WRITE setFlipX NOTIFY flipXChanged  )
        Q_PROPERTY( bool flipY READ getFlipY WRITE setFlipY NOTIFY flipYChanged  )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        ImageFlip();
        virtual ~ImageFlip();

        /** propery methods */
        bool getFlipX() { return m_flipX; }
        bool getFlipY() { return m_flipY; }

    signals:
        void flipXChanged(bool newValue);
        void flipYChanged(bool newValue);

    public slots:
        void setFlipX(bool b);
        void setFlipY(bool b);

    private:

        plv::InputPin<plv::OpenCVImage>* m_inputPin;
        plv::OutputPin<plv::OpenCVImage>* m_outputPin;

        bool m_flipX;
        bool m_flipY;
        int m_method;
    };
}
#endif // IMAGEFLIP_H
