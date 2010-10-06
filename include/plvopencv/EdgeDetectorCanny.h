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

#ifndef EDGEDETECTORCANNY_H
#define EDGEDETECTORCANNY_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Pin.h>

namespace plvopencv
{
    class OpenCVImage;

    class EdgeDetectorCanny : public plv::PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Wim, Dennis, Richard")
        Q_CLASSINFO("name", "Edge detector Canny")
        Q_CLASSINFO("description", "Edge detection using the Canny method.");

        Q_PROPERTY( int apertureSize READ getApertureSize WRITE setApertureSize NOTIFY apertureSizeChanged )
        Q_PROPERTY( double thresholdLow READ getThresholdLow WRITE setThresholdLow NOTIFY thresholdLowChanged )
        Q_PROPERTY( double thresholdHigh READ getThresholdHigh WRITE setThresholdHigh NOTIFY thresholdHighChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        EdgeDetectorCanny();
        virtual ~EdgeDetectorCanny();

        /** propery methods */
        int getApertureSize() { return m_apertureSize; }
        double getThresholdLow() { return m_thresholdLow; }
        double getThresholdHigh() { return m_thresholdHigh; }

    signals:
        void apertureSizeChanged(int newValue);
        void thresholdLowChanged(double newValue);
        void thresholdHighChanged(double newValue);

    public slots:
        void setApertureSize(int i);
        void setThresholdLow (double newValue) { m_thresholdLow  = newValue; emit(thresholdLowChanged(newValue)); }
        void setThresholdHigh(double newValue) { m_thresholdHigh = newValue; emit(thresholdHighChanged(newValue)); }

    private:
        int nearestOdd(int i);

        plv::InputPin<OpenCVImage>* m_inputPin;
        plv::OutputPin<OpenCVImage>* m_outputPin;

        int m_apertureSize;
        double m_thresholdLow;
        double m_thresholdHigh;
    };
}
#endif // EDGEDETECTORCANNY_H
