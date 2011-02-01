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

#ifndef PIPELINEELEMENT_H
#define PIPELINEELEMENT_H

#include <map>
#include <stdexcept>
#include <assert.h>

#include <QString>
#include <QObject>
#include <QMetaType>
#include <QTime>

#include "plvglobal.h"
#include "RefPtr.h"
#include "PlvExceptions.h"
#include "PipelineElementFactory.h"

namespace plv
{
    class IInputPin;
    class IOutputPin;
    class Pipeline;

    class PLVCORE_EXPORT PipelineElement : public QObject, public RefCounted
    {
        Q_OBJECT

    private:
        /** copy constructor and assignment operator are disabled
            since we are a QObject */
        Q_DISABLE_COPY(PipelineElement)

    public:
        /** typedefs to make code more readable */
        typedef std::map< QString, RefPtr< IInputPin > > InputPinMap;
        typedef std::map< QString, RefPtr< IOutputPin > > OutputPinMap;

        enum State {
            UNDEFINED,  /* undefined or non initialized */
            READY,      /* ready for processing */
            DISPATCHED, /* dispatched to be processed */
            RUNNING,    /* pipeline element is processing */
            DONE,       /* done processing */
            ERROR       /* an error occured during processing */
        };

    protected:
        /** processor id */
        int m_id;

        /** The element state. See PipelineElementState enumeration for details */
        State m_state;

        /** map which contains the input pins identified and indexed by their name */
        InputPinMap m_inputPins;

        /** map which contains the output pins identified and indexed by their name */
        OutputPinMap m_outputPins;

        int m_avgProcessingTime;
        int m_lastProcesingTime;

        QTime m_timer;

        mutable QMutex m_pleMutex;
        mutable QMutex m_stateMutex;

        /** mutex used for properties. Properties need a recursive mutex
          * sice the emit() they do to update their own value can return the
          * call to the set function resulting in a deadlock if we use a normal
          * mutex */
        mutable QMutex* m_propertyMutex;

        /** if an error has occured, this string holds an error message.
            Errors are also emitted using signal errorOccurred */
        QString m_errorString;

    public:
        friend class Pipeline;
        friend class RunItem;

        PipelineElement();
        virtual ~PipelineElement();

        /** Initialise the element so it is ready to receive process() calls.
          * This allows for late initialization and an empty constructor.
          * It can be called again after a call to deinit(). Use start for
          * initialisation of resources which might be changed by the user
          * before the pipeline is started, such as file locations.
          * Note: when this method throws an exception,
          * deinit() is called right after. */
        virtual void init() = 0;

        /** Deinitalizes an element and frees resources. This method is not
          * allowed to throw an exception. It is called on PipelineElement
          * deinitalization and if init() throws an exception. Should deallocate
          * all resources. */
        virtual void deinit() throw() = 0;

        /** Start() and stop() are called when the pipeline is started and stopped
          * by the user. This is useful for opening required input devices,
          * starting threads etc. A start() call may occur again after every stop(). */
        virtual void start() = 0;
        virtual void stop() = 0;

        /** returns true if this element has no outgoing connections */
        bool isEndNode() const ;

        /** @returns true when input pins which are required by this processor to
          * be connected are connected. */
        bool requiredPinsConnected() const;

        /** @returns the set of pipeline elements which are connected to
          * this element via the output pins.
          */
        QSet<PipelineElement*> getConnectedElementsToOutputs() const;

        /** @returns the set of pipeline elements which are connected to
          * this element via the input pins.
          */
        QSet<PipelineElement*> getConnectedElementsToInputs() const;

        /** @returns true if input pins which are connected and synchronous
          *  have data available */
        bool dataAvailableOnInputPins( unsigned int& nextSerial );

        /** Get the name that describes this element, in human readable form. This name
            should be defined as a class property in the processors implementation, e.g
            Q_CLASSINFO("name", "Example"). If no name is defined, the unmangled C++
            class name is returned, e.g. plv::ExampleProcessor. */
        QString getName() const;

        /** Adds the input pin to this processing element.
          * @throws IllegalArgumentException if an input pin with
          * the same name already exists
          */
        void addInputPin( IInputPin* pin ) throw (IllegalArgumentException);

        /** Adds the output pin to this processing element.
          * @throws IllegalArgumentException if an input pin with
          * the same name already exists
          */
        void addOutputPin( IOutputPin* pin ) throw (IllegalArgumentException);

        /** @returns the input pin with that name, or null if none exists */
        IInputPin* getInputPin( const QString& name ) const;

        /** @returns the ouput pin with that name, or null if none exists */
        IOutputPin* getOutputPin( const QString& name ) const;

        inline void setId( int id ) { assert(m_id == -1); m_id = id; }
        inline int getId() const { return m_id; }

        /** Get a list of properties defined on this element */
        QStringList getConfigurablePropertyNames();

        /** @returns the summed total of all connections in all input pins */
        int inputPinsConnectionCount() const;

        /** @returns the summed total of all connections in all output pins */
        int outputPinsConnectionCount() const;

        /** @returns the summed total of all connections in all input and output pins */
        int pinsConnectionCount() const;

        /** @returns a list of names of input pins added to this PipelineElement */
        QStringList getInputPinNames() const;

        /** returns a copy of the contents of the input pin map
          * This function is thread safe.
          */
        InputPinMap getInputPins() const;

        /** returns a copy of the contents of the output pin map.
          * This function is thread safe.
          */
        OutputPinMap getOutputPins() const;

        /** @returns a list of names of output pins added to this PipelineElement */
        QStringList getOutputPinNames() const;

        /** returns true if there is at least one Pin with a connection */
        bool hasPinConnections() const;

        /** Returns the largest queue size of the connections connected
          * to the input pins. Returns 0 when there are no input pins with
          * a connection. */
        int maxInputQueueSize() const;

        QString getClassProperty(const char* name) const;

        /** Overridden from QObject::setProperty()
          * @emits propertyChanged(QString name) */
        void setProperty(const char *name, const QVariant &value);

        /** sets the internal error string to msg and emits errorOccured signal */
        void error( PipelineErrorType type, const QString& msg );

        /** returns the error string set by the error which occurred last. The
            String is empty when no error has occurred */
        QString getErrorString() const;

        /** @returns the state of this element. This method is thread safe. */
        State getState();

        /** sets the state of this element. This method is thread safe. */
        void setState( State state );

        /** sets the serial number of the current process call. Not thread safe. */
        inline void setProcessingSerial( unsigned int serial ) { m_serial = serial; }

        /** returs the serial number of the current process call. Not thread safe. */
        inline unsigned int getProcessingSerial() const { return m_serial; }

    signals:
        void propertyChanged(QString);
        void errorOccurred(PipelineErrorType type, PipelineElement* element);

    protected:
        /** serial number of current processing run. */
        unsigned int m_serial;

        /** helper function for creating a partial ordering for cycle detection */
        bool visit( QList<PipelineElement*>& ordering, QSet<PipelineElement*>& visited );

        /** method which is called by the dispatcher to execute the __process() method */
        void run( unsigned int serial );

        virtual bool __ready( unsigned int& serial )    = 0;
        virtual void __process( unsigned int serial )   = 0;

        void startTimer();
        void stopTimer();
    };
}

/** template helper function to register PipelineElements */
template<typename PET>
int plvRegisterPipelineElement()
{
    plv::PipelineElementConstructorHelper<PET>* plec = new plv::PipelineElementConstructorHelper<PET>();
    int id = plv::PipelineElementFactory::registerElement( plec );
    return id;
}

#endif // PIPELINEELEMENT_H
