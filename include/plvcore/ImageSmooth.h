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
        int getOne() { return m_one; }
        int getTwo() { return m_two; }
        double getThree() { return m_three; }
        double getFour() { return m_four; }

    signals:
        void oneChanged(int newValue);
        void twoChanged(int newValue);
        void threeChanged(double newValue);
        void fourChanged(double newValue);

    public slots:
        void setOne(int i) { m_one = i; emit(oneChanged(i)); }
        void setTwo(int i) { m_two = i; emit(twoChanged(i)); }
        void setThree(double i) { m_three = i; emit(threeChanged(i)); }
        void setFour(double i) { m_four = i; emit(fourChanged(i)); }

    private:
        int nearestOdd(int i);

        InputPin<OpenCVImage>* m_inputPin;
        OutputPin<OpenCVImage>* m_outputPin;

        int m_one;
        int m_two;
        double m_three;
        double m_four;
    };
}
#endif // IMAGEFLIP_H
