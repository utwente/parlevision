#ifndef OPENCVCAMERA_H
#define OPENCVCAMERA_H

#include <QString.h>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "OpenCVImage.h"

namespace plv {

class OpenCVCamera : public QThread
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

    OpenCVCamera( int id = 0 );
    ~OpenCVCamera();

    /** Initalizes the camera. Returns 0 on succes, a negative error code
      * on failure.
      */
    int init();

    /** Returns the camera ID. This is a number equal to or above 0. The
      * default is 0.
      */
    int getId() const;

    /** Returns the vendor string as returned by the camera driver */
    //const QString& getName();

    /** Returns true if camera is initialized, false if not */
    bool isInitialized() const;

    /** Returns the state of the camera. See cameraState enum
     *  for possible camera states.
     */
    CameraState getState() const;

    inline int getWidth() const { return m_width; }
    inline int getHeight() const { return m_height; }

protected:
    /** The run loop.
      * Captures camera output and fires signals.
      */
    virtual void run();

    /** Tries to get an image from OpenCV
      * and copy it into an OpenCVImage
      */
    OpenCVImage* getFrame();

private:
    int         m_id;
    CameraState m_state;

    int	m_width;
    int	m_height;

    QMutex          m_mutex;
    QMutex          m_opencv_mutex;
    QWaitCondition  m_condition;
    CvCapture*      m_captureDevice;

    /**
     * Releases any capture devices that might be used.
     * Does nothing if nothing in use.
     */
    void releaseCapture();

    /**
     * Fetches an image from OpenCV.
     * This image might become invalid after it is released by OpenCV
     * so don't keep references to it for long.
     */
    const IplImage* retrieveFrame();

signals:
    void newFrame(plv::OpenCVImage* frame);

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

