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

#ifndef KINECTTHRESHOLD_H
#define KINECTTHRESHOLD_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Enum.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvmskinect
{

/**
 * With this proccessor the received image is manipulated by thresholding
 * all the values in an image array above or below a threshold value. The
 * process either creates a binary image, truncates values to the threshold
 * or truncates to zero. It is required that the input image is in grayscale.
 */
class KinectThreshold : public plv::PipelineProcessor
{
    Q_OBJECT
    Q_DISABLE_COPY( KinectThreshold )
    Q_CLASSINFO("author", "Richard Loos")
    Q_CLASSINFO("name", "Kinect Threshold")
    Q_CLASSINFO("description", "")

    //Q_PROPERTY( plv::Enum method READ getMethod WRITE setMethod NOTIFY methodChanged )
    Q_PROPERTY( int threshold READ getThreshold WRITE setThreshold NOTIFY thresholdChanged )
    Q_PROPERTY( int maxValue READ getMaxValue WRITE setMaxValue NOTIFY maxValueChanged )

    /** required standard method declaration for plv::PipelineProcessor */
    PLV_PIPELINE_PROCESSOR

public:
    KinectThreshold();
    virtual ~KinectThreshold();

    /** propery methods */
    plv::Enum getMethod() const;
    int getThreshold() const;
    int getMaxValue() const;

signals:
    void methodChanged(plv::Enum m);
    void thresholdChanged(int newValue);
    void maxValueChanged(int newValue);

public slots:
    void setMethod(plv::Enum m);
    void setThreshold(int d);
    void setMaxValue(int d);

private:
    plv::CvMatDataInputPin*  m_inputPin;
    plv::CvMatDataOutputPin* m_outputPin;

    plv::Enum m_method;
    int m_threshold;
    int m_maxValue;

};

}

#endif // KINECTTHRESHOLD_H
