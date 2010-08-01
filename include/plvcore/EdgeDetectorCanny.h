#ifndef EDGEDETECTORCANNY_H
#define EDGEDETECTORCANNY_H

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {
    class Pipeline;
    class OpenCVImage;

    class EdgeDetectorCanny : public PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Ported from old version by Wim & Dennis")
        Q_CLASSINFO("name", "Edge detector Canny")
        Q_CLASSINFO("description", "Edge detection using the Canny method.");

        Q_PROPERTY( int apertureSize READ getApertureSize WRITE setApertureSize NOTIFY apertureSizeChanged )
        Q_PROPERTY( double thresholdLow READ getThresholdLow WRITE setThresholdLow NOTIFY thresholdLowChanged )
        Q_PROPERTY( double thresholdHigh READ getThresholdHigh WRITE setThresholdHigh NOTIFY thresholdHighChanged )

    public:
        EdgeDetectorCanny();
        ~EdgeDetectorCanny();

        virtual void init() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        int getApertureSize() { return m_apertureSize; }
        double getThresholdLow() { return m_thresholdLow; }
        double getThresholdHigh() { return m_thresholdHigh; }

    signals:
        void apertureSizeChanged(int newValue);
        void thresholdLowChanged(double newValue);
        void thresholdHighChanged(double newValue);

    public slots:
        void setApertureSize(int i);
        void setThresholdLow (double newValue) { m_thresholdLow  = newValue; emit(thresholdLowChanged(newValue)); }
        void setThresholdHigh(double newValue) { m_thresholdHigh = newValue; emit(thresholdHighChanged(newValue)); }

    private:
        int nearestOdd(int i);

        InputPin<OpenCVImage>* m_inputPin;
        OutputPin<OpenCVImage>* m_outputPin;

        int m_apertureSize;
        double m_thresholdLow;
        double m_thresholdHigh;
    };
}
#endif // EDGEDETECTORCANNY_H
