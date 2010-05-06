#ifndef IMAGECONVERTER_H
#define IMAGECONVERTER_H

#include <QObject>
#include <opencv/cv.h>

#include "RefPtr.h"
#include "QtImage.h"

class QImage;

namespace plv {
    class OpenCVImage;
}

using namespace plv;

namespace plvgui {

    class ImageConverter : public QObject, public RefCounted
    {
        Q_OBJECT

    public:

        /** Starts converting this image to a QImage.
          * This call will return immediately
          * as converting is done asynchronously.
          * @emits converted(QImage*) when converting has finished;
          */
        void convert_OpenCVImage( plv::RefPtr<plv::OpenCVImage> img );

        static QImage* iplImageToQImage( const IplImage* iplImage, uchar** data );

    private:
        static ImageConverter* m_instance;
        void convert(plv::RefPtr<plv::OpenCVImage> img);

        static QImage* iplImageToQImageSafe( const IplImage* img );
        static QImage* iplImageToQImageFast( const IplImage* img );

    signals:
        /** Emitted when converting is done.
          * The contained image might not be valid if an error occurred
          */
        void converted( RefPtr<QtImage> img );
    };
}


#endif // IMAGECONVERTER_H
