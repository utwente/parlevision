#ifndef OpenCVImageRenderer_H
#define OpenCVImageRenderer_H

#include <QMutex>
#include <QLabel>

#include "OpenCVImage.h"
#include "RefPtr.h"
#include "ImageConverter.h"
#include "DataRenderer.h"


class QPixmap;
class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QImage;

namespace plv {
    class Data;
}

using namespace plv;

namespace plvgui
{
    class ImageWidget;

    class OpenCVImageRenderer : public DataRenderer
    {
        Q_OBJECT

    public:
        OpenCVImageRenderer(QWidget* parent);
        virtual ~OpenCVImageRenderer() {}
//        QSize sizeHint() const;
//        int heightForWidth(int w) const;
//        void resizeEvent(QResizeEvent * /*resizeEvent*/);

    protected:
        void showEvent(QShowEvent* event);
        void hideEvent(QHideEvent* event);

    private:
        void putImage();
        void fixAspectRatio();
        //QLabel*         m_imagelabel;

        QVBoxLayout*    m_layout;
        ImageWidget*    m_imageWidget;

        bool            m_busy;
        QMutex          m_busy_mutex;
        RefPtr<ImageConverter> m_converter;

    public slots:
        virtual void newData( RefPtr<Data> data );
        void updateImage( QImage img );
    };

//    class ImageLabel : public QLabel
//    {
//    public:
//        ImageLabel(QWidget* parent);
//        QSize sizeHint() const;
//        QSize minimumSizeHint() const;
//        int heightForWidth(int w) const;
//    };
}
#endif // OpenCVImageRenderer_H
