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

#ifndef CAMERAPRODUCER_H
#define CAMERAPRODUCER_H

#include <QMutex>
#include <QWaitCondition>

#include "PipelineProducer.h"
#include "Pin.h"
#include "OpenCVImage.h"
#include "OpenCVCamera.h"

namespace plv {

    class CameraProducer : public PipelineProducer
    {
        Q_OBJECT

    public:
        CameraProducer();
        virtual ~CameraProducer();
        CameraProducer(const CameraProducer&);

        /** inherited by PipelineProducer */
        virtual void start() throw (PipelineException);
        virtual void stop() throw (PipelineException);

        /** inherited by PipelineElement */
        virtual void init() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        RefPtr<OpenCVCamera> getCamera() const { return m_camera; }

    protected:

        RefPtr<OpenCVCamera> m_camera;
        RefPtr<OpenCVImage> m_lastFrame;
        RefPtr< OutputPin<OpenCVImage> > m_outputPin;

        int m_lastProcessedId;

        QMutex m_frameMutex;
        QWaitCondition m_frameReady;

    public slots:
        void newFrame( RefPtr<Data> frame );
    };

}

#endif // CAMERAPRODUCER_H
