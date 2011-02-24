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

namespace plv
{
    class CvMatDataOutputPin;
}

#include "OpenCVCamera.h"

namespace plvopencv
{
    class CameraProducer : public plv::PipelineProducer
    {
        Q_OBJECT
        Q_DISABLE_COPY( CameraProducer )

        Q_CLASSINFO("author", "Richard Loos")
        Q_CLASSINFO("name", "Camera Producer (OpenCV)")
        Q_CLASSINFO("description", "This producer produces images from a camera using OpenCV's built in camera support.")
        Q_PROPERTY( int cameraId READ getCameraId WRITE setCameraId NOTIFY cameraIdChanged )
        Q_PROPERTY( int width READ getWidth WRITE setWidth NOTIFY widthChanged )
        Q_PROPERTY( int height READ getHeight WRITE setHeight NOTIFY heightChanged )

        /** required standard method declaration for plv::PipelineProducer */
        PLV_PIPELINE_PRODUCER

    public:
        CameraProducer();
        virtual ~CameraProducer();

        virtual bool init();
        virtual bool deinit() throw();
        virtual bool start();
        virtual bool stop();

        inline OpenCVCamera* getCamera() const { return m_camera; }

        inline int getCameraId() const { return m_cameraId; }
        inline int getHeight() const { return m_height; }
        inline int getWidth() const { return m_width; }

    protected:
        OpenCVCamera* m_camera;
        QList<plv::CvMatData> m_frames;
        plv::CvMatDataOutputPin* m_outputPin;

        int m_cameraId;
        int m_width;
        int m_height;
        int m_lastProcessedId;
        int m_maxBufferSize;

        mutable QMutex m_frameMutex;
        QWaitCondition m_frameReady;

    public slots:
        void newFrame( plv::CvMatData frame );
        void cameraFinished();

        void setCameraId(int c) { m_cameraId = c; emit(cameraIdChanged(c));}
        void setHeight(int h) { m_height = h; emit(heightChanged(h));}
        void setWidth(int w) { m_width = w; emit(widthChanged(w));}

    signals:
        void cameraIdChanged(int c);
        void heightChanged(int h);
        void widthChanged(int w);
    };

}

#endif // CAMERAPRODUCER_H
