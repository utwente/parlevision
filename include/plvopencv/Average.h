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

#ifndef AVERAGE_H
#define AVERAGE_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/CvMatData.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    /**
      * Average of two images.
      */
    class Average : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( Average )
        Q_CLASSINFO("author", "Richard Loos")
        Q_CLASSINFO("name", "Average")
        Q_CLASSINFO("description", "Calculates a running average using weight.");
        Q_PROPERTY( double weight READ getWeight WRITE setWeight NOTIFY weightChanged )

        /** required standard method declaration for plv::PipelineProcessor */
        PLV_PIPELINE_PROCESSOR

    public:
        Average();
        virtual ~Average();

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
    };
}
#endif // AVERAGE_H
