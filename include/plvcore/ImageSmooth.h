#ifndef IMAGESMOOTH_H
#define IMAGESMOOTH_H

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {
    class Pipeline;
    class OpenCVImage;

    class ImageSmooth : public PipelineProcessor
    {
        Q_OBJECT
        
        Q_CLASSINFO("author", "Dennis & Wim")
        Q_CLASSINFO("name", "Smooth")
        Q_CLASSINFO("description", "Smoothing using cvSmooth."
                    "See OpenCV reference for meaning of parameters."
                    "<a href='http://opencv.willowgarage.com/documentation/image_filtering.html?highlight=cvsmooth#cvSmooth'>"
                    "http://opencv.willowgarage.com/documentation/image_filtering.html?highlight=cvsmooth#cvSmooth"
                    "</a>")

        Q_PROPERTY( plv::Enum method READ getMethod WRITE setMethod NOTIFY methodChanged )

        Q_PROPERTY( int one READ getOne WRITE setOne NOTIFY oneChanged )
        Q_PROPERTY( int two READ getTwo WRITE setTwo NOTIFY twoChanged )
        Q_PROPERTY( double three READ getThree WRITE setThree NOTIFY threeChanged )
        Q_PROPERTY( double four READ getFour WRITE setFour NOTIFY fourChanged )

    public:
        ImageSmooth();
        ~ImageSmooth();

        virtual void init() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        plv::Enum getMethod() const { return m_method; }
        int getOne() { return m_one; }
        int getTwo() { return m_two; }
        double getThree() { return m_three; }
        double getFour() { return m_four; }

    signals:
        void oneChanged(int newValue);
        void twoChanged(int newValue);
        void threeChanged(double newValue);
        void fourChanged(double newValue);
        void methodChanged( plv::Enum m );

    public slots:
        void setOne(int i);
        void setTwo(int i);
        void setThree(double i) { m_three = i; emit(threeChanged(i)); }
        void setFour(double i) { m_four = i; emit(fourChanged(i)); }
        void setMethod( plv::Enum m ) { m_method = m; emit(methodChanged(m_method));}

    private:

        InputPin<OpenCVImage>* m_inputPin;
        OutputPin<OpenCVImage>* m_outputPin;

        plv::Enum m_method;
        int m_one;
        int m_two;
        double m_three;
        double m_four;
    };
}
#endif // IMAGEFLIP_H
