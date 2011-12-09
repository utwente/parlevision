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

#include "PipelineElement.h"

#include <QString>
#include <QStringList>
#include <QStringBuilder>
#include <QDebug>
#include <QMetaObject>
#include <QMetaProperty>
#include <QtConcurrentRun>
#include <algorithm>
#include <opencv/cv.h>

#include "Pipeline.h"
#include "RefCounted.h"
#include "Pin.h"


using namespace plv;

PipelineElement::PipelineElement() :
        m_id( -1 ),
        m_state( PLE_UNDEFINED ),
        m_avgProcessingTime(0),
        m_lastProcesingTime(0),
        m_propertyMutex( new QMutex( QMutex::Recursive ) ),
        m_errorType(PlvNoError),
        m_errorString("")
{
}

PipelineElement::~PipelineElement()
{
    delete m_propertyMutex;
}

int PipelineElement::addInputPin( IInputPin* pin )
{
    assert( pin != 0 );
    QMutexLocker lock( &m_pleMutex );

    int id = pin->getId();
    if( id == -1 )
    {
        id = getNextInputPinId();
        pin->setId(id);
        assert( m_inputPins.find(id) == m_inputPins.end() );
    }
    else
    {
        InputPinMap::iterator itr = m_inputPins.find(id);
        if( itr != m_inputPins.end() )
        {
            QString err = QString( tr( "Tried to add input pin %1 with id %2 "
                                       "which already exists. Ignoring."))
                    .arg( pin->getName() ).arg( pin->getId() );
            qWarning() << err;
            return -1;
        }
    }
    RefPtr<IInputPin> rpin( pin );
    m_inputPins.insert( std::make_pair( id, rpin ));
    emit inputPinAdded(pin);
    return id;
}

void PipelineElement::removeInputPin( int id )
{
    QMutexLocker lock( &m_pleMutex );
    InputPinMap::iterator itr = m_inputPins.find(id);
    if( itr != m_inputPins.end() )
    {
        QString err = QString(tr( "Tried to remove an input pin with invalid id %1. "
                                  "Ignoring.").arg(id));
        qWarning() << err;
    }
    emit inputPinRemoved(itr->first);
    m_inputPins.erase(itr);
}

int PipelineElement::addOutputPin( IOutputPin* pin )
{
    assert( pin != 0 );
    QMutexLocker lock( &m_pleMutex );

    int id = pin->getId();
    if( id == -1 )
    {
        id = getNextOutputPinId();
        assert( m_outputPins.find(id) == m_outputPins.end() );
        pin->setId(id);
    }
    else
    {
        OutputPinMap::iterator itr = m_outputPins.find(id);
        if( itr != m_outputPins.end() )
        {
            QString err = QString( tr( "Tried to add output pin %1 with id %2 "
                                       "which already exists. Ignoring."))
                    .arg( pin->getName() ).arg( pin->getId() );
            qWarning() << err;
            return -1;
        }
    }
    RefPtr<IOutputPin> rpin( pin );
    m_outputPins.insert( std::make_pair( id, rpin ));
    emit outputPinAdded(pin);
    return id;
}

void PipelineElement::removeOutputPin( int id )
{
    QMutexLocker lock( &m_pleMutex );
    OutputPinMap::iterator itr = m_outputPins.find(id);
    if( itr != m_outputPins.end() )
    {
        QString err = QString( tr( "Tried to remove an output pin with invalid id %1. "
                                   "Ignoring.").arg(id) );
        qWarning() << err;
    }
    emit outputPinRemoved(itr->first);
    m_outputPins.erase(itr);
}

int PipelineElement::getNextInputPinId() const
{
    int id=0;

    InputPinMap::const_iterator itr = m_inputPins.begin();
    for( ;itr != m_inputPins.end(); ++itr, ++id)
    {
        int cid = itr->first;
        if( cid > id )
            return id;
    }
    return id;
}

int PipelineElement::getNextOutputPinId() const
{
    int id=0;

    OutputPinMap::const_iterator itr = m_outputPins.begin();
    for( ;itr != m_outputPins.end(); ++itr, ++id)
    {
        int cid = itr->first;
        if( cid > id )
            return id;
    }
    return id;
}

IInputPin* PipelineElement::getInputPin( int id ) const
{
    QMutexLocker lock( &m_pleMutex );

    InputPinMap::const_iterator itr = m_inputPins.find( id );
    if( itr != m_inputPins.end() )
    {
        return itr->second.getPtr();
    }
    qDebug() << tr("Could not find pin with id %1 in PipelineElement::getInputPin").arg(id);
    return 0;
}

IOutputPin* PipelineElement::getOutputPin( int id ) const
{
    QMutexLocker lock( &m_pleMutex );

    OutputPinMap::const_iterator itr = m_outputPins.find( id );
    if( itr != m_outputPins.end() )
    {
        return itr->second.getPtr();
    }
    qDebug() << tr("Could not find pin with name %1 in PipelineElement::getOutputPin").arg(id);
    return 0;
}

PipelineElement::InputPinMap PipelineElement::getInputPins() const
{
    QMutexLocker lock( &m_pleMutex );

    // return a copy
    return m_inputPins;
}

PipelineElement::OutputPinMap PipelineElement::getOutputPins() const
{
    QMutexLocker lock( &m_pleMutex );

    // return a copy
    return m_outputPins;
}

QString PipelineElement::getName() const
{
    QString name = this->getClassProperty("name");
    if( name.isEmpty() )
    {
        const char* className = this->metaObject()->className();
        qWarning() << "No name registered for processor with class name " << className;
        return className;
    }
    return name;
}

QStringList PipelineElement::getConfigurablePropertyNames()
{
    const QMetaObject* metaObject = this->metaObject();
    QStringList list;
    for(int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i)
    {
        list.push_back(QString::fromLatin1(metaObject->property(i).name()));
    }
    return list;
}

QSet<PipelineElement*> PipelineElement::getConnectedElementsToOutputs() const
{
    QMutexLocker lock( &m_pleMutex );

    QSet<PipelineElement*> elements;
    for( OutputPinMap::const_iterator itr = m_outputPins.begin();
        itr != m_outputPins.end(); ++itr )
    {
        RefPtr<IOutputPin> out = itr->second;
        if( out->isConnected() )
        {
            std::list< RefPtr<PinConnection> > connections = out->getConnections();
            for( std::list< RefPtr<PinConnection> >::const_iterator connItr = connections.begin();
                 connItr != connections.end(); ++connItr )
            {
                RefPtr<PinConnection> connection = *connItr;
                RefPtr<const IInputPin> toPin = connection->toPin();
                PipelineElement* pinOwner = toPin->getOwner();
                elements.insert( pinOwner );
            }
        }
    }
    return elements;
}

QSet<PipelineElement*> PipelineElement::getConnectedElementsToInputs() const
{
    QMutexLocker lock( &m_pleMutex );

    QSet<PipelineElement*> elements;
    for( InputPinMap::const_iterator itr = m_inputPins.begin();
        itr != m_inputPins.end(); ++itr )
    {
        RefPtr<IInputPin> in = itr->second;
        if( in->isConnected() )
        {
            RefPtr<PinConnection> connection = in->getConnection();
            RefPtr<const IOutputPin> fromPin = connection->fromPin();
            PipelineElement* pinOwner = fromPin->getOwner();
            elements.insert( pinOwner );
        }
    }
    return elements;
}

bool PipelineElement::isEndNode() const
{
    QMutexLocker lock( &m_pleMutex );
    for( OutputPinMap::const_iterator itr = m_outputPins.begin();
         itr != m_outputPins.end(); ++itr )
    {
        RefPtr<IOutputPin> out = itr->second;
        if( out->isConnected() )
            return false;
    }
    return true;
}


bool PipelineElement::requiredPinsConnected() const
{
    QMutexLocker lock( &m_pleMutex );
    for( InputPinMap::const_iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        RefPtr<IInputPin> in = itr->second;
        if( in->isRequired() )
            if( !in->isConnected() )
                return false;
    }
    return true;
}

bool PipelineElement::dataAvailableOnInputPins( unsigned int& nextSerial )
{
    // synchronous processor
    if( m_hasSynchronousPin )
    {
        std::vector<unsigned int> serials;
        for( InputPinMap::const_iterator itr = m_inputPins.begin();
             itr != m_inputPins.end();
             ++itr )
        {
            IInputPin* in = itr->second.getPtr();

            // only check synchronous connections
            if( in->isConnected() )
            {
                if( in->isSynchronous() )
                {
                    if( in->hasData() )
                    {
                        unsigned int serial;
                        bool isNull;
                        in->peekNext(serial, isNull);
                        serials.push_back( serial );
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }

        // check if all serials are the same (which should be the case)
        bool valid = true;
        for( unsigned int i=0; i<serials.size() && valid; ++i)
        {
            valid = (serials[0] == serials[i]);
        }

        // the model should guarantee that
        // this should never happen obviously
        if( !valid )
        {
            setError( PlvPipelineRuntimeError, "Input corrupted" );
            return false;
        }

        // save the serial
        nextSerial = serials[0];

        return true;
    }
    // asynchronous processor, only has asynchronous pins
    else if( m_hasAsynchronousPin )
    {
        std::vector<unsigned int> serials;
        for( InputPinMap::const_iterator itr = m_inputPins.begin();
             itr != m_inputPins.end();
             ++itr )
        {
            IInputPin* in = itr->second.getPtr();

            // only check asynchronous connections
            if( in->isConnected() ) //&& in->isAsynchronous() )
            {
                if( in->hasData() )
                {
                    unsigned int serial;
                    bool isNull;
                    in->peekNext(serial, isNull);
                    serials.push_back(serial);
                }
            }
        }

        if( serials.size() > 0 )
        {
            std::sort( serials.begin(), serials.end() );
            // return smallest serial
            nextSerial = serials[0];
            return true;
        }
    }
    return false;
}

QStringList PipelineElement::getInputPinNames() const
{
    QMutexLocker lock( &m_pleMutex );

    QStringList names;
    for( InputPinMap::const_iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        names.push_back(itr->second->getName());
    }
    return names;
}

QStringList PipelineElement::getOutputPinNames() const
{
    QMutexLocker lock( &m_pleMutex );

    QStringList names;
    for( OutputPinMap::const_iterator itr = m_outputPins.begin();
         itr != m_outputPins.end(); ++itr )
    {
        names.push_back(itr->second->getName());
    }
    return names;
}

int PipelineElement::outputPinsConnectionCount() const
{
    QMutexLocker lock( &m_pleMutex );

    int connectionCount = 0;

    for( OutputPinMap::const_iterator itr = m_outputPins.begin();
         itr != m_outputPins.end(); ++itr )
    {
        RefPtr<IOutputPin> pin = itr->second;
        assert(pin.isNotNull());
        connectionCount += pin->connectionCount();
    }

    return connectionCount;
}

int PipelineElement::inputPinsConnectionCount() const
{
    QMutexLocker lock( &m_pleMutex );

    int connectionCount = 0;

    for( InputPinMap::const_iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        RefPtr<IInputPin> pin = itr->second;
        assert(pin.isNotNull());
        if( pin->isConnected() ) ++connectionCount;
    }

    return connectionCount;
}

int PipelineElement::pinsConnectionCount() const
{
    return inputPinsConnectionCount() + outputPinsConnectionCount();
}

int PipelineElement::maxInputQueueSize() const
{
    QMutexLocker lock( &m_pleMutex );

    int maxQueueSize = 0;

    for( PipelineElement::InputPinMap::const_iterator itr = m_inputPins.begin();
        itr!=m_inputPins.end(); ++itr )
    {
        int queueSize = 0;

        IInputPin* inputPin = itr->second;
        if( inputPin->isConnected() )
        {
            queueSize = inputPin->getConnection()->size();
        }

        if( queueSize > maxQueueSize ) maxQueueSize = queueSize;
    }
    return maxQueueSize;
}

int PipelineElement::maxOutputQueueSize() const
{
    QMutexLocker lock( &m_pleMutex );

    int maxQueueSize = 0;

    for( PipelineElement::OutputPinMap::const_iterator itr = m_outputPins.begin();
        itr!=m_outputPins.end(); ++itr )
    {
        IOutputPin* outputPin = itr->second;
        int queueSize = outputPin->maxDataOnConnection();
        if( queueSize > maxQueueSize )
            maxQueueSize = queueSize;
    }
    return maxQueueSize;
}


QString PipelineElement::getClassProperty(const char* name) const
{
    int idx = this->metaObject()->indexOfClassInfo(name);
    if(idx == -1)
        return "";

    return this->metaObject()->classInfo(idx).value();
}

void PipelineElement::setProperty(const char *name, const QVariant &value)
{
    QObject::setProperty(name, value);
    emit propertyChanged(QString(name));
}

void PipelineElement::setError( PlvErrorType type, const QString& msg )
{
    setState(PLE_ERROR);

    QMutexLocker lock( &m_pleMutex );
    m_errorString = msg;
    m_errorType = type;
}

PlvErrorType PipelineElement::getErrorType() const
{
    QMutexLocker lock( &m_pleMutex );
    return m_errorType;
}

QString PipelineElement::getErrorString() const
{
    QMutexLocker lock( &m_pleMutex );
    return m_errorString;
}

bool PipelineElement::visit( QList< PipelineElement* >& ordering,
                             QSet< PipelineElement* >& visited )
{
    // check if this node is already in partial ordering
    if( ordering.contains( this ))
        return true;

    // check for cycles
    if( visited.contains( this ))
    {
        // cycle detected
        ordering.append( this );
        return false;
    }
    visited.insert( this );

    // visit all incoming connections
    for( PipelineElement::InputPinMap::const_iterator itr = m_inputPins.begin();
        itr!=m_inputPins.end(); ++itr )
    {
        IInputPin* inputPin = itr->second;
        if( inputPin->isConnected() )
        {
            PipelineElement* node = inputPin->getConnection()->fromPin()->getOwner();
            // directly quit if cycle detected
            if( !node->visit( ordering, visited ) )
                return false;
        }
    }
    // go up in call stack
    visited.remove( this );
    ordering.append( this );
    return true;
}

void PipelineElement::startTimer()
{
    m_timer.start();
}

void PipelineElement::stopTimer()
{
    int elapsed = m_timer.elapsed();
    m_avgProcessingTime = m_avgProcessingTime > 0 ?
                          elapsed * 0.01f + m_avgProcessingTime * 0.99f : elapsed;
    m_lastProcesingTime = elapsed;
    emit onProcessingTimeUpdate((int)m_avgProcessingTime, elapsed);
}

PipelineElement::State PipelineElement::getState()
{
    QMutexLocker lock( &m_stateMutex );
    return m_state;
}

void PipelineElement::setState( State state )
{
    QMutexLocker lock( &m_stateMutex );
    m_state = state;
}

bool PipelineElement::__init()
{
    assert( getState() == PLE_UNDEFINED );

    QMutexLocker lock( &m_pleMutex );
    m_hasAsynchronousPin = false;
    m_hasSynchronousPin = false;
    for( InputPinMap::const_iterator itr = m_inputPins.begin();
         itr != m_inputPins.end();
         ++itr )
    {
        IInputPin* in = itr->second.getPtr();

        // only automatically check synchronous connections
        if( in->isConnected() )
        {
            if( in->isSynchronous() )
            {
                m_hasSynchronousPin = true;
            }
            else
            {
                m_hasAsynchronousPin = true;
            }
        }
    }
    lock.unlock();

    if( !this->init() )
    {
        setState(PLE_ERROR);
        return false;
    }
    setState(PLE_INITIALIZED);
    return true;
}

bool PipelineElement::__deinit() throw()
{
    assert( getState() == PLE_ERROR || getState() == PLE_INITIALIZED );
    if( !this->deinit() )
    {
        setState(PLE_ERROR);
        return false;
    }
    setState(PLE_UNDEFINED);

    QMutexLocker lock(&m_pleMutex);
    m_avgProcessingTime = 0;
    m_lastProcesingTime = 0;
    m_errorType = PlvNoError;
    m_errorString = "";
    return true;
}

bool PipelineElement::__start()
{
    assert(getState() == PLE_INITIALIZED);
    if(!this->start())
    {
        setState(PLE_ERROR);
        return false;
    }
    setState(PLE_STARTED);
    return true;
}

bool PipelineElement::__stop()
{
    assert(getState() >= PLE_STARTED);
    if( !this->stop() )
    {
        setState(PLE_ERROR);
        return false;
    }
    setState(PLE_INITIALIZED);
    return true;
}

bool PipelineElement::run( unsigned int serial )
{
    assert(getState() == PLE_DISPATCHED);

    //qDebug() << "PipelineElement::run for object " << this->getName()
    //         << " running in thread " << QThread::currentThread();

    setState(PLE_RUNNING);
    startTimer();
    try
    {
        // calls implementation (producer or procesor) specific private __process method
        this->__process( serial );
    }
    catch( RuntimeError& re )
    {
        qWarning() << "Uncaught exception in PipelineElement::process()"
                 << " of file " << re.getFileName()
                 << " on line " << re.getLineNumber()
                 << " of type PlvRuntimeException with message: " << re.what();
        stopTimer();
        setError(PlvPipelineRuntimeError, re.what());
        return false;
    }
    catch( plv::Exception& e )
    {
        qWarning() << "Uncaught exception in PipelineElement::process()"
                   << "of type PlvException with message: " << e.what();
        stopTimer();
        setError(PlvPipelineRuntimeError, e.what());
        return false;
    }
    catch( cv::Exception& e )
    {
        qWarning() << "Uncaught exception in PipelineElement::process()"
                 << "of type cv::Exception with message: " << e.what();
        stopTimer();
        setError(PlvPipelineRuntimeError, e.what());
        return false;
    }
    catch( std::runtime_error& err )
    {
        qWarning() << "Uncaught exception in PipelineElement::process()"
                 << "of type std::runtime_error with message: " << err.what();
        stopTimer();
        setError(PlvPipelineRuntimeError, err.what());
        return false;
    }
    catch( ... )
    {
        qWarning() << "Uncaught exception in PipelineElement::process()"
                   << "of unknown type.";
        stopTimer();
        setError(PlvPipelineRuntimeError, "Unknown exception caught");
        return false;
    }
    stopTimer();
    setState(PLE_STARTED);
    return true;
}
