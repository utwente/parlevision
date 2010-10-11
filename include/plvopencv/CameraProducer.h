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

#ifndef CAMERAPRODUCER_H
#define CAMERAPRODUCER_H

#include <QMutex>
#include <QWaitCondition>

#include <plvcore/PipelineProducer.h>
#include <plvcore/Pin.h>

#include "OpenCVCamera.h"

namespace plvopencv
{
    class CameraProducer : public plv::PipelineProducer
    {
        Q_OBJECT

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        CameraProducer();
        virtual ~CameraProducer();
        CameraProducer(const CameraProducer&);

        /** @returns true if a new frame is available */
        bool isReadyForProcessing() const;

        inline OpenCVCamera* getCamera() const { return m_camera.getPtr(); }

    protected:
        plv::RefPtr<OpenCVCamera> m_camera;
        plv::RefPtr<plv::OpenCVImage> m_lastFrame;
        plv::OutputPin<plv::OpenCVImage>* m_outputPin;

        int m_lastProcessedId;

        QMutex m_frameMutex;
        QWaitCondition m_frameReady;

    public slots:
        void newFrame( plv::RefPtr<plv::Data> frame );
    };

}

#endif // CAMERAPRODUCER_H
