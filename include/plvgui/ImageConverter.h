#ifndef IMAGECONVERTER_H
#define IMAGECONVERTER_H

#include <QObject>
#include <opencv/cv.h>

#include "RefPtr.h"
#include "QtImage.h"

#include <stdexcept>

class QImage;

namespace plv {
    class OpenCVImage;
}

using namespace plv;

namespace plvgui
{
    class ImageConversionException : public std::runtime_error
    {
    public:
        ImageConversionException( const std::string& why ) :
                std::runtime_error( why ) {}
    };

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

    private:
        static ImageConverter* m_instance;
        void convert(plv::RefPtr<plv::OpenCVImage> img);

        /** Converts an OpenCV iplImage to a QImage.
          * @throw ImageConversionException when conversion fails.
          */
        static QImage* iplImageToQImage( const IplImage* img )
                throw( ImageConversionException );

    signals:
        /** Emitted when converting is done.
          * The contained image might not be valid if an error occurred
          */
        void converted( RefPtr<QtImage> img );
    };
}


#endif // IMAGECONVERTER_H
