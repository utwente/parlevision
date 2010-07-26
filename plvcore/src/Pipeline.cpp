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
        m_stopRequested( false ),
        m_running( false )
{
}

Pipeline::~Pipeline()
{
    assert( !m_running );
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
        throw (IncompatibleTypeException, DuplicateConnectionException)
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

void Pipeline::schedule( QMap< int, ScheduleInfo* >& schedule )
{
    QMapIterator< int, RefPtr<PipelineElement> > itr( m_children );
    while( itr.hasNext() )
    {
        itr.next();
        RefPtr<PipelineElement> element = itr.value();

        ScheduleInfo* si;
        QMap< int, ScheduleInfo* >::iterator itr2 = schedule.find( element->getId() );
        if( itr2 != schedule.end() )
        {
            si = itr2.value();
        }
        else
        {
            si = new ScheduleInfo( element.getPtr() );
            schedule.insert( element->getId(), si );
        }
    }
}

void Pipeline::runProcessor( ScheduleInfo* info )
{
    int id = info->getElement()->getId();

    QTime t;
    t.start();
    info->getElement()->__process();
    int elapsed = t.elapsed();
    int avgTime = info->getAvgProcessingTime();
    int nAvgTime = avgTime > 0 ? (int) ( elapsed * 0.1 ) + ( avgTime * 0.9 ) : elapsed;
    info->setAvgProcessingTime( nAvgTime );

    //qDebug(" Executed processor %i, time elapsed: %d ms", id , elapsed );

    QMutexLocker lock( &m_schedulerMutex );
    m_processing.remove( id );
}

void Pipeline::run()
{
    m_running = true;

    QMap< int, ScheduleInfo* > schedule;

    while(!m_stopRequested)
    {
        this->schedule( schedule );

        if( !schedule.isEmpty() )
        {
            QMapIterator< int, ScheduleInfo* > itr( schedule );
            while( itr.hasNext() )
            {
                try
                {
                    itr.next();
                    ScheduleInfo* si = itr.value();
                    RefPtr<PipelineElement> element = si->getElement();
                    int id = element->getId();
                    //qDebug() << "Scheduling processor " << element->getName() << " with count " << itr.key();

                    QMutexLocker lock( &m_schedulerMutex );
                    if( !m_processing.contains( id ) )
                    {
                        if( element->isReadyForProcessing() )
                        {
                            m_processing.insert( id );
                            QtConcurrent::run( this, &Pipeline::runProcessor, si );
                        }
                    }
                }
                catch( PipelineException& pe )
                {
                    qDebug() << "Uncaught exception in PipelineElement::process()"
                             << "of type PipelineException with message: " << pe.what();
                }
                catch( IllegalAccessException& iae )
                {
                    qDebug() << "Uncaught exception in PipelineElement::process()"
                             << "of type IllegalAccessException with message: " << iae.what();
                }
                catch( ... )
                {
                    qDebug() << "Uncaught exception in PipelineElement::process()"
                             << "of unknown type.";
                }
            }
        }
        else
        {
            // nothing to do, sleep a little while
            usleep( 0 );
        }
    }
    m_running = false;
}
