/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvcore module of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

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

        bool schedule();

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

    signals:
        void errorOccurred( QString errorString );
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
        QString m_errorString;

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

        QString getErrorString() const { return m_errorString; }

    private:
        void setErrorString( const QString& error );
        ScheduleInfo( const ScheduleInfo&);
        void setDynamicPriority( int priority ) { m_dynamicPriority = priority; }
        void startTimer();
        void stopTimer();
        void run();
    };
}
#endif
