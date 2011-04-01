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

#ifndef MASK_H
#define MASK_H

#include <plvcore/PipelineProcessor.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    /**
      * Mask two images.
      */
    class Mask : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( Mask )
        Q_CLASSINFO("author", "Richard")
        Q_CLASSINFO("name", "Mask")
        Q_CLASSINFO("description", "Applies a mask to the input. Mask should be single channel and CV_8U." )

        Q_PROPERTY( bool negative READ getNegative WRITE setNegative NOTIFY negativeChanged )

        /** required standard method declaration for plv::PipelineProcessor */
        PLV_PIPELINE_PROCESSOR

    public:
        Mask();
        virtual ~Mask();

        /** propery methods */
        bool getNegative() const;

    signals:
        void negativeChanged(bool d);

    public slots:
        void setNegative(bool a);

    private:
        plv::CvMatDataInputPin* m_inputPin1;
        plv::CvMatDataInputPin* m_inputPin2;
        plv::CvMatDataOutputPin* m_outputPin;

        bool m_negative;
    };
}
#endif // MASK_H
