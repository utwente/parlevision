#ifndef OPENCVCAMERA_H
#define OPENCVCAMERA_H

#include <QString.h>
#include <QThread>

#include <opencv/cv.h>
#include <opencv/highgui.h>


class OpenCVCamera : public QThread
{
     Q_OBJECT

public:

    /** Possible camera states */
    enum CameraState {
        CAM_UNITIALIZED,
        CAM_STOPPED,
        CAM_RUNNING,
        CAM_PAUSED
    };

    OpenCVCamera( int id = 0 );
    ~OpenCVCamera();

    /** Start thread execution.
      *
      */
    virtual void run();

    /** Initalizes the camera. Returns 0 on succes, a negative error code
      * on failure.
      */
    int init();

    /** Returns the camera ID. This is a number equal to or above 0. The
      * default is 0.
      */
    int getId();

    /** Returns the vendor string as returned by the camera driver */
    //const QString& getName();

    /** Returns true if camera is initialized, false if not */
    bool isInitialized();

    /** Returns the state of the camera. See cameraState enum
     *  for possible camera states.
     */
    CameraState getState();

    /** stop, start and pause function not in use atm*/
//    void stop();
//    void start();
//    void pause();

    /** Tries to get a frame from the camera. Returns an IplImage pointer if
      * succesful. Returns 0 when image acquisition failed.
      */
    const IplImage* getFrame();

    inline int getWidth() const { return m_width; }
    inline int getHeight() const { return m_height; }

private:
    int	m_id;
    CameraState m_state;
    int	m_width;
    int	m_height;

    CvCapture* m_captureDevice;

signals:
     void newFrame(const IplImage* frame);

};

#endif // OPENCVCAMERA_H

