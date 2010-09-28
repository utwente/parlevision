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
#include <plvcore/Pin.h>

#include <QMutex>
#include <QMutexLocker>
#include <QList>

namespace plvopencv
{
    class OpenCVImage;

    class DelayImage : public plv::PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Dennis");
        Q_CLASSINFO("name", "Delay image");
        Q_CLASSINFO("description", "Delay an image by given number of steps (max 10).");

        Q_PROPERTY( int steps READ getSteps WRITE setSteps NOTIFY stepsChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        static const int MAX_STEPS = 1000;

        DelayImage();
        ~DelayImage();

        /** propery methods */
        int getSteps() { return m_steps; }

    signals:
        void stepsChanged(int newValue);

    public slots:
        void setSteps(int i);

    private:
        plv::InputPin<OpenCVImage>*  m_inputPin;
        plv::OutputPin<OpenCVImage>* m_outputPin;
        plv::OutputPin<OpenCVImage>* m_delayedOutputPin;

        /** List of delayed images. */
        QList< plv::RefPtr<OpenCVImage> > m_images;

        /** Number of steps images are delayed */
        int m_steps;
    };

}

#endif // DELAYIMAGE_H
