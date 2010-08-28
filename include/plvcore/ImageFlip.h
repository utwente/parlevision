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

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {
    class Pipeline;
    class OpenCVImage;

    class ImageFlip : public PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Ported from old version by Wim & Dennis")
        Q_CLASSINFO("name", "Flip")
        Q_CLASSINFO("description", "Flip image. FlipX means \"flip around x-axis\". Same for FlipY.");

        Q_PROPERTY( bool flipX READ getFlipX WRITE setFlipX NOTIFY flipXChanged  )
        Q_PROPERTY( bool flipY READ getFlipY WRITE setFlipY NOTIFY flipYChanged  )

    public:
        ImageFlip();
        ~ImageFlip();

        virtual void init() throw (PipelineException);
        virtual void start() throw (PipelineException);
        virtual void stop() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

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

        InputPin<OpenCVImage>* m_inputPin;
        OutputPin<OpenCVImage>* m_outputPin;

        bool m_flipX;
        bool m_flipY;
        int m_method;
    };
}
#endif // IMAGEFLIP_H
