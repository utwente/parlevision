#ifndef OPENCVCAMERA_H
#define OPENCVCAMERA_H

#include <QString.h>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "OpenCVImage.h"
#include "RefCounted.h"
#include "RefPtr.h"

namespace plv
{
    class OpenCVCamera : public QThread, public RefCounted
    {
         Q_OBJECT

    public:

        /** Possible camera states */
        enum CameraState {
            CAM_UNINITIALIZED,
            CAM_INITIALIZED,
            CAM_RUNNING,
            CAM_PAUSED
        };

        /** Constructor default to OpenCV camera with id 0. */
        OpenCVCamera( int id = 0 );

        /** Initializes camera. Allocates resources. */
        bool init();

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

        inline int getWidth() const { return m_width; }
        inline int getHeight() const { return m_height; }

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

        /** camera dimensions */
        int    m_width;
        int    m_height;

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
        OpenCVImage* getFrame();

    signals:
        void newFrame( RefPtr<Data> frame );

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

