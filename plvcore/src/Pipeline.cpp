#include "Pipeline.h"

#include <QDebug>
#include <QStringBuilder>
#include <list>

#include "PipelineElement.h"
#include "PipelineProcessor.h"
#include "PipelineProducer.h"
#include "Pin.h"
#include "PinConnection.h"

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

void Pipeline::removeElement( PipelineElement* child )
{
    int id = child->getId();
    m_children.remove( id );
}

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
    // TODO is this correct
    m_children.clear();

/*
    // build list of ids as removing here would invalidate the map
    std::list<int> ids;
    for( PipelineElementMap::iterator itr = m_children.begin(); itr!=m_children.end(); ++itr )
    {
        ids.push_back(itr->first);
    }

    // remove them one by one
    for(std::list<int>::iterator itr = ids.begin(); itr != ids.end(); ++itr)
    {
        remove(*itr);
    }
*/
}

void Pipeline::removeConnectionsForElement( PipelineElement* element )
{
    RefPtr<PipelineElement> ple( element );

    std::list< RefPtr<IInputPin> >* inputPins = ple->getInputPins();
    std::list< RefPtr<IOutputPin> >* outputPins = ple->getOutputPins();

    for( std::list< RefPtr<IInputPin> >::iterator itr = inputPins->begin()
        ; itr!=inputPins->end(); ++itr)
    {
        RefPtr<IInputPin> ipp = *itr;
        if( ipp->isConnected() )
        {
            RefPtr<PinConnection> connection = ipp->getConnection();
            disconnect( connection );
        }
    }

    for( std::list< RefPtr<IOutputPin> >::iterator itr = outputPins->begin()
        ; itr!=outputPins->end(); ++itr)
    {
        RefPtr<IOutputPin> opp = *itr;

        if( opp->isConnected() )
        {
            const std::list< RefPtr<PinConnection> >& connections = opp->getConnections();
            for( std::list< RefPtr<PinConnection> >::const_iterator itr = connections.begin();
                 itr!= connections.end(); ++itr )
            {
                disconnect( *itr );
            }
        }
    }


    // TODO better to use automatic removal than this
    delete inputPins;
    delete outputPins;
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

    PipelineConnectionsList toRemove;
    for( PipelineConnectionsList::iterator itr = m_connections.begin();
            itr != m_connections.end(); ++itr )
    {
        RefPtr<PinConnection> connection = *itr;
        toRemove.push_back(connection);
    }

    for( PipelineConnectionsList::iterator itr = toRemove.begin();
            itr != toRemove.end(); ++itr )
    {
        RefPtr<PinConnection> connection = *itr;
        assert(connection.isNotNull());
        removeConnection(connection);
    }
}

void Pipeline::disconnect( PinConnection* connection )
{
    if(connection == 0)
    {
        qWarning() << "Ignoring disconnect of null connection";
        return;
    }
    RefPtr<PinConnection> conn( connection );
    conn->disconnect();
    removeConnection(connection);
}

void Pipeline::removeConnection( PinConnection* connection )
{
    if(connection == 0)
    {
        qWarning() << "Ignoring removal of null connection";
        return;
    }
    RefPtr<PinConnection> con1( connection );

    for( PipelineConnectionsList::iterator itr = m_connections.begin();
            itr != m_connections.end(); ++itr )
    {
        RefPtr<PinConnection> con2 = *itr;
        if( con1.getPtr() == con2.getPtr() )
        {
            RefPtr<PinConnection> conn = *itr;
            m_connections.erase( itr );
            emit(connectionRemoved(conn));
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
    if( itr.hasNext() )
    {
        itr.next();
        RefPtr<PipelineElement> element = itr.value();
        if(!m_initialized.contains(element->getId()))
        {
            bool state = element->init();
            if( !state )
                return false;
            else
                m_initialized.insert(element->getId());
        }
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

    // TODO make thread safe
    // and ensure that pipeline cannot be started twice
    m_stopRequested = false;
    QThread::start();
}

void Pipeline::stop()
{
    m_stopRequested = true;
    //TODO more elegant solution here with QTimer
    while( m_running )
    {
        usleep(100);
    }
}

void Pipeline::run()
{
    m_running = true;

    while(!m_stopRequested)
    {
        QMapIterator< int, RefPtr<PipelineElement> > itr( m_children );
        while( itr.hasNext() )
        {
            itr.next();

            RefPtr<PipelineElement> element = itr.value();
            try
            {
                element->__process();
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
//        usleep(100);
        usleep(1000000/40);
    }
    m_running = false;
}
