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

#include "Pipeline.h"
#include "RefCounted.h"
#include "Pin.h"

using namespace plv;

//QStringList PipelineElement::s_types;
//std::map<QString,QString> PipelineElement::s_names;

PipelineElement::PipelineElement() :
        m_id( -1 ),
        m_propertyMutex( new QMutex( QMutex::Recursive ) )
{
}

PipelineElement::~PipelineElement()
{
    delete m_propertyMutex;
}

void PipelineElement::addInputPin( IInputPin* pin ) throw (IllegalArgumentException)
{
    QMutexLocker lock( &m_pleMutex );

    InputPinMap::iterator itr = m_inputPins.find( pin->getName() );
    if( itr != m_inputPins.end() )
    {
        QString err = "Tried to add input pin with name which already exists";
        err += pin->getName();
        throw IllegalArgumentException( err );
    }
    RefPtr<IInputPin> rpin( pin );
    m_inputPins.insert( std::make_pair( pin->getName(), rpin ));
}

void PipelineElement::addOutputPin( IOutputPin* pin ) throw (IllegalArgumentException)
{
    QMutexLocker lock( &m_pleMutex );

    OutputPinMap::iterator itr = m_outputPins.find( pin->getName() );
    if( itr != m_outputPins.end() )
    {
        QString err = "Tried to add output pin with name which already exists";
        err += pin->getName();
        throw IllegalArgumentException( err );
    }
    RefPtr<IOutputPin> rpin( pin );
    m_outputPins.insert( std::make_pair( pin->getName(), rpin ));
}

IInputPin* PipelineElement::getInputPin( const QString& name ) const
{
    QMutexLocker lock( &m_pleMutex );

    InputPinMap::const_iterator itr = m_inputPins.find( name );
    if( itr != m_inputPins.end() )
    {
        return itr->second.getPtr();
    }
    qDebug() << "could not find pin named " << name;
    return 0;
}

IOutputPin* PipelineElement::getOutputPin( const QString& name ) const
{
    QMutexLocker lock( &m_pleMutex );

    OutputPinMap::const_iterator itr = m_outputPins.find( name );
    if( itr != m_outputPins.end() )
    {
        return itr->second.getPtr();
    }
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

bool PipelineElement::dataAvailableOnInputPins() const
{
    QMutexLocker lock( &m_pleMutex );

    for( InputPinMap::const_iterator itr = m_inputPins.begin();
         itr != m_inputPins.end();
         ++itr )
    {
        IInputPin* in = itr->second.getPtr();

        // only automatically check synchronous connections
        if( in->isConnected() &&
            in->isSynchronous() )
        {
            // check for data
            if( !in->hasData() )
            {
                return false;
            }
        }
    }
    return true;
}

QStringList PipelineElement::getInputPinNames() const
{
    QMutexLocker lock( &m_pleMutex );

    QStringList names;
    for( InputPinMap::const_iterator itr = m_inputPins.begin();
         itr != m_inputPins.end(); ++itr )
    {
        names.push_back(itr->first);
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
        names.push_back(itr->first);
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

//QStringList PipelineElement::types()
//{
//    return PipelineElement::s_types;
//}

//int PipelineElement::registerType(QString typeName, QString humanName)
//{
//    qDebug() << "Registering PipelineElement " << typeName
//             << " as " << "'" << humanName << "'";

//    PipelineElement::s_types.push_back( typeName );
//    PipelineElement::s_names[typeName] = humanName;
//    return 0;
//}

//QString PipelineElement::nameForType(QString typeName)
//{
//    return PipelineElement::s_names[typeName];
//}

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
    emit(propertyChanged(QString(name)));
}

void PipelineElement::error( ErrorType type, QString msg )
{
    m_errorString = msg;
    emit( errorOccured( type, msg ) );
}
