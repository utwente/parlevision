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

#ifndef RUNNINGAVERAGE_H
#define RUNNINGAVERAGE_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/CvMatData.h>
#include <plvcore/Pin.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    /**
      * RunningAverage of two images.
      */
    class RunningAverage : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( RunningAverage )
        Q_CLASSINFO("author", "Richard Loos")
        Q_CLASSINFO("name", "RunningAverage")
        Q_CLASSINFO("description", "Calculates a running average using the formula "
                    " A(x,y) = (1-weight)*A(x,y) + weight*C(x,y)"
                    " where the running average image A and the current image C:");

        Q_PROPERTY( double weight READ getWeight WRITE setWeight NOTIFY weightChanged )

        /** required standard method declaration for plv::PipelineProcessor */
        PLV_PIPELINE_PROCESSOR

    public:
        RunningAverage();
        virtual ~RunningAverage();

        double getWeight() const;

    public slots:
        void setWeight(double w);

    signals:
        void weightChanged(double w);

    private:
        plv::CvMatDataInputPin* m_inputPin;
        plv::CvMatDataOutputPin* m_outputPin;

        plv::CvMatData m_avg;
        plv::CvMatData m_tmp;
        plv::CvMatData m_out;

        double m_weight;
        double m_conversionFactor;
    };
}
#endif // AVERAGE_H
