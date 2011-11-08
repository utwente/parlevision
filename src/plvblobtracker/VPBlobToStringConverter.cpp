#include "VPBlobToStringConverter.h"

using namespace plv;
using namespace plvblobtracker;

#include <opencv/cv.h>

VPBlobToStringConverter::VPBlobToStringConverter()
{
    m_inputBlobs = createInputPin< QList<Blob> >( "input", this );
    m_inputImage = createCvMatDataInputPin( "image", this );

    m_inputImage->addAllChannels();
    m_inputImage->addAllDepths();

    m_outputPin = createOutputPin<QString>( "output", this );
}

VPBlobToStringConverter::~VPBlobToStringConverter()
{
}

bool VPBlobToStringConverter::process()
{
    CvMatData image = m_inputImage->get();
    QList<plvblobtracker::Blob> blobs = m_inputBlobs->get();

    QString out = QString("FRAME:%1#").arg( this->getProcessingSerial() );

    // convert coordinates to what the virtual playground expects
    foreach( Blob b , blobs )
    {
        cv::Point p = b.getCenterOfGravity();
        //p.x = image.width() - p.x;
        p.y = image.height() - p.y;
        QString blobString = QString("BEGIN_MARKER#MARKER_ID:%1#MARKER_CENTER_X:%2#MARKER_CENTER_Y:%3#").arg(0).arg(p.x).arg(p.y);
        out.append(blobString);
    }

    // end TCP frame with newline
    out.append("\n");

    m_outputPin->put(out);
    return true;
}
