#ifndef OpenCVImageRenderer_H
#define OpenCVImageRenderer_H

#include <QMutex>

#include "OpenCVImage.h"
#include "QtImage.h"
#include "RefPtr.h"
#include "ImageConverter.h"
#include "DataRenderer.h"

class QLabel;
class QPixmap;
class QLabel;
class QVBoxLayout;
class QImage;

namespace plv {
    class Data;
}

using namespace plv;

namespace plvgui {

    class OpenCVImageRenderer : public DataRenderer
    {
        Q_OBJECT

    public:
        OpenCVImageRenderer(QWidget* parent);
        virtual ~OpenCVImageRenderer() {}

    private:
        void putImage();
        QLabel*         m_imagelabel;
        QVBoxLayout*    m_layout;
        bool            m_busy;
        QMutex          m_busy_mutex;
        RefPtr<ImageConverter> m_converter;

    public slots:
        virtual void newData( RefPtr<Data> data );
        void updateImage( RefPtr<QtImage> img );

    };    
}
#endif // OpenCVImageRenderer_H
