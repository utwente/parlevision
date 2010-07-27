#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "RefPtr.h"
#include "PipelineElement.h"
#include "PinConnection.h"

#include <QObject>
#include <QTime>
#include <QFuture>

namespace plv
{
    class Scheduler : public QObject
    {
        Q_OBJECT

    protected:
        QMap< int, ScheduleInfo* > m_scheduleInfo;

    public:
        Scheduler( Pipeline* pipeline );
        ~Scheduler();

        void schedule();

        void setActiveThreadCount(int num);
        int getActiveThreadCount();

    public slots:
        void start();
        void stop();

        void elementAdded(plv::RefPtr<plv::PipelineElement>);
        void elementRemoved(plv::RefPtr<plv::PipelineElement>);
        void elementChanged(plv::RefPtr<plv::PipelineElement>);

        void connectionAdded(plv::RefPtr<plv::PinConnection>);
        void connectionRemoved(plv::RefPtr<plv::PinConnection>);
        void connectionChanged(plv::RefPtr<plv::PinConnection>);
    };

    class ScheduleInfo
    {
    public:
        typedef enum _ScheduleState {
            UNDEFINED,  /* undefined or non initialized */
            READY,      /* ready for processing */
            WAITING,    /* waiting to be processed */
            RUNNING,    /* pipeline element is processing */
            DONE,       /* done processing */
            ERROR       /* an error occured during processing */
        } ScheduleState;

        typedef enum _PipelineElementType {
            PRODUCER,
            PROCESSOR,
            CONSUMER
        } PipelineElementType;

    protected:
        RefPtr<PipelineElement> m_element;
        PipelineElementType m_type;
        int m_staticPriority;
        int m_dynamicPriority;
        int m_avgProcessingTime;
        int m_lastProcesingTime;
        ScheduleState m_state;
        QMap<int, ScheduleInfo*> m_connectedTo;
        QFuture<void> m_result;
        QMutex m_mutex;
        QTime m_timer;

    public:
        ScheduleInfo( PipelineElement* pl, PipelineElementType type, int priority = 0 );

        void setStaticPriority( int priority ) { m_staticPriority = priority; }

        int getStaticPriority() const { return m_staticPriority; }

        int getDynamicPriority() const { return m_dynamicPriority; }

        PipelineElement* getElement() const { return m_element.getPtr(); }

        int getAvgProcessingTime() const { return m_avgProcessingTime; }

        /** Dispatches the pipeline element. This does not mean it will be
          * executed right away. The processor state will go into WAITING
          * until execution starts when it will change to RUNNING
          */
        void dispatch();
        inline int maxQueueSize();

        /** Updates the state by calling the isReadyForProcessing function
            of the PipelineElement or checking when the processor is in a
            RUNNING state if it has completed. This function is thread safe.
            @returns the updated state.
          */
        ScheduleState updateAndGetState();

        /** @returns the state of this processor. This method is thread safe. */
        ScheduleState getState();

        /** sets the state of this processor. This method is thread safe. */
        void setState( ScheduleState state );

    private:
        ScheduleInfo( const ScheduleInfo&);
        void setDynamicPriority( int priority ) { m_dynamicPriority = priority; }
        void startTimer();
        void stopTimer();
        void run();
    };
}
#endif
