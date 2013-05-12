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

#ifndef PLVCORE_DATACONSUMER_H
#define PLVCORE_DATACONSUMER_H

#include "plvglobal.h"
#include <QHash>

#include "DataProducer.h"

namespace plv
{
    /** typedefs to make code more readable */
    typedef QHash< int, RefPtr< IInputPin > > InputPinMap;

    class ScoreBoard
    {
    public:
        ScoreBoard() : m_pincount(0), m_base(0)
        {
        }

        ~ScoreBoard()  {}

        void setPinCount(int count) { m_pincount = count; }

        inline int add( int pinId, unsigned int serial )
        {
            Q_UNUSED(pinId);
            assert( serial > m_base );

            int count = m_lookup.value(serial, 0);
            if( ++count == m_pincount )
            {
                m_lookup.remove(serial);
                m_base = serial;
                return true;
            }
            m_lookup.insert(serial, count);
            return false;
        }
    private:
        int m_pincount;
        unsigned int m_base;
        QHash<unsigned int, int> m_lookup;

    };

    /** interface for pipeline consumers */
    class PLVCORE_EXPORT DataConsumer : public DataProducer
    {
        Q_OBJECT

    public:
        DataConsumer();
        virtual ~DataConsumer();

        /** initializes this consumer. Should be called after pins have been
            added and before processing starts */
        void initInputPins();

        /** returns true if this element has no outgoing connections */
        virtual bool isEndNode() const;

        virtual bool visit( QList<PipelineElement*>& ordering, QSet<PipelineElement*>& visited );

        /** @returns true when input pins which are required by this processor to
          * be connected are connected. */
        bool requiredInputPinsConnected() const;

        /** @returns the set of pipeline elements which are connected to
          * this element via the input pins.
          */
        QSet<PipelineElement*> getConnectedElementsToInputs() const;

        /** @returns true if input pins which are connected and synchronous
          *  have data available */
        bool dataAvailableOnInputPins( unsigned int& nextSerial );

        /** Adds the input pin to this processing element.
            Returns id on success or -1 on failure. */
        int addInputPin( IInputPin* pin );

        /** Removes the input pin with id. If the input pins does not exist
            it is ignored but printed in the log */
        void removeInputPin( int id );

        /** Removes all input pins */
        void removeAllInputPins();

        /** @returns the input pin with that id, or null if none exists */
        IInputPin* getInputPin( int id ) const;

        /** @returns the summed total of all connections in all input pins */
        int inputPinsConnectionCount() const;

        /** @returns a list of names of input pins added to this PipelineElement */
        QStringList getInputPinNames() const;

        /** @returns a const reference to the input pin map
          * This function is thread safe.
          */
        const InputPinMap& getInputPins() const;

        /** @returns true if there is at least one Pin with a connection */
        bool hasPinConnections() const;

        /** Returns the largest queue size of the connections connected
          * to the input pins. Returns 0 when there are no input pins with
          * a connection. */
        int maxInputQueueSize() const;

        bool visitAllIncomingConnections( QList< PipelineElement* >& ordering,
                                          QSet< PipelineElement* >& visited );

        /** calls the pre method on all input pins. This method should be called
            before processing starts. nullDetected is true when a NULL data item
            has been detected on one of the connected synchronous input pins */
        void preInput( bool& nullDetected );

        /** calls the post method on all input pins. This method should be called
            adter processing is finished */
        void postInput();

        /** removes the first data item for each synchronous connected pin */
        void flushFirstOnSynchronousPins();

        virtual bool isDataConsumer() const { return true; }
        virtual bool isDataProducer() const { return true; }

        virtual void onInputConnectionSet(IInputPin* pin, PinConnection* connection);
        virtual void onInputConnectionRemoved(IInputPin* pin, PinConnection* connection);

        void newData(IInputPin* pin, unsigned int serial);

    protected:
        int getNextInputPinId() const;

        InputPinMap m_inputPins;

        /** True if this element has at least one connected asynchronous pin.
            Initialized in __init method.  */
        bool m_hasAsynchronousPin;

        /** true if this element has at least one connected synchronous pin.
            Initialized in __init method. */
        bool m_hasSynchronousPin;

        ScoreBoard m_scoreboard;

    signals:
        void inputPinAdded(plv::IInputPin* pin);
        void inputPinRemoved(int id);
    };
}

#endif // PLVCORE_DATACONSUMER_H
