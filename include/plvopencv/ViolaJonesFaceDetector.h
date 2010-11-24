#ifndef VIOLAJONESFACEDETECTOR_H
#define VIOLAJONESFACEDETECTOR_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Types.h>
#include <opencv/cv.h>
#include <plvcore/Pin.h>

namespace plv
{
    class OpenCVImage;
    class OpenCVImageInputPin;
    class OpenCVImageOutputPin;
}

namespace plvopencv
{
    class ViolaJonesFaceDetector : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( ViolaJonesFaceDetector )
        Q_CLASSINFO("author", "Dennis")
        Q_CLASSINFO("name", "Face Detect V.-J.")
        Q_CLASSINFO("description", "Viola-Jones Face Detector.");

        Q_PROPERTY( int minNeighbours READ getMinNeighbours WRITE setMinNeighbours NOTIFY minNeighboursChanged  )
        Q_PROPERTY( double scaleFactor READ getScaleFactor WRITE setScaleFactor NOTIFY scaleFactorChanged  )
        Q_PROPERTY( bool useCannyPruning READ getUseCannyPruning WRITE setUseCannyPruning NOTIFY useCannyPruningChanged  )
        Q_PROPERTY( int minWidth READ getMinWidth WRITE setMinWidth NOTIFY minWidthChanged )
        Q_PROPERTY( int minHeight READ getMinHeight WRITE setMinHeight NOTIFY minHeightChanged )
        Q_PROPERTY( QString haarCascadeFile READ getHaarCascadeFile WRITE setHaarCascadeFile NOTIFY haarCascadeFileChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        ViolaJonesFaceDetector();
        virtual ~ViolaJonesFaceDetector();

        /** propery methods */
        int getMinNeighbours();
        double getScaleFactor();
        bool getUseCannyPruning();
        int getMinWidth();
        int getMinHeight();
        QString getHaarCascadeFile();

    signals:
        void minNeighboursChanged(int newValue);
        void scaleFactorChanged(double newValue);
        void useCannyPruningChanged(bool newValue);
        void minWidthChanged(int val);
        void minHeightChanged(int val);
        void haarCascadeFileChanged( QString filename );

    public slots:
        void setMinNeighbours(int i);
        void setScaleFactor(double d);
        void setUseCannyPruning(bool b);
        void setMinWidth(int val);
        void setMinHeight(int val);
        void setHaarCascadeFile(QString filename);

    private:
        plv::OpenCVImageInputPin* m_inputPin;
        plv::OutputPin<plv::RectangleData>* m_outputPinRectangles;
        plv::OpenCVImageOutputPin* m_outputPinMonitor;

        int m_minNeighbours;
        double m_scaleFactor;
        bool m_useCannyPruning;
        int m_minWidth;
        int m_minHeight;
        QString m_haarCascadeFile;

        CvHaarClassifierCascade* m_pCascade;
        CvMemStorage* m_pStorage;
    };
}

#endif // VIOLAJONESFACEDETECTOR_H
