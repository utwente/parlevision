#ifndef IMAGEFLIP_H
#define IMAGEFLIP_H

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {
    class Pipeline;
    class OpenCVImage;

    class ImageFlip : public PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Ported from old version by Wim & Dennis")
        Q_CLASSINFO("name", "Flip")
        Q_CLASSINFO("description", "Flip image. FlipX means \"flip around x-axis\". Same for FlipY.");

        Q_PROPERTY( bool flipX READ getFlipX WRITE setFlipX NOTIFY flipXChanged  )
        Q_PROPERTY( bool flipY READ getFlipY WRITE setFlipY NOTIFY flipYChanged  )

    public:
        ImageFlip();
        ~ImageFlip();

        virtual void init() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        bool getFlipX() { return m_flipX; }
        bool getFlipY() { return m_flipY; }

    signals:
        void flipXChanged(bool newValue);
        void flipYChanged(bool newValue);

    public slots:
        void setFlipX(bool b);
        void setFlipY(bool b);

    private:

        InputPin<OpenCVImage>* m_inputPin;
        OutputPin<OpenCVImage>* m_outputPin;

        bool m_flipX;
        bool m_flipY;
        int m_method;
    };
}
#endif // IMAGEFLIP_H
