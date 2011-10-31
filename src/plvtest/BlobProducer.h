#ifndef BLOBPRODUCER_H
#define BLOBPRODUCER_H

#include <plvcore/PipelineProducer.h>
#include <plvcore/CvMatDataPin.h>

class BlobProducer : public plv::PipelineProducer
{
public:
    Q_OBJECT
    Q_DISABLE_COPY( BlobProducer )

    Q_CLASSINFO("author", "Richard Loos")
    Q_CLASSINFO("name", "BlobProducer")
    Q_CLASSINFO("description", "A producer which generates an image with blobs.")
    Q_PROPERTY( int maxStep READ getMaxStep WRITE setMaxStep NOTIFY maxStepChanged )
    Q_PROPERTY( int numBlobs READ getNumBlobs WRITE setNumBlobs NOTIFY numBlobsChanged )

    /** required standard method declaration for plv::PipelineProcessor */
    PLV_PIPELINE_PRODUCER

    BlobProducer();
    virtual ~BlobProducer();

    bool init();

    int getMaxStep() const;
    int getNumBlobs() const;

public slots:
    void setMaxStep(int step);
    void setNumBlobs(int num);

signals:
    void maxStepChanged(int s);
    void numBlobsChanged(int n);

private:
    int m_width;
    int m_height;
    int m_maxStep;
    int m_numBlobs;
    double m_factor;
    QVector<cv::Point> m_positions;
    QVector<cv::Point> m_targets;
    plv::CvMatDataOutputPin* m_outputPin;
};

#endif // BLOBPRODUCER_H
