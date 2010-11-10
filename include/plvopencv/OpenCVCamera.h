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

#ifndef OPENCVCAMERA_H
#define OPENCVCAMERA_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <plvcore/RefCounted.h>
#include <plvcore/RefPtr.h>
#include <plvcore/OpenCVImage.h>

namespace plvopencv
{
    class OpenCVCamera : public QThread, public plv::RefCounted
    {
        Q_OBJECT
        Q_DISABLE_COPY( OpenCVCamera )

    public:

        /** Possible camera states */
        enum CameraState {
            CAM_UNINITIALIZED,
            CAM_INITIALIZED,
            CAM_RUNNING,
            CAM_PAUSED
        };

        /** Constructor default to OpenCV camera with id 0. */
        OpenCVCamera();

        /** Initializes camera. Allocates resources. */
        bool init( int id );

        /** Returns the camera ID. This is a number equal to or above 0. The
          * default is 0.
          */
        int getId() const;

        /** Returns the vendor string as returned by the camera driver */
        //const QString& getName();

        /** Returns true if camera is initialized, false if not */
        //bool isInitialized() const;

        /** Returns the state of the camera. See cameraState enum
         *  for possible camera states.
         */
        CameraState getState() const;

        /** Sets the camera dimensions. Only 4:3 resolutions are supported
          * by the OpenCV highgui library.
          * @return true on succesful resolution switch, or if resolution is
          * already as requested. False when resolution switch failed or resolution
          * is invalid.
          */
        bool setDimensions( int width, int height );

        int width() const;
        int height() const;

        /** Returns the number of available cameras.
          * Note that this needs to be run while no cameras are in use
          * and while all cameras are connected
          */
//        static const QHash<int,QString>* getAvailableCameras();

    protected:
        /** The run loop.
          * Captures camera output and fires signals.
          */
        virtual void run();
        virtual ~OpenCVCamera();

    private:
        int         m_id;
        CameraState m_state;

        QMutex          m_mutex;
        QMutex          m_opencv_mutex;
        QWaitCondition  m_condition;
        CvCapture*      m_captureDevice;

        /**
         * Releases any capture devices that might be used.
         * Does nothing if nothing in use.
         */
        void releaseCapture();

        /** Tries to get an image from OpenCV and copy it into an OpenCVImage.
          * \return 0 on failure, pointer to an OpenCVImage on success.
          */
        plv::OpenCVImage* getFrame();

    signals:
        void newFrame( plv::RefPtr<plv::Data> frame );

    public slots:

         /** Start the thread and begin capturing
           * @pre getState() == CAM_STOPPED || CAM_PAUSED
           * @post getState() == CAM_STARTING. Eventually -> getState() == CAM_STARTED
           */
         virtual void start();

         /** Pause
           * @pre getState() == CAM_RUNNING
           * @post getState() == CAM_PAUSED
           */
         virtual void pause();

         /**
           * Release the camera and deinitialize
           */
         virtual void release();

    };
}

#endif // OPENCVCAMERA_H

