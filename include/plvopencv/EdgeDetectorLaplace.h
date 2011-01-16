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

#ifndef EDGEDETECTORLAPLACE_H
#define EDGEDETECTORLAPLACE_H

#include <plvcore/PipelineProcessor.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    class EdgeDetectorLaplace : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( EdgeDetectorLaplace )
        Q_CLASSINFO("author", "Richard, Wim, Dennis")
        Q_CLASSINFO("name", "Laplace edge detector")
        Q_CLASSINFO("description", "Edge detection using the Laplace method."
                    "See OpenCV manual at "
                    "<a href='http://opencv.willowgarage.com/documentation/image_filtering.html?highlight=cvlaplace#cvLaplace'>"
                    "http://opencv.willowgarage.com/documentation/image_filtering.html?highlight=cvlaplace#cvLaplace"
                    "</a> "
                    "for meaning of parameters and info." );

        Q_PROPERTY( int apertureSize READ getApertureSize WRITE setApertureSize NOTIFY apertureSizeChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        EdgeDetectorLaplace();
        virtual ~EdgeDetectorLaplace();

        /** propery methods */
        int getApertureSize() { QMutexLocker lock( m_propertyMutex ); return m_apertureSize; }

    signals:
        void apertureSizeChanged(int newValue);

    public slots:
        void setApertureSize(int i);

    private:
        plv::CvMatDataInputPin* m_inputPin;
        plv::CvMatDataOutputPin* m_outputPin;
        int m_apertureSize;
    };

}

#endif // EDGEDETECTORLAPLACE_H
