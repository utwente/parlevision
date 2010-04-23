#ifndef OPENCVIMAGEINSPECTOR_H
#define OPENCVIMAGEINSPECTOR_H

#include "Inspector.h"

class QLabel;
class QPixmap;
class QLabel;
class QVBoxLayout;
class QImage;

namespace plv {
    class Data;
    class OpenCVImage;
}

namespace plvgui {
    class OpenCVImageInspector : public Inspector
    {
        Q_OBJECT

    public:
        OpenCVImageInspector(QWidget* parent);
        virtual ~OpenCVImageInspector() {}

    private:
        void putImage(plv::OpenCVImage* img);
        QLabel*         m_imagelabel;
        QVBoxLayout*    m_layout;
        QImage          m_image;

    public slots:
        virtual void newData(Data* data);

    };    
}
#endif // OPENCVIMAGEINSPECTOR_H
