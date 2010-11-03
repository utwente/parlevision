#ifndef VIOLAJONESFACEDETECTOR_H
#define VIOLAJONESFACEDETECTOR_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Types.h>
#include <opencv/cv.h>

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
        int getMinNeighbours() { return m_minNeighbours; }
        double getScaleFactor() { return m_scaleFactor; }
        bool getUseCannyPruning() { return m_useCannyPruning; }
        int getMinWidth() { return m_minWidth; }
        int getMinHeight() { return m_minHeight; }
        QString getHaarCascadeFile() { return m_haarCascadeFile; }

    signals:
        void minNeighboursChanged(int newValue);
        void scaleFactorChanged(double newValue);
        void useCannyPruningChanged(bool newValue);
        void minWidthChanged(int val);
        void minHeightChanged(int val);
        void haarCascadeFileChanged( QString filename );

    public slots:
        void setMinNeighbours(int i) {if (i>0) m_minNeighbours = i; emit(minNeighboursChanged(m_minNeighbours));}
        void setScaleFactor(double d) {if (d>1) m_scaleFactor = d; emit(scaleFactorChanged(m_scaleFactor));}
        void setUseCannyPruning(bool b) {m_useCannyPruning = b; emit(useCannyPruningChanged(m_useCannyPruning));}
        void setMinWidth(int val) {if(val>=0) m_minWidth = val; emit( minWidthChanged(m_minWidth) ); }
        void setMinHeight(int val) {if(val>=0) m_minHeight = val; emit( minHeightChanged(m_minHeight) ); }
        void setHaarCascadeFile(QString filename) { m_haarCascadeFile = filename; emit( haarCascadeFileChanged(filename)); }

    private:
        plv::OpenCVImageInputPin* m_inputPin;
        plv::OpenCVImageOutputPin* m_outputPin;

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
