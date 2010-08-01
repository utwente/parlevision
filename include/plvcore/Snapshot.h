#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {

    class Pipeline;
    class OpenCVImage;

    class Snapshot : public PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Dennis")
        Q_CLASSINFO("name", "Snapshot")
        Q_CLASSINFO("description", "On request, make a new snapshot of the input pin. On the output pin, always present the latest snapshot. Always enforces a snapshot on the first frame.");

        Q_PROPERTY( bool makeSnapshot READ getMakeSnapshot WRITE setMakeSnapshot NOTIFY makeSnapshotChanged  )


    public:
        Snapshot();
        ~Snapshot();

        virtual void init() throw (PipelineException);
        /**override: every start, a new snapshot must be enforced*/
        virtual void start() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        bool getMakeSnapshot() { return m_makeSnapshot; }

    signals:
        void makeSnapshotChanged(bool newValue);

    public slots:
        void setMakeSnapshot(bool b) {m_makeSnapshot = b; emit(makeSnapshotChanged(b));}

    private:
        InputPin<OpenCVImage>* m_inputPin;
        OutputPin<OpenCVImage>* m_outputPin;
        RefPtr<OpenCVImage> m_imgSnapshot;

        bool m_makeSnapshot;

    };

}

#endif // SNAPSHOT_H
