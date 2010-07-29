#ifndef IMAGECORNERHARRIS_H
#define IMAGECORNERHARRIS_H

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {
    class Pipeline;
    class OpenCVImage;

    class ImageCornerHarris : public PipelineProcessor
    {
        Q_OBJECT
        Q_PROPERTY( int apertureSize READ getApertureSize WRITE setApertureSize NOTIFY apertureSizeChanged )
        Q_PROPERTY( int m_blockSize READ getBlockSize WRITE setBlockSize NOTIFY blockSizeChanged )
        Q_PROPERTY( double k READ getK WRITE setK NOTIFY kChanged )

    public:
        ImageCornerHarris();
        ~ImageCornerHarris();

        virtual void init() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        int getApertureSize() { return m_apertureSize; }
        int getBlockSize() { return m_blockSize; }
        double getK() { return m_k; }

    signals:
        void apertureSizeChanged(int newValue);
        void blockSizeChanged(int newValue);
        void kChanged(double newValue);

    public slots:
        void setApertureSize(int i) { m_apertureSize = i; emit(apertureSizeChanged(i)); }
        void setBlockSize(int i) { m_blockSize = i; emit(blockSizeChanged(i)); }
        void setK (double newValue) { m_k = newValue; emit(kChanged(newValue)); }

    private:
        int nearestOdd(int i);

        InputPin<OpenCVImage>* m_inputPin;
        OutputPin<OpenCVImage>* m_outputPin;

        int m_apertureSize;
        int m_blockSize;
        double m_k;
    };
}
#endif // IMAGECORNERHARRIS_H
