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

#include "Pipeline.h"

#include <QDebug>
#include <QStringBuilder>
#include <list>
#include <QtConcurrentRun>
#include <QTime>

#include "PipelineElement.h"
#include "PipelineProcessor.h"
#include "PipelineProducer.h"
#include "Pin.h"
#include "PinConnection.h"
#include "PlvExceptions.h"

using namespace plv;

Pipeline::Pipeline() :
        m_pipelineThread(0),
        m_serial( 1 ),
        m_running(false),
        m_runQueueThreshold(10),
        m_stepwise(false),
        m_producersReady(false),
        m_numFramesSinceLastFPSCalculation(0),
        m_fps(-1.0f)
{
}

Pipeline::~Pipeline()
{
    assert(!m_running );
    assert(m_children.isEmpty());
    assert(m_processors.isEmpty());
    assert(m_producers.isEmpty());
}

bool Pipeline::setThread(QThread* thread)
{
    assert( !isRunning() );
    assert( thread != 0 );
    assert( m_pipelineThread == 0 );

    QMutexLocker lock(&m_pipelineMutex);
    if( m_pipelineThread != 0 )
        return false;

    m_pipelineThread = thread;

    QMapIterator<int, RefPtr<PipelineElement> > itr( m_children );
    while(itr.hasNext())
    {
        itr.next();
        RefPtr<PipelineElement> elem = itr.value();
        elem->moveToThread(thread);
    }
    return true;
}

int Pipeline::addElement( PipelineElement* child ) throw (IllegalArgumentException)
{
    if( !canAddElement( child) )
    {
        QString msg = "Tried to add PipelineElement " % child->getName()
                      % " to pipeline with an id which is already in use.";
        throw IllegalArgumentException( msg );
    }

    RefPtr<PipelineElement> element = child;
    int id = element->getId();
    if( id == - 1 )
    {
        id = getNewPipelineElementId();
        element->setId( id );
    }

    // move the element to this thread
    // and this pipeline's event loop
    if( m_pipelineThread != 0 )
        element->moveToThread(m_pipelineThread);

    // for error reporting to GUI by pipeline elements
    // we used queued connections here so we do not accidentally deadlock
    connect(element.getPtr(), SIGNAL(onError(PlvErrorType, PipelineElement*)),
            this, SLOT(pipelineElementError(PlvErrorType, PipelineElement*)),
            Qt::QueuedConnection );

    QMutexLocker lock( &m_pipelineMutex );
    m_children.insert( id, element );
    emit elementAdded(element);

    RefPtr<PipelineProducer> producer = ref_ptr_dynamic_cast<PipelineProducer>(element);
    if( producer.isNotNull() )
    {
        m_producers.insert( id, producer );
    }

    RefPtr<PipelineProcessor> processor = ref_ptr_dynamic_cast<PipelineProcessor>(element);
    if( processor.isNotNull() )
    {
        m_processors.insert( id, processor );
    }
    return id;
}

bool Pipeline::canAddElement( PipelineElement* element )
{
    QMutexLocker lock( &m_pipelineMutex );
    int id = element->getId();
    if( id != -1 )
    {
        return( !m_children.contains( id ) );
    }
    return true;
}

int Pipeline::getNewPipelineElementId()
{
    QMutexLocker lock( &m_pipelineMutex );

    int id = 0;

    QMapIterator<int, RefPtr<PipelineElement> > itr( m_children );
    bool done = false;
    while( itr.hasNext() && !done )
    {
        itr.next();
        if( itr.key() > id )
            done = true;
        else
            ++id;
    }
    return id;
}

PipelineElement* Pipeline::getElement( int id )
{
    QMutexLocker lock( &m_pipelineMutex );

    if( m_children.contains( id ) )
    {
        return m_children.value( id );
    }
    return 0;
}

void Pipeline::removeElement( int id )
{
    QMutexLocker lock( &m_pipelineMutex );

    if( m_children.contains( id ) )
    {
        // preserve the element so we can send it over the signal later
        RefPtr<PipelineElement> element = m_children.value( id );
        removeConnectionsForElement( element );
        m_children.remove( id );
        if( m_producers.contains(id) )
        {
            m_producers.remove(id);
        }
        else if( m_processors.contains(id) )
        {
            m_processors.remove(id);
        }
        emit( elementRemoved(element) );
    }
}

void Pipeline::removeAllElements()
{
    QMutexLocker lock( &m_pipelineMutex );

    foreach( RefPtr<PipelineElement> element, m_children )
    {
        removeConnectionsForElement(element);
        emit elementRemoved(element);
    }
    m_children.clear();
    m_producers.clear();
    m_processors.clear();
}

const Pipeline::PipelineElementMap& Pipeline::getChildren() const
{
    return m_children;
}

const Pipeline::PipelineConnectionsList& Pipeline::getConnections() const
{
    return m_connections;
}

bool Pipeline::canConnectPins( IOutputPin* outputPin, IInputPin* inputPin,
                               QString reason )
{
    return PinConnection::canConnectPins( outputPin, inputPin, reason );
}

void Pipeline::connectPins( IOutputPin* outputPin, IInputPin* inputPin)
        throw (PinConnection::IllegalConnectionException,
               PinConnection::IncompatibleTypeException,
               PinConnection::DuplicateConnectionException)
{
    QMutexLocker lock( &m_pipelineMutex );

    RefPtr<PinConnection> connection = new PinConnection(outputPin, inputPin);
    m_connections.push_back(connection);
    emit( connectionAdded(connection) );
}

void Pipeline::pinConnectionDisconnect( PinConnection* connection )
{
    assert( connection != 0 );
    QMutexLocker lock( &m_pipelineMutex );
    threadUnsafeDisconnect( connection );
}

bool Pipeline::init()
{
    QMapIterator<int, RefPtr<PipelineElement> > itr( m_children );
    QSet<PipelineElement*> initialized;
    while( itr.hasNext() )
    {
        itr.next();
        RefPtr<PipelineElement> element = itr.value();

        PlvErrorType errType = PlvNoError;
        QString msg;

        try
        {
            if( !element->__init() )
            {
                errType = element->getErrorType();
                msg = tr("Error in PipelineElement %1: %2")
                      .arg(element->getName())
                      .arg(element->getErrorString());
            }
        }
        catch( PlvException& e )
        {
            errType = PlvExceptionThrownError;
            msg = tr("Error in PipelineElement %1: %2").arg(element->getName()).arg(e.what());
        }
        catch( ... )
        {
            errType = PlvExceptionThrownError;
            msg = tr("Unknown exception caught in PipelineElement %1" ).arg(element->getName());
        }

        if( errType != PlvNoError )
        {
            element->__deinit();
            emit handleMessage( QtFatalMsg, msg );

            foreach( PipelineElement* element, initialized )
            {
                element->__deinit();
            }
            return false;
        }
        initialized.insert(element.getPtr());
    }
    return true;
}

void Pipeline::clear()
{
    assert( !m_running );

    // we need to explicitly remove the connections
    // and the children because they hold a ref pointer
    // to Pipeline and will prevent us from deleting ourselves
    removeAllConnections();
    removeAllElements();

    m_ordering.clear();
    m_producers.clear();
    m_processors.clear();
}

void Pipeline::start()
{
    // TODO call init somwhere else
    if( !this->init() )
    {
        // error already handled in init
        return;
    }

    QMutexLocker lock( &m_pipelineMutex );

    // check if all required pins of all elements are connected
    foreach( RefPtr<PipelineElement> element, m_children )
    {
        if( !element->requiredPinsConnected() )
        {
            QString msg = tr("PipelineElement's' %1 required pins are not all connected.").arg(element->getName());
            handleMessage(QtFatalMsg, msg);
            return;
        }
    }

    // check for cycles
    if( !generateGraphOrdering( m_ordering ) )
    {
        QString msg = tr("Cycle detected in pipeline graph");
        handleMessage(QtFatalMsg, msg);
        return;
    }

    bool error = false;
    QMapIterator<int, RefPtr<PipelineElement> > itr( m_children );
    QSet<PipelineElement*> started;
    while( itr.hasNext() && !error )
    {
        itr.next();
        RefPtr<PipelineElement> element = itr.value();
        try
        {
            element->__start();
            started.insert( element.getPtr() );
        }
        catch( PlvException& e )
        {
            QString msg = element->getName() % ": " % e.what();
            handleMessage(QtCriticalMsg, msg);
            element->__deinit();
            error = true;
        }
    }

    // cleanup after error
    if( error )
    {
        foreach( PipelineElement* element, started )
        {
            element->__stop();
            element->__deinit();
        }
        return;
    }

    // start the heartbeat at 100Hz
    connect(&m_heartbeat, SIGNAL(timeout()), this, SLOT(schedule()));
    m_heartbeat.start(10);

    m_running = true;
    m_runQueueThreshold = m_processors.size();
    m_timeSinceLastFPSCalculation.start();
    lock.unlock();
    emit pipelineStarted();
}

void Pipeline::stop()
{
    QMutexLocker lock( &m_pipelineMutex );

    assert(m_running);

    // stop the heartbeat
    m_heartbeat.stop();
    m_heartbeat.disconnect();
    disconnect(this, SLOT(schedule()));

    // stop requested, wait while all processors finish
    // TODO insert a timeout here for elements which will not finish
    while( m_runQueue.size() != 0 )
    {
        for( int i=0; i<m_runQueue.size(); ++i )
        {
            PipelineElement* element = m_runQueue.at(i).element();
            PipelineElement::State state = element->getState();
            if( state == PipelineElement::DONE || state == PipelineElement::ERROR )
            {
                m_runQueue.removeAt(i);
                element->setState(PipelineElement::STARTED);
            }
        }
    }

    // TODO formalize this procedure (s of pipeline) more!
    QMapIterator<int, RefPtr<PipelineElement> > itr( m_children );
    while( itr.hasNext() )
    {
        itr.next();
        itr.value()->__stop();
        itr.value()->__deinit();
    }

    foreach(RefPtr<PinConnection> conn, m_connections)
    {
        conn->flush();
        assert(!conn->hasData());
    }

    m_running = false;
    lock.unlock();
    emit pipelineStopped();
}

void Pipeline::finish()
{
    QMutexLocker lock( &m_pipelineMutex );

    // stop the pipeline
    if( m_running )
    {
        lock.unlock();
        stop();
        lock.relock();
    }

    // clear the pipeline
    clear();

    // tell everyone we are finished
    emit finished();
}

void Pipeline::schedule()
{
    QMutexLocker lock( &m_pipelineMutex );

    // remove stale entries
    //if( m_runQueue.size() > m_runQueueThreshold )
    //{
        for( int i=0; i<m_runQueue.size(); ++i )
        {
            PipelineElement* ple = m_runQueue.at(i).element();
            PipelineElement::State state = ple->getState();
            if( state == PipelineElement::DONE )
            {
                m_runQueue.removeAt(i);
                ple->setState(PipelineElement::STARTED);
            }
            else if(state == PipelineElement::ERROR)
            {
                // stop on error
                m_pipelineMutex.unlock();
                stop();
                emit pipelineMessage(QtFatalMsg, m_runQueue.at(i).element()->getErrorString() );
                return;
            }
        }
    //}

    // schedule processors
    QList<RunItem> newItems;
    QMapIterator<int, PipelineProcessor* > procItr( m_processors );
    while( procItr.hasNext() )
    {
        procItr.next();
        PipelineProcessor* proc = procItr.value();
        unsigned int serial;
        if( proc->__ready(serial) )
        {
            RunItem item( proc, serial );
            newItems.append( item );
        }
    }

    // sort on serial number, oldest first
    qSort( newItems.begin(), newItems.end() );
    for( QList<RunItem>::iterator listItr = newItems.begin();
         listItr != newItems.end();
         ++listItr )
    {
        RunItem& item = (*listItr);
        item.dispatch();
        m_runQueue.append(item);
    }

    // only if there are not too many processors
    // processing, call new production run
    if( m_runQueue.size() == 0 )
    {
        if(!m_producersReady)
        {
            QMapIterator<int, PipelineProducer* > itr( m_producers );
            bool ready = true;
            unsigned int dummy;
            while( itr.hasNext() && ready )
            {
                itr.next();
                ready = itr.value()->__ready(dummy);
            }
            m_producersReady = ready;
        }

        if( m_producersReady )
        {
            lock.unlock();
            step();
            lock.relock();
            m_producersReady = false;
        }
    }
}

void Pipeline::step()
{
    m_pipelineMutex.lock();

    // unsigned int will wrap around
    ++m_serial;

    // dispatch producers
    QMapIterator<int, PipelineProducer* > prodItr( m_producers );
    while( prodItr.hasNext() )
    {
        prodItr.next();
        RunItem item( prodItr.value(), m_serial );
        item.dispatch();
        m_runQueue.append(item);
    }
    m_pipelineMutex.unlock();

    ++m_numFramesSinceLastFPSCalculation;
    int elapsed = m_timeSinceLastFPSCalculation.elapsed();
    if( elapsed > 10000 )
    {
        // add one so elapsed is never 0 and
        // we do not get div by 0
        m_fps = (m_numFramesSinceLastFPSCalculation * 1000) / elapsed;
        //m_fps = m_fps == -1.0f ? fps : m_fps * 0.9f + 0.1f * fps;
        m_timeSinceLastFPSCalculation.restart();
        m_numFramesSinceLastFPSCalculation = 0;
        qDebug() << "FPS: " << (int)m_fps;
        emit framesPerSecond(m_fps);
    }

    emit stepTaken(m_serial);
}

//void Pipeline::run()
//{
//    m_running = true;
//    setStopRequested(false);
//    emit( pipelineStarted() );
//    QList<RunItem> m_runQueueu;

//    int numFrames = 0;
//    float fps = 10.0f;
//    QTime time;
//    time.start();

//    int threshold = m_children.size() * 8;
//    bool error = false;
//    while( !isStopRequested() && !error )
//    {
//        /** actual scheduling */

//        // produce something
//        if( m_runQueueu.size() <= threshold )
//        {
//            if( producersReady() )
//            {
//                m_pipelineMutex.lock();

//                /** unsigned int will wrap around */
//                ++m_serial;

//                QMapIterator<int, PipelineProducer* > itr( m_producers );
//                while( itr.hasNext() )
//                {
//                    itr.next();
//                    RunItem item( itr.value(), m_serial );
//                    item.dispatch();
//                    m_runQueueu.append(item);
//                }
//                m_pipelineMutex.unlock();

//                if( ++numFrames == 10 )
//                {
//                    // add one so elapsed is never 0 and
//                    // we do not get div by 0
//                    int elapsed = time.elapsed() + 1;
//                    fps = fps * 0.9f + ( 1000.0f / elapsed );
//                    time.restart();
//                    numFrames = 0;
//                    //qDebug() << "FPS: " << (int)fps;
//                }

//                emit( stepTaken(m_serial) );
//            }
//            else
//            {
//                usleep(0);
//            }

//            m_pipelineMutex.lock();
//            QList<RunItem> newItems;
//            QMapIterator<int, PipelineProcessor* > itr( m_processors );
//            while( itr.hasNext() )
//            {
//                itr.next();
//                PipelineProcessor* proc = itr.value();
//                unsigned int serial;
//                if( proc->__ready(serial) )
//                {
//                    RunItem item( proc, serial );
//                    newItems.append( item );
//                }
//            }

//            // sort on serial number
//            qSort( newItems.begin(), newItems.end() );
//            for( QList<RunItem>::iterator listItr = newItems.begin();
//                 listItr != newItems.end();
//                 ++listItr )
//            {
//                RunItem& item = (*listItr);
//                item.dispatch();
//                m_runQueueu.append(item);
//            }
//            m_pipelineMutex.unlock();
//        }
//        else
//        // remove stale entries
//        {
//            for( int i=0; i<m_runQueueu.size(); ++i )
//            {
//                PipelineElement::State state = m_runQueueu.at(i).element()->getState();
//                if( state == PipelineElement::DONE )
//                {
//                    m_runQueueu.removeAt(i);
//                }
//                else if( state == PipelineElement::ERROR )
//                {
//                    error = true;
//                }
//            }
//        }
//    }

//    // stop requested, wait while all processors finish
//    // TODO insert a timeout here for elements which will not finish
//    while( m_runQueueu.size() != 0 )
//    {
//        usleep(100);
//        for( int i=0; i<m_runQueueu.size(); ++i )
//        {
//            PipelineElement::State state = m_runQueueu.at(i).element()->getState();
//            if( state == PipelineElement::DONE || state == PipelineElement::ERROR )
//            {
//                m_runQueueu.removeAt(i);
//            }
//        }
//    }

//    m_running = false;
//}

/****************************************************************************/
/* PRIVATE FUNCTIONS                                                        */
/****************************************************************************/

// private function, relies on another function
// to lock mutex
void Pipeline::removeAllConnections()
{
    assert( !m_running );

    foreach( RefPtr<PinConnection> connection, m_connections )
    {
        connection->disconnect();
        emit( connectionRemoved( connection ) );
    }
    m_connections.clear();
}

void Pipeline::threadUnsafeDisconnect( PinConnection* connection )
{
    RefPtr<PinConnection> con( connection );
    con->disconnect();

    PipelineConnectionsList::iterator itr;
    for(itr = m_connections.begin();
        itr != m_connections.end(); ++itr)
    {
        RefPtr<PinConnection> con2 = *itr;
        if(con.getPtr() == con2.getPtr())
        {
            m_connections.erase(itr);
            emit( connectionRemoved(con) );
            break;
        }
    }
}

// private function, relies on another function
// to lock mutex
void Pipeline::removeConnectionsForElement( PipelineElement* element )
{
    RefPtr<PipelineElement> ple( element );

    // get a copy of the input pin map
    PipelineElement::InputPinMap inputPins = ple->getInputPins();
    for( PipelineElement::InputPinMap::const_iterator itr = inputPins.begin()
        ; itr!=inputPins.end(); ++itr)
    {
        RefPtr<IInputPin> ipp = itr->second;
        if( ipp->isConnected() )
        {
            RefPtr<PinConnection> connection = ipp->getConnection();
            threadUnsafeDisconnect( connection.getPtr() );
        }
    }

    // get a copy of the output pin map
    PipelineElement::OutputPinMap outputPins = ple->getOutputPins();
    for( PipelineElement::OutputPinMap::const_iterator outputPinItr = outputPins.begin()
        ; outputPinItr!=outputPins.end(); ++outputPinItr)
    {
        RefPtr<IOutputPin> opp = outputPinItr->second;

        std::list< RefPtr<PinConnection> > connections = opp->getConnections();
        for( std::list< RefPtr<PinConnection> >::const_iterator connItr = connections.begin();
             connItr!= connections.end(); ++connItr )
        {
            RefPtr<PinConnection> connection = *connItr;
            threadUnsafeDisconnect( connection.getPtr() );
        }
    }
}

bool Pipeline::generateGraphOrdering( QList<PipelineElement*>& ordering )
{
    assert( m_children.size() != 0 );

    QSet<PipelineElement*> endNodes;
    // find nodes with no outgoing connections
    foreach( RefPtr<PipelineElement> si, m_children )
    {
        if( si->isEndNode() )
        {
            endNodes.insert( si );
        }
    }

    if( endNodes.empty() )
    {
        // at least one cycle detected
        return false;
    }

    QSet<PipelineElement*> visited;
    foreach( PipelineElement* node, endNodes )
    {
        if( !node->visit( ordering, visited ) )
        {
            // cycle detected
            return false;
        }
    }

    // TODO shall we check if all nodes are visited?
    QString out;
    foreach( PipelineElement* si, ordering )
    {
        out.append( si->getName() % "->" );
    }
    qDebug() << "Partial ordering of graph: " << out;

    return true;
}

void Pipeline::pipelineElementError( PlvErrorType type, PipelineElement* ple )
{
    QtMsgType qtType = QtDebugMsg;

    switch(type)
    {
    case PlvNoError:
    case PlvNonFatalError:
        break;
    case PlvInitError:
    case PlvResourceError:
    case PlvExceptionThrownError:
    case PlvFatalError:
        qtType = QtFatalMsg;
        break;
    default:
        break;
    }

    QString msg = ple->getErrorString();
    handleMessage(qtType, msg);
}

void Pipeline::handleMessage(QtMsgType type, QString msg)
{
    QString dbg = QString("Pipeline received and relaying message of type %1: %2").arg(type).arg(msg);
    qDebug() << dbg;
    emit pipelineMessage(type, msg);
}


