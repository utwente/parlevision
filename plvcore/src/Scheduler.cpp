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

#include "Scheduler.h"

#include "Pipeline.h"

#include "PipelineProducer.h"
#include "PipelineProcessor.h"
#include "Pin.h"

#include <QtConcurrentRun>
#include <QTime>
#include <QFuture>

#include <unistd.h>

using namespace plv;

/********** SCHEDULER *********************************************/

Scheduler::Scheduler( Pipeline* pipeline )
{
    connect(pipeline,
            SIGNAL(elementAdded(plv::RefPtr<plv::PipelineElement>)),
            this,
            SLOT(elementAdded(plv::RefPtr<plv::PipelineElement>))
            );

    connect(pipeline,
            SIGNAL( elementChanged(plv::RefPtr<plv::PipelineElement>) ),
            this,
            SLOT( elementChanged(plv::RefPtr<plv::PipelineElement>) )
            );

    connect(pipeline,
            SIGNAL(elementRemoved(plv::RefPtr<plv::PipelineElement>)),
            this,
            SLOT(elementRemoved(plv::RefPtr<plv::PipelineElement>))
            );

    connect(pipeline,
            SIGNAL(connectionAdded(plv::RefPtr<plv::PinConnection>)),
            this,
            SLOT(documentChanged())
            );

    connect(pipeline,
            SIGNAL(connectionChanged(plv::RefPtr<plv::PinConnection>)),
            this,
            SLOT(connectionChanged(plv::RefPtr<plv::PinConnection>))
            );

    connect(pipeline,
            SIGNAL(connectionRemoved(plv::RefPtr<plv::PinConnection>)),
            this,
            SLOT(connectionRemoved(plv::RefPtr<plv::PinConnection>))
            );
}

Scheduler::~Scheduler()
{
    foreach( ScheduleInfo* si, m_scheduleInfo )
    {
        delete si;
    }
}

void Scheduler::start()
{
}

void Scheduler::stop()
{
}

void Scheduler::elementAdded(plv::RefPtr<plv::PipelineElement> ple )
{
    ScheduleInfo::PipelineElementType type;

    RefPtr<PipelineProducer> plp = ref_ptr_dynamic_cast<PipelineProducer>( ple );
    if( plp.isNotNull() )
    {
        // producer
        type = ScheduleInfo::PRODUCER;
    }
    else
    {
        // processor
        type = ScheduleInfo::PROCESSOR;
    }

    ScheduleInfo* si = new ScheduleInfo( ple.getPtr(), type, 0 );

    QMutexLocker lock( &m_schedulerMutex );
    m_scheduleInfo.insert( ple->getId(), si );
}

void Scheduler::elementRemoved(plv::RefPtr<plv::PipelineElement> ple )
{
    QMutexLocker lock( &m_schedulerMutex );
    m_scheduleInfo.remove( ple->getId() );
}

void Scheduler::elementChanged(plv::RefPtr<plv::PipelineElement> ple )
{
    // TODO not implemented yet
}

void Scheduler::connectionAdded(plv::RefPtr<plv::PinConnection> connection )
{
    // TODO not implemented yet
}

void Scheduler::connectionRemoved(plv::RefPtr<plv::PinConnection>)
{
    // TODO not implemented yet
}

void Scheduler::connectionChanged(plv::RefPtr<plv::PinConnection>)
{
    // TODO not implemented yet
}

bool Scheduler::schedule()
{
    m_schedulerMutex.lock();

    foreach( ScheduleInfo* si, m_scheduleInfo )
    {
        ScheduleInfo::ScheduleState state = si->updateAndGetState();

        switch( state )
        {
        case ScheduleInfo::READY:
            si->dispatch();
            break;
        case ScheduleInfo::ERROR:
        {
            QString errStr = si->getErrorString();
            emit( errorOccurred( si->getElement()->getName() + ": "
                                 + errStr ) );
            m_schedulerMutex.unlock();
            return false;
        }
        case ScheduleInfo::UNDEFINED:
        case ScheduleInfo::WAITING:
        case ScheduleInfo::RUNNING:
        case ScheduleInfo::DONE:
        default:
            break;
        }
    }

    // give other functions a go
    m_schedulerMutex.unlock();
    m_schedulerMutex.lock();

    int numBusy;
    do
    {
        numBusy = 0;
        foreach( ScheduleInfo* si, m_scheduleInfo )
        {
            if( si->getState() == ScheduleInfo::RUNNING )
            {
                ++numBusy;
            }
        }
        usleep( 0 );
    }
    while( numBusy > 0 );

    m_schedulerMutex.unlock();
    return true;
}

void Scheduler::setActiveThreadCount( int num )
{
    QThreadPool::globalInstance()->setMaxThreadCount( num );
}

int Scheduler::getActiveThreadCount()
{
    return QThreadPool::globalInstance()->activeThreadCount();
}


/********** SCHEDULE INFO *****************************************/

ScheduleInfo::ScheduleInfo( PipelineElement* pl, PipelineElementType type, int priority ) :
    m_element( pl ),
    m_type( type ),
    m_staticPriority( priority ),
    m_dynamicPriority( priority ),
    m_avgProcessingTime( 0 ),
    m_lastProcesingTime( 0 ),
    m_state( UNDEFINED )
{
}

void ScheduleInfo::dispatch()
{
    assert( getState() == READY );
    setState( WAITING );
    m_result = QtConcurrent::run( this, &ScheduleInfo::run );
}

void ScheduleInfo::run()
{
    assert( getState() == WAITING );
    setState( RUNNING );

    startTimer();
    try
    {
        m_element->__process();
    }
    catch( PipelineException& pe )
    {
        qDebug() << "Uncaught exception in PipelineElement::process()"
                 << "of type PipelineException with message: " << pe.what();
        stopTimer();
        setState( ERROR );
        setErrorString( pe.what() );
        return;
    }
    catch( IllegalAccessException& iae )
    {
        qDebug() << "Uncaught exception in PipelineElement::process()"
                 << "of type IllegalAccessException with message: " << iae.what();
        stopTimer();
        setState( ERROR );
        setErrorString( iae.what() );
        return;
    }
    catch( PlvException& e )
    {
        qDebug() << "Uncaught exception in PipelineElement::process()"
                 << "of type PlvException with message: " << e.what();
        stopTimer();
        setState( ERROR );
        setErrorString( e.what() );
        return;
    }
    catch( std::runtime_error& err )
    {
        qDebug() << "Uncaught exception in PipelineElement::process()"
                 << "of type PlvException with message: " << err.what();
        stopTimer();
        setState( ERROR );
        setErrorString( err.what() );
        return;
    }
    catch( ... )
    {
        qDebug() << "Uncaught exception in PipelineElement::process()"
                 << "of unknown type.";
        stopTimer();
        setState( ERROR );
        setErrorString( "Unknown exception caught" );
        return;
    }
    stopTimer();
    setState( DONE );
}

void ScheduleInfo::setState( ScheduleState state )
{
    QMutexLocker lock( &m_mutex );
    m_state = state;
}

void ScheduleInfo::setErrorString( const QString& error )
{
    QMutexLocker lock( &m_mutex );
    m_errorString = error;
}

ScheduleInfo::ScheduleState ScheduleInfo::getState()
{
    QMutexLocker lock( &m_mutex );
    return m_state;
}

int ScheduleInfo::maxQueueSize()
{
    QMutexLocker lock( &m_mutex );
    assert( m_state != RUNNING );
    return m_element->maxInputQueueSize();
}

ScheduleInfo::ScheduleState ScheduleInfo::updateAndGetState()
{
    QMutexLocker lock( &m_mutex );
    switch( m_state )
    {
    case READY:
    case WAITING:
    case ERROR:
        break;

    case RUNNING:
        // TODO check for timeout
        // not possible right now
        break;

    case UNDEFINED:
    case DONE:
        if( m_element->__isReadyForProcessing() )
        {
            m_state = READY;
        }
        break;
    }

    return m_state;
}

ScheduleInfo::ScheduleInfo( const ScheduleInfo& )
{
}

void ScheduleInfo::startTimer()
{
    m_timer.start();
}

void ScheduleInfo::stopTimer()
{
    int elapsed = m_timer.elapsed();
    m_avgProcessingTime = m_avgProcessingTime > 0 ?
                          (int) (( elapsed * 0.01f ) + ( m_avgProcessingTime * 0.99f )) : elapsed;
    m_lastProcesingTime = elapsed;
}

