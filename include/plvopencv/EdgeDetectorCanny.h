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

namespace plv
{
    class OpenCVImage;
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    class EdgeDetectorCanny : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( EdgeDetectorCanny )
        Q_CLASSINFO("author", "Wim, Dennis, Richard")
        Q_CLASSINFO("name", "Canny edge detector")
        Q_CLASSINFO("description", "Edge detection using the Canny method. See "
                    "<a href='http://opencv.willowgarage.com/documentation/cpp/imgproc_feature_detection.html?highlight=canny#Canny'>"
                    "OpenCV reference"
                    "</a> for a detailed explanation of the parameters.");

        Q_PROPERTY( int apertureSize READ getApertureSize WRITE setApertureSize NOTIFY apertureSizeChanged )
        Q_PROPERTY( double thresholdLow READ getThresholdLow WRITE setThresholdLow NOTIFY thresholdLowChanged )
        Q_PROPERTY( double thresholdHigh READ getThresholdHigh WRITE setThresholdHigh NOTIFY thresholdHighChanged )
        Q_PROPERTY( bool l2Gradient READ getL2Gradient WRITE setL2Gradient NOTIFY l2GradientChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        EdgeDetectorCanny();
        virtual ~EdgeDetectorCanny();

        /** propery methods */
        int getApertureSize() const;
        double getThresholdLow() const;
        double getThresholdHigh() const;
        bool getL2Gradient() const;

    signals:
        void apertureSizeChanged(int i);
        void thresholdLowChanged(double d);
        void thresholdHighChanged(double d);
        void l2GradientChanged(bool b);

    public slots:
        void setApertureSize(int i);
        void setThresholdLow (double d);
        void setThresholdHigh(double d);
        void setL2Gradient(bool b);

    private:
        plv::CvMatDataInputPin*  m_inputPin;
        plv::CvMatDataOutputPin* m_outputPin;

        int m_apertureSize;
        double m_thresholdLow;
        double m_thresholdHigh;
        bool m_l2Gradient;
    };
}
#endif // EDGEDETECTORCANNY_H
