#ifndef OPENCVCAMERA_H
#define OPENCVCAMERA_H

#include <QString.h>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include <opencv/cv.h>
#include <opencv/highgui.h>


class OpenCVCamera : public QThread
{
     Q_OBJECT

public:

    /** Possible camera states */
    enum CameraState {
        CAM_UNITIALIZED,
        CAM_STARTING,
        CAM_RUNNING,
        CAM_PAUSED,
        CAM_STOPPING,
        CAM_STOPPED
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

    /**
      * Stop capturing and
      */
    virtual void stop();

    /** Start the thread and begin capturing
      * @pre getState() == CAM_STOPPED || CAM_PAUSED
      * @post getState() == CAM_STARTING. Eventually -> getState() == CAM_STARTED
      */
    virtual void start();

    /** Pause (currently implemented as busy loop)
      * @pre getState() == CAM_RUNNING
      * @post getState() == CAM_PAUSED
      */
    virtual void pause();

    inline int getWidth() const { return m_width; }
    inline int getHeight() const { return m_height; }

protected:
    /** The run loop.
      * Captures camera output and fires signals.
      */
    virtual void run();

    /** Tries to get a frame from the camera. Returns an IplImage pointer if
      * succesful. Returns 0 when image acquisition failed.
      */
    const IplImage* getFrame();

private:
    int         m_id;
    CameraState m_state;

    int	m_width;
    int	m_height;

    QMutex          m_mutex;
    QWaitCondition  m_condition;
    CvCapture*      m_captureDevice;

signals:
     void newFrame(const IplImage* frame);

};

#endif // OPENCVCAMERA_H

