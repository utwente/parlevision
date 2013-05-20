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

using namespace plv;

PipelineElement::PipelineElement() :
        m_id( -1 ),
        m_state( PLE_UNDEFINED ),
        m_avgProcessingTime(0),
        m_lastProcesingTime(0),
        m_errorType(PlvNoError),
        m_errorString(""),
        m_serial(0),
        m_propertyMutex( new QMutex( QMutex::Recursive ) )
{
}

PipelineElement::~PipelineElement()
{
    delete m_propertyMutex;
}

void PipelineElement::setId( int id )
{
    assert(m_id == -1);
    m_id = id;
}

int PipelineElement::getId() const
{
    return m_id;
}

void PipelineElement::setPipeline(Pipeline* pipeline)
{
    assert( pipeline != 0 );
    m_pipeline = pipeline;
}

Pipeline* PipelineElement::getPipeline() const
{
    return m_pipeline;
}

/** sets the serial number of the current process call. */
void PipelineElement::setProcessingSerial( unsigned int serial )
{
    m_serial = serial;
}

/** returs the serial number of the current process call */
unsigned int PipelineElement::getProcessingSerial() const
{
    return m_serial;
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

//int PipelineElement::pinsConnectionCount() const
//{
//    return 0;
//    //return inputPinsConnectionCount() + outputPinsConnectionCount();
//}

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
    m_errorType   = type;
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
    m_serial = 0;
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
    setState(PLE_DONE);
    return true;
}
