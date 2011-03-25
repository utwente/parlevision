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

#ifndef DILATEERODE_H
#define DILATEERODE_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Enum.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    class DilateErode : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( DilateErode )
        Q_CLASSINFO("author", "Richard Loos");
        Q_CLASSINFO("name", "Dilate and/or Erode an image");
        Q_CLASSINFO("description", "Dilates and/or erodes an image");

        Q_PROPERTY( int erosionIterations READ getErosionIterations WRITE setErosionIterations NOTIFY erosionIterationsChanged )
        Q_PROPERTY( int dilationIterations READ getDilationIterations WRITE setDilationIterations NOTIFY dilationIterationsChanged )
        Q_PROPERTY( plv::Enum order READ getOrder WRITE setOrder NOTIFY orderChanged )

        /** required standard method declaration for plv::PipelineProcessor */
        PLV_PIPELINE_PROCESSOR

    public:
        static const int MAX_STEPS = 100;

        DilateErode();
        virtual ~DilateErode();

        int getErosionIterations() const;
        int getDilationIterations() const;
        plv::Enum getOrder() const;

    signals:
        void erosionIterationsChanged(int e);
        void dilationIterationsChanged(int d);
        void orderChanged(plv::Enum);

    public slots:
        void setErosionIterations(int e);
        void setDilationIterations(int d);
        void setOrder(plv::Enum order);

    private:
        plv::CvMatDataInputPin*  m_inputPin;
        plv::CvMatDataOutputPin* m_outputPin;

        int m_erosionIterations;
        int m_dilationIterations;
        plv::Enum m_delationErosionOrder;
    };

}

#endif // DILATEERODE_H
