#include <QDebug>

#include "ViolaJonesFaceDetector.h"
#include "Pin.h"
#include "OpenCVImage.h"


using namespace plv;

#define INPUT_PIN_NAME "input"
#define OUTPUT_PIN_NAME "output"

ViolaJonesFaceDetector::ViolaJonesFaceDetector() :
        m_minNeighbours(3),
        m_scaleFactor(1.1),
        m_useCannyPruning(true),
        m_minWidth(20),
        m_minHeight(20)
{


    m_inputPin = createInputPin<OpenCVImage>( INPUT_PIN_NAME, this, IInputPin::REQUIRED );
    m_outputPin = createOutputPin<OpenCVImage>( OUTPUT_PIN_NAME, this );
}

ViolaJonesFaceDetector::~ViolaJonesFaceDetector()
{
    if (m_pCascade != NULL)
            cvReleaseHaarClassifierCascade(&m_pCascade);

    if (m_pStorage != NULL)
            cvReleaseMemStorage(&m_pStorage);
}

void ViolaJonesFaceDetector::init() throw (PipelineException)
{
}

void ViolaJonesFaceDetector::start() throw (PipelineException)
{
    m_pStorage = cvCreateMemStorage(0);
    m_pCascade = (CvHaarClassifierCascade*)cvLoad("C://OpenCV2.1/data/haarcascades/haarcascade_frontalface_alt.xml",0,0,0);
}

bool ViolaJonesFaceDetector::isReadyForProcessing() const
{
    return m_inputPin->hasData() && m_pCascade!=0 && m_pStorage!=0;
}


void ViolaJonesFaceDetector::process()
{
    assert(m_inputPin != 0);
    assert(m_outputPin != 0);

    RefPtr<OpenCVImage> imgIn = m_inputPin->get();
    RefPtr<OpenCVImage> imgOut = OpenCVImageFactory::instance()->get(
            imgIn->getWidth(), imgIn->getHeight(), imgIn->getDepth(), imgIn->getNumChannels() );

    // open for reading
    const IplImage* iplImgIn = imgIn->getImage();

    // open image for writing
    IplImage* iplImgOut = imgOut->getImageForWriting();

    //detect faces
    m_pFaceRectSeq = cvHaarDetectObjects(iplImgIn, m_pCascade, m_pStorage,
            m_scaleFactor, /* increase scale by m_scaleFactor each pass */
            m_minNeighbours, /*drop groups fewer than m_minNeighbours detections */
            int (m_useCannyPruning), /* 1 means: CV_HAAR_DO_CANNY_PRUNING */
            cvSize(m_minWidth,m_minHeight) /* (0,0) means: use default smallest scale for detection */);

    //copy input image
    cvCopy( iplImgIn, iplImgOut );

    //draw face rects
    for (int i = 0; i <(m_pFaceRectSeq?m_pFaceRectSeq->total:0);i++)
    {
            CvRect* r = (CvRect*)cvGetSeqElem(m_pFaceRectSeq,i);
            CvPoint pt1 = {r->x,r->y};
            CvPoint pt2 = {r->x+r->width,r->y+r->height};
            cvRectangle(iplImgOut, pt1, pt2, CV_RGB(0,255,0),3,4,0);
    }

    // publish the new image
    m_outputPin->put( imgOut.getPtr() );

}
