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

namespace plv
{
    //class IInputPin;
    //class IOutputPin;
    class Pipeline;
    class PinConnection;

    class PLVCORE_EXPORT PipelineElement : public QObject, public RefCounted
    {
        Q_OBJECT

    private:
        /** copy constructor and assignment operator are disabled
            since we are a QObject */
        Q_DISABLE_COPY(PipelineElement)

    public:
        enum State {
            PLE_UNDEFINED,  /* undefined or non initialized */
            PLE_INITIALIZED,/* initialized(init method executed succesfully) */
            PLE_STARTED,    /* started (start method executed succesfully) */
            PLE_DISPATCHED, /* dispatched to be processed */
            PLE_RUNNING,    /* pipeline element is processing */
            PLE_DONE,       /* pipeline element is done processing */
            PLE_ERROR       /* an error occured during processing */
        };

    public:
        PipelineElement();
        virtual ~PipelineElement();

        void setId( int id );
        int getId() const;

        void setPipeline( Pipeline* pipeline);
        Pipeline* getPipeline() const;

        /** Get the name that describes this element, in human readable form. This name
            should be defined as a class property in the processors implementation, e.g
            Q_CLASSINFO("name", "Example"). If no name is defined, the unmangled C++
            class name is returned, e.g. plv::ExampleProcessor. */
        QString getName() const;

        /** Get a list of properties defined on this element */
        QStringList getConfigurablePropertyNames();

        QString getClassProperty(const char* name) const;

        /** sets the internal state to ERROR, error type to type and error string to msg */
        void setError( PlvErrorType type, const QString& msg );

        /** returns the error string set by the error which occurred last. The
            String is empty when no error has occurred */
        QString getErrorString() const;

        /** returns the error type which is set when an error occurs. Returns
            PlvNoError when no error has occured yet */
        PlvErrorType getErrorType() const;

        /** @returns the state of this element. This method is thread safe. */
        State getState();

        /** sets the state of this element. This method is thread safe. */
        void setState( State state );

        virtual bool __init();
        virtual bool __deinit() throw();
        virtual bool __start();
        virtual bool __stop();

        virtual bool __ready( unsigned int& serial )    = 0;
        virtual bool __process( unsigned int serial )   = 0;

        /** method which is called by the dispatcher to execute the __process() method
            returns false on error, true on succes */
        bool run( unsigned int serial );

        /** helper function for creating a partial ordering for cycle detection */
        virtual bool visit( QList<PipelineElement*>& ordering, QSet<PipelineElement*>& visited ) = 0;

        /** returns true if there are no output connections to other elements */
        virtual bool isEndNode() const = 0;

        virtual bool requiredPinsConnected() const = 0;

        virtual bool isDataConsumer() const = 0;
        virtual bool isDataProducer() const = 0;

        /** sets the serial number of the current process call. Not thread safe. */
        void setProcessingSerial( unsigned int serial );

        /** returs the serial number of the current process call. Not thread safe. */
        unsigned int getProcessingSerial() const;

        /** signals this element that it is ready to be dispatched */
//        virtual void signalReady() = 0;

//        virtual void onInputConnectionSet(IInputPin* pin, PinConnection* connection);
//        virtual void onInputConnectionRemoved(IInputPin* pin, PinConnection* connection);

//        virtual void onOutputConnectionAdded(IOutputPin* pin, PinConnection* connection);
//        virtual void onOutputConnectionRemoved(IOutputPin* pin, PinConnection* connection);

    public slots:
        /** Overridden from QObject::setProperty()
          * @emits propertyChanged(QString name) */
        void setProperty(const char* name, const QVariant &value);

   protected:
        /** Initialises the element so it is ready to receive process() calls.
            This allows for late initialization and an empty constructor.
            It can be called again after a call to deinit(). Use start for
            initialisation of resources which might be changed by the user
            before the pipeline is started, such as file locations.
            Note: when this method throws an exception or returns false,
            deinit() is called right after.
            Default implementation just returns true. Override in own class. */
        virtual bool init() { return true; }

        /** Deinitalises an element and frees resources. This method is not
            allowed to throw an exception. It is called on PipelineElement
            deinitalization and if init() throws an exception or returns false.
            Should deallocate all resources.
            Default implementation just returns true. Override in own class. */
        virtual bool deinit() throw() { return true; }

        /** Start() and stop() are called by the pipeline when the pipeline is
            started or stopped. This is useful for opening required input devices,
            starting threads etc. A start() call may occur again after every stop().
            Default implementation just returns true. Override in own class. */
        virtual bool start() { return true; }
        virtual bool stop() { return true; }

        void startTimer();
        void stopTimer();

        /** send a message to the pipeline to display to the user */
        inline void message(PlvMessageType type, const QString& msg)
        {
            emit sendMessageToPipeline(type, msg);
        }



        /** processor id */
        int m_id;

        /** The element state. See PipelineElementState enumeration for details */
        State m_state;

        mutable QMutex m_stateMutex;

        float m_avgProcessingTime;
        int m_lastProcesingTime;

        QTime m_timer;

        /** if an error has occured, this holds the type of error. */
        PlvErrorType m_errorType;

        /** if an error has occured, this string holds an error message. */
        QString m_errorString;

        /** serial number of current processing run. */
        unsigned int m_serial;

        /** if set contains a pointer to the current pipeline, NULL otherwise */
        Pipeline* m_pipeline;

        mutable QMutex m_pleMutex;

        /** mutex used for properties. Properties need a recursive mutex
          * sice the emit() they do to update their own value can return the
          * call to the set function resulting in a deadlock if we use a normal
          * mutex */
        mutable QMutex* m_propertyMutex;

    signals:
        void propertyChanged(QString);

        /** this signal should be emitted when an error occurs outside of the
            usual API functions which can signal the presense of an error by
            returning false. For instance, when a connection fails */
        void onError(PlvErrorType type, plv::PipelineElement* element);

        /** send a message to the pipeline to display to the user */
        void sendMessageToPipeline(PlvMessageType type, const QString& msg);

        /** average and last processing time of process call in milliseconds */
        void onProcessingTimeUpdate(int avg, int last);

        void onStateChange(PipelineElement::State state);
    };

//    void PipelineElement::onInputConnectionSet(IInputPin* pin, PinConnection* connection)
//    {
//        Q_UNUSED(pin);
//        Q_UNUSED(connection);
//    }

//    void PipelineElement::onInputConnectionRemoved(IInputPin* pin, PinConnection* connection)
//    {
//        Q_UNUSED(pin);
//        Q_UNUSED(connection);
//    }

//    void PipelineElement::onOutputConnectionAdded(IOutputPin* pin, PinConnection* connection)
//    {
//        Q_UNUSED(pin);
//        Q_UNUSED(connection);
//    }

//    void PipelineElement::onOutputConnectionRemoved(IOutputPin* pin, PinConnection* connection)
//    {
//        Q_UNUSED(pin);
//        Q_UNUSED(connection);
//    }
}
Q_DECLARE_METATYPE(plv::PipelineElement::State)

#endif // PIPELINEELEMENT_H
