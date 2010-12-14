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

#ifndef DELAYIMAGE_H
#define DELAYIMAGE_H

#include <plvcore/PipelineProcessor.h>

#include <QList>
#include <plvcore/CvMatData.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    class DelayImage : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( DelayImage )
        Q_CLASSINFO("author", "Dennis, Richard");
        Q_CLASSINFO("name", "Delay image");
        Q_CLASSINFO("description", "Delays an image by a given number of steps (max 100).");

        Q_PROPERTY( int steps READ getSteps WRITE setSteps NOTIFY stepsChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        static const int MAX_STEPS = 100;

        DelayImage();
        virtual ~DelayImage();

        /** propery methods */
        int getSteps() { return m_steps; }

    signals:
        void stepsChanged(int newValue);

    public slots:
        void setSteps(int i);

    private:
        plv::CvMatDataInputPin*  m_inputPin;
        plv::CvMatDataOutputPin* m_outputPin;
        plv::CvMatDataOutputPin* m_delayedOutputPin;

        /** List of delayed images. */
        QList< plv::CvMatData > m_images;

        /** Number of steps images are delayed */
        int m_steps;
    };

}

#endif // DELAYIMAGE_H
