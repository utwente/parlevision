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

#ifndef DELAYIMAGE_H
#define DELAYIMAGE_H

#include <QMutex>
#include <QMutexLocker>
#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {

    class Pipeline;
    class OpenCVImage;

    class DelayImage : public PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Dennis");
        Q_CLASSINFO("name", "Delay image");
        Q_CLASSINFO("description", "Delay an image by given number of steps (max 10).");

        Q_PROPERTY( int steps READ getSteps WRITE setSteps NOTIFY stepsChanged )


    public:
        DelayImage();
        ~DelayImage();

        virtual void init() throw (PipelineException);
        /**override: every start, the buffer must be cleared */
        virtual void start() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        int getSteps() { return m_steps; }

        static const int MAX_STEPS = 10;

    signals:
        void stepsChanged(int newValue);

    public slots:
        void setSteps(int i);

    private:
        /** Clear contents of buffer; clear m_latestFrameIndex */
        void clearBuffer();

        InputPin<OpenCVImage>* m_inputPin;
        OutputPin<OpenCVImage>* m_outputPin;

        /** CIrcular buffer of delayed images.
          Latest image at [m_latestFrameIndex]; one frame older at [m_latestFrameIndex-1] etc. */
        RefPtr<OpenCVImage>* m_delayedImgs[MAX_STEPS+1];

        /** How many steps are images delayed? */
        int m_steps;
        /** Index of most recent frame in buffer */
        int m_latestFrameIndex;


    };

}

#endif // DELAYIMAGE_H
