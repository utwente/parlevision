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

#include <plvcore/PipelineProcessor.h>

namespace plv
{
    class OpenCVImage;
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    /**
      * Add two images.
      */
    class Add : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( Add )
        Q_CLASSINFO("author", "Ported from old version by Dennis")
        Q_CLASSINFO("name", "A add B")
        Q_CLASSINFO("description", "A simple processor to add two images. Optionally, the result is normalized.");
        Q_PROPERTY( bool normalizeAfterAdd READ getNormalizeAfterAdd WRITE setNormalizeAfterAdd NOTIFY normalizeAfterAddChanged  )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        Add();
        virtual ~Add();

        /** propery methods */
        bool getNormalizeAfterAdd() { return m_normalize; }

    signals:
        void normalizeAfterAddChanged(bool newValue);

    public slots:
        void setNormalizeAfterAdd(bool b) {m_normalize = b; emit(normalizeAfterAddChanged(b));}

    private:
        plv::CvMatDataInputPin* m_inputPin1;
        plv::CvMatDataInputPin* m_inputPin2;
        plv::CvMatDataOutputPin* m_outputPin;
        bool m_normalize;
    };
}
#endif // ADD_H
