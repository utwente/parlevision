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
#include <QMutableMapIterator>

#include "PipelineElement.h"
#include "DataConsumer.h"
#include "DataProducer.h"
#include "PipelineProcessor.h"
#include "PipelineProducer.h"
#include "PinConnection.h"
#include "PlvExceptions.h"
#include "PipelineLoader.h"
#include "IInputPin.h"
#include "IOutputPin.h"

using namespace plv;

Pipeline::Pipeline() :
        m_serial( 1 ),
        m_running(false),
        m_runQueueThreshold(10),
        m_stepwise(false),
        m_producersReady(false),
        m_numFramesSinceLastFPSCalculation(0),
        m_fps(-1.0f),
        m_testCount(0)
{
    //m_pipelineThread.start();
}

Pipeline::~Pipeline()
{
    assert(!m_running );

    if(!m_children.isEmpty())
    {
        clear();
    }

    assert(m_children.isEmpty());
    assert(m_processors.isEmpty());
    assert(m_producers.isEmpty());
}

int Pipeline::addElement( PipelineElement* child )
{
    if( !canAddElement( child) )
    {
        QString msg = tr("Tried to add PipelineElement %1 to "
                         "pipeline with an id which is already in use or invalid.")
                        .arg(child->getName());
        emit pipelineMessage(QtWarningMsg, msg);
    }

    RefPtr<PipelineElement> element = child;
    int id = element->getId();
    if( id == - 1 )
    {
        id = getNewPipelineElementId();
        element->setId( id );
    }

    element->setPipeline(this);

    // move the element to this thread
    // and this pipeline's event loop
    //element->moveToThread(&m_pipelineThread);

    // set the object name for easier debugging
    element->setObjectName(element->getName());

    // for error reporting to GUI by pipeline elements
    // we use queued connections here so we do not accidentally deadlock
    connect(element.getPtr(), SIGNAL(onError(PlvErrorType, plv::PipelineElement*)),
            this, SLOT(pipelineElementError(PlvErrorType, plv::PipelineElement*)),
            Qt::QueuedConnection );

    // for sending messages to GUI by pipeline elements
    // we use queued connections here so we do not accidentally deadlock
    connect(element.getPtr(),
            SIGNAL( sendMessageToPipeline(PlvMessageType, QString)),
            this,
            SLOT( handleMessage(PlvMessageType,QString)),
            Qt::QueuedConnection );

    QMutexLocker lock( &m_pipelineMutex );
    m_children.insert( id, element );

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

    setChanged(true);
    lock.unlock();

    emit elementAdded(id);
    emit elementAdded(element);
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

int Pipeline::getNewPinConnectionId()
{
    QMutexLocker lock( &m_pipelineMutex );

    int id = 0;

    QMapIterator<int, RefPtr<PinConnection> > itr( m_connections );
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

PipelineElement* Pipeline::getElement(int id)
{
    QMutexLocker lock( &m_pipelineMutex );
    if( m_children.contains( id ) )
    {
        return m_children.value( id );
    }
    return 0;
}

PinConnection* Pipeline::getConnection(int id)
{
    QMutexLocker lock( &m_pipelineMutex );
    if( m_connections.contains(id) )
    {
        return m_connections.value(id);
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
        setChanged(true);
        lock.unlock();
        emit elementRemoved(id);
        emit elementRemoved(element);
    }
}

const Pipeline::PipelineElementMap& Pipeline::getChildren() const
{
    return m_children;
}

const Pipeline::PipelineConnectionMap& Pipeline::getConnections() const
{
    return m_connections;
}

bool Pipeline::canConnectPins( IOutputPin* outputPin, IInputPin* inputPin,
                               QString& reason )
{
    return PinConnection::canConnectPins( outputPin, inputPin, reason );
}

int Pipeline::connectPins( IOutputPin* outputPin, IInputPin* inputPin)
        throw (PinConnection::IllegalConnectionException,
               PinConnection::IncompatibleTypeException,
               PinConnection::DuplicateConnectionException)
{
    int id = getNewPinConnectionId();
    RefPtr<PinConnection> connection = new PinConnection(id, outputPin, inputPin);

    QMutexLocker lock( &m_pipelineMutex );
    m_connections.insert(id, connection);
    lock.unlock();

    emit connectionAdded(id);
    emit connectionAdded(connection);
    return id;
}

void Pipeline::pinConnectionDisconnect( int id )
{
    QMutexLocker lock( &m_pipelineMutex );
    threadUnsafeDisconnect( id );
}

void Pipeline::pipelineDataConsumerReady(unsigned int serial, DataConsumer *consumer)
{
    QMutexLocker lock(&m_readyQueueMutex);
    RunItem item(consumer, serial);
    m_readyQueue.append(item);
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
        catch(Exception& e)
        {
            errType = PlvPipelineRuntimeError;
            msg = tr("Exception caught in PipelineElement %1: %2").arg(element->getName()).arg(e.what());
        }
        catch(...)
        {
            errType = PlvPipelineRuntimeError;
            msg = tr("Unknown exception caught in PipelineElement %1" ).arg(element->getName());
        }

        if( errType != PlvNoError )
        {
            element->__deinit();
            emit handleMessage( QtWarningMsg, msg );

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

void Pipeline::deinit()
{
    QMapIterator<int, RefPtr<PipelineElement> > itr( m_children );
    while( itr.hasNext() )
    {
        itr.next();
        RefPtr<PipelineElement> element = itr.value();

        PlvErrorType errType = PlvNoError;
        QString msg;


        if( !element->__deinit() )
        {
            errType = element->getErrorType();
            msg = tr("Error in PipelineElement %1: %2")
                  .arg(element->getName())
                  .arg(element->getErrorString());

            if( errType != PlvNoError )
            {
                emit handleMessage( QtWarningMsg, msg );
            }
        }
    }
}

bool Pipeline::isEmpty()
{
    QMutexLocker lock( &m_pipelineMutex );

    // no need to check for connection: if there are no
    // children, there are no connections
    return m_children.isEmpty();
}

void Pipeline::clear()
{
    QMutexLocker lock( &m_pipelineMutex );

    assert( !m_running );

    // we need to explicitly remove the connections
    // and the children because they hold a ref pointer
    // to Pipeline and will prevent us from deleting ourselves
    removeAllConnections();
    removeAllElements();
    setChanged(true);
}

void Pipeline::start()
{
    int numThreads = QThreadPool::globalInstance()->maxThreadCount();

    qDebug() << "Starting pipeline using " << numThreads << " threads.";

    QMutexLocker lock( &m_pipelineMutex );

    if( m_children.size() == 0 )
        return;

    lock.unlock();
    // TODO call init somwhere else
    if( !this->init() )
    {
        // error already handled in init
        return;
    }
    lock.relock();


    // check if all required pins of all elements are connected
    foreach( RefPtr<PipelineElement> element, m_children )
    {
        if( !element->requiredPinsConnected() )
        {
            QString msg = tr("PipelineElement's' %1 required pins are not all connected.").arg(element->getName());
            handleMessage(QtWarningMsg, msg);
            deinit();
            return;
        }
    }

    // check for cycles
    m_ordering.clear();
    if( !generateGraphOrdering( m_ordering ) )
    {
        QString msg = tr("Cycle detected in pipeline graph");
        handleMessage(QtWarningMsg, msg);
        return;
    }
    m_serial = 0;
    m_pipelineStages.clear();
    for( int i=0; i < numThreads; ++i )
        m_pipelineStages.insert(++m_serial, 0);

    bool error = false;
    QMapIterator<int, RefPtr<PipelineElement> > itr( m_children );
    QSet<PipelineElement*> started;
    while( itr.hasNext() && !error )
    {
        itr.next();
        RefPtr<PipelineElement> element = itr.value();
        try
        {
            if( element->__start() )
            {
                started.insert( element.getPtr() );
            }
            else
            {
                QString msg = tr("PipelineElement %1 failed to start.").arg(element->getName());
                handleMessage(QtWarningMsg, msg);
                element->__deinit();
                error = true;
            }
        }
        catch( Exception& e )
        {
            QString msg = element->getName() % ": " % e.what();
            handleMessage(QtWarningMsg, msg);
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
    m_runQueueThreshold = m_processors.size() + m_producers.size() + 1;
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
            PipelineElement* element = m_runQueue.at(i).getElement();
            PipelineElement::State state = element->getState();
            if( state == PipelineElement::PLE_DONE ||
                state == PipelineElement::PLE_ERROR )
            {
                m_runQueue.removeAt(i);
                element->setState(PipelineElement::PLE_STARTED);
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
    // stop the pipeline
    if( isRunning() )
    {
        stop();
    }

    // clear the pipeline
    clear();

    // tell everyone we are finished
    emit finished();
}

void Pipeline::schedule()
{
    QMutexLocker pleLock(&m_pipelineMutex);
    ++m_testCount;
    assert( m_testCount == 1);
    pleLock.unlock();

    // remove stale entries
    for( int i=0; i<m_runQueue.size(); ++i )
    {
        RunItem runItem = m_runQueue.at(i);
        QFuture<bool> future = runItem.getFuture();
        if( future.isFinished() )
        {
            m_runQueue.removeAt(i);
            bool result = future.result();
            if( !result )
            {
                // stop on error
                QString msg = tr("Pipeline stopped because of an error in %1. The error is %2")
                              .arg(runItem.getElement()->getName())
                              .arg(runItem.getElement()->getErrorString());
                stop();
                emit pipelineMessage(QtWarningMsg, msg);
                return;
            }
            runItem.getElement()->setState(PipelineElement::PLE_STARTED);
        }
    }

    // dispatch processors
    QMutexLocker rqLock(&m_readyQueueMutex);
    foreach( RunItem readyItem, m_readyQueue )
    {
        PipelineElement* readyElem = readyItem.getElement();
        if( readyElem->getState() < PipelineElement::PLE_DISPATCHED )
        {
            readyItem.dispatch();
            m_runQueue.append( readyItem );
            m_readyQueue.removeFirst();
        }
    }
    rqLock.unlock();

    // run producers
    if(true)
    {
        bool allReady = true;

        // check if all producers are ready
        foreach( PipelineProducer* producer, m_producers)
        {
            unsigned int serial;
            if( !producer->__ready(serial))
                allReady = false;
        }

        if( allReady )
        {
            foreach( PipelineProducer* producer, m_producers)
            {
                RunItem item( producer, m_serial );
                item.dispatch();
                m_runQueue.append( item );
            }

            // unsigned int will wrap around
            ++m_serial;

            ++m_numFramesSinceLastFPSCalculation;
            int elapsed = m_timeSinceLastFPSCalculation.elapsed();
            if( elapsed > 10000 )
            {
                m_fps = (m_numFramesSinceLastFPSCalculation * 1000) / elapsed;
                m_timeSinceLastFPSCalculation.restart();
                m_numFramesSinceLastFPSCalculation = 0;
                qDebug() << "FPS: " << (int)m_fps;
                emit framesPerSecond(m_fps);
            }
            emit stepTaken(m_serial);
        }
    }

#if 0
    int maxIdx = m_ordering.size();
    QMutableMapIterator<unsigned int, int> itr(m_pipelineStages);
    while( itr.hasNext() && maxIdx >= 0 )
    {
        itr.next();
        unsigned int serial = itr.key();
        int idx = itr.value();
        bool stop = false;
        while( !stop )
        {
            PipelineElement* element = m_ordering.at(idx);
            if( !element->__ready(serial) )
            {
                stop = true;
                maxIdx = idx - 1;
            }
            else
            {
                assert( idx <= maxIdx );
                if(idx == maxIdx)
                {
                    stop = true;
                    maxIdx -= 1;
                }
                else
                {
                    RunItem item( element, serial );
                    item.dispatch();
                    m_runQueue.append( item );
                    ++idx;
                    itr.setValue(idx);
                    if( idx == m_ordering.size() )
                    {
                        itr.remove();

                        // unsigned int will wrap around
                        ++m_serial;

                        // add a new stage
                        m_pipelineStages.insert(m_serial, 0);

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
                        emit stepTaken(serial);
                        stop = true;
                    }
                }
            }
        }
    }
#endif

    pleLock.relock();
    --m_testCount;
    assert( m_testCount == 0);
    pleLock.unlock();

}

/****************************************************************************/
/* PRIVATE FUNCTIONS                                                        */
/****************************************************************************/

void Pipeline::removeAllElements()
{
    foreach( RefPtr<PipelineElement> element, m_children )
    {
        removeConnectionsForElement(element);
        emit elementRemoved(element->getId());
        emit elementRemoved(element);
    }
    m_ordering.clear();
    m_children.clear();
    m_producers.clear();
    m_processors.clear();
}

// private function, relies on another function
// to lock mutex
void Pipeline::removeAllConnections()
{
    assert( !m_running );

    foreach( RefPtr<PinConnection> connection, m_connections )
    {
        connection->disconnect();
        emit connectionRemoved(connection->getId());
        emit connectionRemoved(connection);
    }
    m_connections.clear();
}

void Pipeline::threadUnsafeDisconnect( int id )
{
    if( !m_connections.contains(id))
    {
        qWarning("Pipeline::threadUnsafeDisconnect: tried to remove pin with invalid id. Ignoring.");
        Q_ASSERT(false);
        return;
    }
    RefPtr<PinConnection> connection = m_connections.value(id);
    connection->disconnect();
    m_connections.remove(id);

    emit connectionRemoved(connection->getId());
    emit connectionRemoved(connection);
}

// private function, relies on another function
// to lock mutex
void Pipeline::removeConnectionsForElement( PipelineElement* element )
{
    RefPtr<PipelineElement> ple( element );

    RefPtr<DataConsumer> dc = ref_ptr_dynamic_cast<DataConsumer>(ple);
    if( dc.isNotNull() )
    {
        // get a copy of the input pin map
        InputPinMap inputPins = dc->getInputPins();
        for( InputPinMap::const_iterator itr = inputPins.begin()
            ; itr!=inputPins.end(); ++itr)
        {
            RefPtr<IInputPin> ipp = itr.value();
            if( ipp->isConnected() )
            {
                RefPtr<PinConnection> connection = ipp->getConnection();
                threadUnsafeDisconnect( connection->getId() );
            }
        }
    }

    RefPtr<DataProducer> dp = ref_ptr_dynamic_cast<DataProducer>(ple);
    if( dp.isNotNull() )
    {
        // get a copy of the output pin map
        OutputPinMap outputPins = dp->getOutputPins();
        for( OutputPinMap::const_iterator outputPinItr = outputPins.begin()
            ; outputPinItr!=outputPins.end(); ++outputPinItr)
        {
            RefPtr<IOutputPin> opp = outputPinItr.value();

            std::list< RefPtr<PinConnection> > connections = opp->getConnections();
            for( std::list< RefPtr<PinConnection> >::const_iterator connItr = connections.begin();
                 connItr!= connections.end(); ++connItr )
            {
                RefPtr<PinConnection> connection = *connItr;
                threadUnsafeDisconnect( connection->getId() );
            }
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
        RefPtr<DataProducer> dp = ref_ptr_dynamic_cast<DataProducer>(si);
        if( dp.isNotNull() )
        {
            if( si->isEndNode() )
            {
                endNodes.insert( si );
            }
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
    case PlvNonFatalError:
    case PlvPipelineInitError:
    case PlvPipelineRuntimeError:
        qtType = QtWarningMsg;
        break;
    case PlvFatalError:
        qtType = QtFatalMsg;
        break;
    default:
        break;
    }

    QString msg = ple->getErrorString();
    handleMessage(qtType, msg);
	
	// stop the pipeline
	stop();
}

void Pipeline::handleMessage(PlvMessageType type, const QString& msg)
{
    QString dbg = QString("Pipeline received and relaying message of type %1: %2").arg(type).arg(msg);
    qDebug() << dbg;

    QtMsgType qtype = QtWarningMsg;
    switch( type )
    {
        case PlvDebugMessage:
            qtype = QtDebugMsg;
            break;
        case PlvNotifyMessage:
        case PlvWarningMessage:
        case PlvErrorMessage:
        //   qtype = QtWarningMsg;
            break;
    }
    emit pipelineMessage(qtype, msg);
}

void Pipeline::handleMessage(QtMsgType type, const QString& msg)
{
    QString dbg = QString("Pipeline received and relaying message of type %1: %2").arg(type).arg(msg);
    qDebug() << dbg;
    emit pipelineMessage(type, msg);
}

bool Pipeline::load( const QString& filename )
{
    if( !isEmpty() )
    {
        QString msg = tr("Pipeline load called on pipeline which is not empty. Ignoring.");
        qWarning() << msg;
        Q_ASSERT_X(false, __FUNCTION__, msg.toAscii());
        return false;
    }
    m_filename = filename;
    try
    {
        PipelineLoader::deserialize( m_filename, this );
        emit pipelineLoaded(m_filename);
    }
    catch( std::runtime_error& e )
    {

        QString msg = tr("Failed to load pipeline %1").arg(e.what());
        handleMessage(QtWarningMsg, msg);
        return false;
    }
    return true;
}

bool Pipeline::save()
{
    try
    {
        if(isChanged())
        {
            PipelineLoader::serialize( m_filename, this );
            setChanged(false);
            emit pipelineSaved(m_filename);
        }
    }
    catch( std::runtime_error& e )
    {
        QString msg = tr("Failed to save pipeline %1").arg(e.what());
        handleMessage(QtWarningMsg, msg);
        return false;
    }
    return true;
}

bool Pipeline::saveAs( const QString& filename )
{
    if( QString::compare(m_filename, filename) != 0 )
    {
        m_filename = filename;
    }
    return save();
}


