#include <QDebug>
#include <QFile>

#include "ViolaJonesFaceDetector.h"
#include "OpenCVImage.h"

#include <plvcore/Pin.h>

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
    m_inputPin = createInputPin<OpenCVImage>( "input", this, IInputPin::INPUT_REQUIRED );
    m_outputPin = createOutputPin<OpenCVImage>( "output", this );
}

ViolaJonesFaceDetector::~ViolaJonesFaceDetector()
{
    deinit();
}

void ViolaJonesFaceDetector::init() throw (PipelineException)
{
    // should not be initialized yet
    assert( m_pStorage == 0 );
    assert( m_pCascade == 0 );


    // we expect this to always succeed
    m_pStorage = cvCreateMemStorage(0);
    if( m_pStorage == 0 )
    {
        throw PipelineException( "Failed to allocate temporary storage in OpenCV for "
                                 "Viola Jones Facedetector processor");
    }

    QFile file( m_haarCascadeFile );
    if( !file.exists() )
    {
        throw PipelineException( "Failed to load haar cascade file " +
                                 m_haarCascadeFile + ". File does not exist." );
    }
    if( !file.isReadable() )
    {
        throw PipelineException( "Failed to load haar cascade file " +
                                 m_haarCascadeFile + ". File is not readable." );
    }
    void* cascade = cvLoad( m_haarCascadeFile.toAscii(),0,0,0 );
    if( cascade == 0 )
    {

        throw PipelineException( "Failed to load haar cascade file "
                                 + m_haarCascadeFile );
    }
    m_pCascade = (CvHaarClassifierCascade*) cascade;
}

void ViolaJonesFaceDetector::deinit() throw()
{
    if (m_pCascade != 0)
        cvReleaseHaarClassifierCascade(&m_pCascade);

    if (m_pStorage != 0)
        cvReleaseMemStorage(&m_pStorage);
}

void ViolaJonesFaceDetector::start() throw (PipelineException)
{
}

void ViolaJonesFaceDetector::stop() throw (PipelineException)
{
}

void ViolaJonesFaceDetector::process()
{
    assert( m_inputPin != 0 );
    assert( m_outputPin != 0 );
    assert( m_pCascade != 0 );
    assert( m_pStorage != 0 );

    RefPtr<OpenCVImage> imgIn = m_inputPin->get();
    RefPtr<OpenCVImage> imgOut = OpenCVImageFactory::instance()->get(
            imgIn->getWidth(), imgIn->getHeight(), imgIn->getDepth(), imgIn->getNumChannels() );

    // open for reading
    const IplImage* iplImgIn = imgIn->getImage();

    // open image for writing
    IplImage* iplImgOut = imgOut->getImageForWriting();

    //detect faces
    CvSeq* faceRectSeq = cvHaarDetectObjects(iplImgIn, m_pCascade, m_pStorage,
            m_scaleFactor, /* increase scale by m_scaleFactor each pass */
            m_minNeighbours, /*drop groups fewer than m_minNeighbours detections */
            int (m_useCannyPruning), /* 1 means: CV_HAAR_DO_CANNY_PRUNING */
            cvSize(m_minWidth,m_minHeight) /* (0,0) means: use default smallest scale for detection */);

    //copy input image
    cvCopy( iplImgIn, iplImgOut );

    //draw face rects
    for (int i = 0; i < ( faceRectSeq ? faceRectSeq->total : 0 ); ++i )
    {
        CvRect* r = (CvRect*)cvGetSeqElem( faceRectSeq,i );
        CvPoint pt1 = {r->x,r->y};
        CvPoint pt2 = {r->x+r->width,r->y+r->height};
        cvRectangle( iplImgOut, pt1, pt2, CV_RGB(0,255,0),3,4,0 );
    }

    // publish the new image
    m_outputPin->put( imgOut.getPtr() );
}
