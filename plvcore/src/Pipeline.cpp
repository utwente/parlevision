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
#include "Scheduler.h"

using namespace plv;

Pipeline::Pipeline() :
        m_stopRequested( false ),
        m_running( false ),
        m_scheduler( new Scheduler( this ) )
{
}

Pipeline::~Pipeline()
{
    assert( !m_running );
    delete m_scheduler;
}

int Pipeline::addElement( PipelineElement* child ) throw (IllegalArgumentException)
{
    if( !canAddElement( child) )
    {
        QString msg = "Tried to add PipelineElement " % child->getName()
                      % " to pipeline with an id which is already in use.";
        throw IllegalArgumentException( msg.toStdString() );
    }

    RefPtr<PipelineElement> element = child;
    int id = element->getId();
    if( id == - 1 )
    {
        id = getNewPipelineElementId();
        element->setId( id );
    }
    element->setPipeline(this);
    m_children.insert( id, element );
    emit( elementAdded(element) );
    return id;
}

bool Pipeline::canAddElement( PipelineElement* child )
{
    RefPtr<PipelineElement> element = child;
    int id = element->getId();
    if( id != -1 )
    {
        return( !m_children.contains( id ) );
    }
    return true;
}

//void Pipeline::removeElement( PipelineElement* child )
//{
//    int id = child->getId();
//    m_children.remove( id );
//}

int Pipeline::getNewPipelineElementId()
{
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
    if( m_children.contains( id ) )
    {
        return m_children.value( id );
    }
    return 0;
}

void Pipeline::removeElement( int id )
{
    if( m_children.contains( id ) )
    {
        // preserve the element so we can send it over the signal later
        RefPtr<PipelineElement> element = m_children.value( id );
        removeConnectionsForElement( element );
        m_children.remove( id );
        emit( elementRemoved(element) );
    }
}

void Pipeline::removeAllElements()
{
    foreach( RefPtr<PipelineElement> element, m_children )
    {
        removeConnectionsForElement( element );
        emit( elementRemoved(element) );
    }
    m_children.clear();
}

void Pipeline::removeConnectionsForElement( PipelineElement* element )
{
    RefPtr<PipelineElement> ple( element );

    const PipelineElement::InputPinMap& inputPins = ple->getInputPins();
    for( PipelineElement::InputPinMap::const_iterator itr = inputPins.begin()
        ; itr!=inputPins.end(); ++itr)
    {
        RefPtr<IInputPin> ipp = itr->second;
        if( ipp->isConnected() )
        {
            RefPtr<PinConnection> connection = ipp->getConnection();
            disconnect( connection.getPtr() );
        }
    }

    const PipelineElement::OutputPinMap& outputPins = ple->getOutputPins();
    for( PipelineElement::OutputPinMap::const_iterator itr = outputPins.begin()
        ; itr!=outputPins.end(); ++itr)
    {
        RefPtr<IOutputPin> opp = itr->second;

        const std::list< RefPtr<PinConnection> >& connections = opp->getConnections();
        for( std::list< RefPtr<PinConnection> >::const_iterator itr = connections.begin();
             itr!= connections.end(); ++itr )
        {
            disconnect( (*itr).getPtr() );
        }
    }
}

const Pipeline::PipelineElementMap& Pipeline::getChildren() const
{
    return m_children;
}

const Pipeline::PipelineConnectionsList& Pipeline::getConnections() const
{
    return m_connections;
}

void Pipeline::connectPins( IOutputPin* outputPin, IInputPin* inputPin)
{
    RefPtr<PinConnection> connection = new PinConnection(outputPin, inputPin);
    m_connections.push_back(connection);
    emit( connectionAdded(connection) );
}

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

void Pipeline::disconnect( PinConnection* connection )
{
    if( connection == 0 )
    {
        qWarning() << "Ignoring disconnect of null connection";
        return;
    }
    RefPtr<PinConnection> conn( connection );
    conn->disconnect();
    removeConnection( connection );
}

void Pipeline::removeConnection( PinConnection* connection )
{
    RefPtr<PinConnection> con = connection;

    if( con.isNull() )
    {
        qWarning() << "Ignoring removal of null connection";
        return;
    }

    for( PipelineConnectionsList::iterator itr = m_connections.begin();
            itr != m_connections.end(); ++itr )
    {
        RefPtr<PinConnection> con2 = *itr;
        if( con.getPtr() == con2.getPtr() )
        {
            m_connections.erase( itr );
            emit( connectionRemoved( con ) );
            break;
        }
    }
}

// TODO maybe refactor so we can report the processor which fails to initialize
bool Pipeline::init()
{
    QMapIterator<int, RefPtr<PipelineElement> > itr( m_children );

    // TODO make initialisation proper with exceptions and reporting of
    // processor which fails to initialize
    // and not return true all the time
    try
    {
        if( itr.hasNext() )
        {
            itr.next();
            RefPtr<PipelineElement> element = itr.value();
            if( !m_initialized.contains(element->getId()) )
            {
                element->init();
                m_initialized.insert(element->getId());
            }
        }
    }
    catch( PipelineException& e )
    {
        qDebug() << "FIXME: Exception caught in Pipeline::init()" << e.what();
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
}

void Pipeline::start()
{
    // TODO this is a hack to test
    init();

    // TODO formalize this procedure (starting of pipeline) more!
    QMapIterator<int, RefPtr<PipelineElement> > itr( m_children );
    while( itr.hasNext() )
    {
        itr.next();
        itr.value()->start();
    }

    // TODO make thread safe
    // and ensure that pipeline cannot be started twice
    m_stopRequested = false;
    QThread::start();
}

void Pipeline::stop()
{
    // TODO formalize this procedure (s of pipeline) more!
    QMapIterator<int, RefPtr<PipelineElement> > itr( m_children );
    while( itr.hasNext() )
    {
        itr.next();
        itr.value()->stop();
    }

    //TODO more elegant solution here with QTimer
    m_stopRequested = true;
    while( m_running )
    {
        usleep(100);
    }
}

void Pipeline::run()
{
    m_running = true;


    m_scheduler->setActiveThreadCount( 8 );

    while(!m_stopRequested)
    {
        m_scheduler->schedule();
    }

    m_running = false;

//        if( !schedule.isEmpty() )
//        {
//            QSet<ScheduleInfo*> scheduled;
//            QMapIterator< int, ScheduleInfo* > itr( schedule );
//            while( itr.hasNext() )
//            {
//                try
//                {
//                    itr.next();
//                    ScheduleInfo* si = itr.value();
//                    RefPtr<PipelineElement> element = si->getElement();
//                    int id = element->getId();

//                    QMutexLocker lock( &m_schedulerMutex );
//                    if( si->updateAndGetState() == ScheduleInfo::READY )
//                    {
//                        m_processing.insert( id );
//                        scheduled.insert( si );
////                        //int activeThreads = QThreadPool::globalInstance()->activeThreadCount();
////                        while( QThreadPool::globalInstance()->activeThreadCount() == QThreadPool::globalInstance()->maxThreadCount() )
////                        {
////                            qDebug() << "Max thread count reached ("
////                                    << QThreadPool::globalInstance()->activeThreadCount()
////                                    << "), sleeping.";
////                            usleep( 100 );
////                        }
//                        QFuture<ScheduleInfo*> info = QtConcurrent::run( this, &Pipeline::runProcessor, si );
//                    }
//                }
//                catch( PipelineException& pe )
//                {
//                    qDebug() << "Uncaught exception in PipelineElement::process()"
//                             << "of type PipelineException with message: " << pe.what();
//                }
//                catch( IllegalAccessException& iae )
//                {
//                    qDebug() << "Uncaught exception in PipelineElement::process()"
//                             << "of type IllegalAccessException with message: " << iae.what();
//                }
//                catch( ... )
//                {
//                    qDebug() << "Uncaught exception in PipelineElement::process()"
//                             << "of unknown type.";
//                }
//            }

//            foreach( ScheduleInfo* si, scheduled )
//            {
//                qDebug(" Executed processor %i, avg time elapsed: %d ms, maxQueueSize %i",
//                       si->getElement()->getId(), si->getAvgProcessingTime(), si->maxQueueSize() );
//            }
//        }
//        else
//        {
//            // nothing to do, sleep a little while
//            usleep( 0 );
//        }
//    }
}


