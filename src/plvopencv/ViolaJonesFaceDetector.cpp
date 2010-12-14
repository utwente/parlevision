#include <QDebug>
#include <QFile>

#include "ViolaJonesFaceDetector.h"
#include <plvcore/CvMatData.h>

#include <plvcore/CvMatDataPin.h>

using namespace plv;
using namespace plvopencv;

ViolaJonesFaceDetector::ViolaJonesFaceDetector() :
        m_minNeighbours(3),
        m_scaleFactor(1.1),
        m_useCannyPruning(true),
        m_minWidth(20),
        m_minHeight(20),
        //TODO this should be stored somewhere else maybe?
        m_haarCascadeFile( "C://OpenCV2.0/data/haarcascades/haarcascade_frontalface_alt.xml" ),
        m_pCascade( 0 ),
        m_pStorage( 0 )
{
    m_inputPin = createCvMatDataInputPin( "input", this );
    m_inputPin->addAllChannels();
    m_inputPin->addAllDepths();

    m_outputPinRectangles = createOutputPin<RectangleData>( "face rectangles", this );

    m_outputPinMonitor = createCvMatDataOutputPin( "monitor", this );
    m_outputPinMonitor->addAllChannels();
    m_outputPinMonitor->addAllDepths();
}

ViolaJonesFaceDetector::~ViolaJonesFaceDetector()
{
    deinit();
}

void ViolaJonesFaceDetector::init()
{
    // should not be initialized yet
    assert( m_pStorage == 0 );
    assert( m_pCascade == 0 );

    // we expect this to always succeed
    m_pStorage = cvCreateMemStorage(0);
    if( m_pStorage == 0 )
    {
        throw PlvInitialisationException( "Failed to allocate temporary storage in OpenCV for "
                                          "Viola Jones Facedetector processor");
    }

    QFile file( m_haarCascadeFile );
    if( !file.exists() )
    {
        throw PlvInitialisationException( "Failed to load haar cascade file " +
                                           m_haarCascadeFile + ". File does not exist." );
    }

    void* cascade = cvLoad( m_haarCascadeFile.toAscii(),0,0,0 );
    if( cascade == 0 )
    {

        throw PlvInitialisationException( "Failed to load haar cascade file "
                                            + m_haarCascadeFile );
    }
    m_pCascade = (CvHaarClassifierCascade*) cascade;
}

void ViolaJonesFaceDetector::deinit() throw()
{
    if (m_pCascade != 0)
    {
        cvReleaseHaarClassifierCascade(&m_pCascade);
        m_pCascade = 0;
    }

    if (m_pStorage != 0)
    {
        cvReleaseMemStorage(&m_pStorage);
        m_pStorage = 0;
    }
}

void ViolaJonesFaceDetector::start()
{
}

void ViolaJonesFaceDetector::stop()
{
}

void ViolaJonesFaceDetector::process()
{
    assert( m_pCascade != 0 );
    assert( m_pStorage != 0 );

    CvMatData srcPtr = m_inputPin->get();
    CvMatData dstPtr = CvMatData::create( srcPtr.properties() );

    // open for reading
    const cv::Mat& src = srcPtr;


    // open image for writing
    cv::Mat& dst = dstPtr;

    // detect faces
    // this still uses old C interface
    CvMat srcMat = src;
    CvSeq* faceRectSeq =
            cvHaarDetectObjects(&srcMat, m_pCascade, m_pStorage,
            m_scaleFactor, /* increase scale by m_scaleFactor each pass */
            m_minNeighbours, /*drop groups fewer than m_minNeighbours detections */
            int (m_useCannyPruning), /* 1 means: CV_HAAR_DO_CANNY_PRUNING */
            cv::Size(m_minWidth,m_minHeight) /* (0,0) means: use default smallest scale for detection */);

    //copy input image
    src.copyTo(dst);
    RectangleData rectangles( srcPtr.width(), srcPtr.height() );

    //draw face rects
    for (int i = 0; i < ( faceRectSeq ? faceRectSeq->total : 0 ); ++i )
    {
        CvRect* r = (CvRect*)cvGetSeqElem( faceRectSeq,i );
        CvPoint pt1 = {r->x,r->y};
        CvPoint pt2 = {r->x+r->width,r->y+r->height};
        cv::rectangle( dst, pt1, pt2, CV_RGB(0,255,0),3,4,0 );
        rectangles.add( QRect( r->x, r->y, r->width, r->height ) );
    }

    // publish the new image and rectangle data
    m_outputPinRectangles->put( rectangles );
    m_outputPinMonitor->put( dstPtr );
}

void ViolaJonesFaceDetector::setMinNeighbours(int i)
{
    QMutexLocker lock( m_propertyMutex );
    if (i>0) m_minNeighbours = i;
    emit(minNeighboursChanged(m_minNeighbours));
}

int ViolaJonesFaceDetector::getMinNeighbours()
{
    QMutexLocker lock( m_propertyMutex); return m_minNeighbours;
}

void ViolaJonesFaceDetector::setScaleFactor(double d)
{
    QMutexLocker lock( m_propertyMutex );
    if (d>1) m_scaleFactor = d;
    emit(scaleFactorChanged(m_scaleFactor));
}

double ViolaJonesFaceDetector::getScaleFactor()
{
    QMutexLocker lock( m_propertyMutex); return m_scaleFactor;
}

void ViolaJonesFaceDetector::setUseCannyPruning(bool b)
{
    QMutexLocker lock( m_propertyMutex );
    m_useCannyPruning = b;
    emit(useCannyPruningChanged(m_useCannyPruning));
}

bool ViolaJonesFaceDetector::getUseCannyPruning()
{
    QMutexLocker lock( m_propertyMutex);
    return m_useCannyPruning;
}

void ViolaJonesFaceDetector::setMinWidth(int val)
{
    QMutexLocker lock( m_propertyMutex );
    if(val>=0) m_minWidth = val;
    emit( minWidthChanged(m_minWidth) );
}

int ViolaJonesFaceDetector::getMinWidth()
{
    QMutexLocker lock( m_propertyMutex);
    return m_minWidth;
}

void ViolaJonesFaceDetector::setMinHeight(int val)
{
    QMutexLocker lock( m_propertyMutex );
    if(val>=0) m_minHeight = val;
    emit( minHeightChanged(m_minHeight) );
}

int ViolaJonesFaceDetector::getMinHeight()
{
    QMutexLocker lock( m_propertyMutex );
    return m_minHeight;
}

void ViolaJonesFaceDetector::setHaarCascadeFile(QString filename)
{
    QMutexLocker lock( m_propertyMutex );
    m_haarCascadeFile = filename;
    emit( haarCascadeFileChanged(filename));
}

QString ViolaJonesFaceDetector::getHaarCascadeFile()
{
    QMutexLocker lock( m_propertyMutex);
    return m_haarCascadeFile;
}


