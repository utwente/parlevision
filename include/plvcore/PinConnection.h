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

#ifndef PINCONNECTION_H
#define PINCONNECTION_H

#include <queue>
#include <QMutexLocker>

#include "RefPtr.h"
#include "RefCounted.h"
#include "PlvExceptions.h"

namespace plv
{
    class Data;
    class IOutputPin;
    class IInputPin;

    class PLVCORE_EXPORT PinConnection : public RefCounted
    {
        enum ConnectionType {
            LOSSLESS,
            LOSSY_FIFO,
            LOSSY_LIFO
        };
        friend class Pipeline;
    public:
        class PLVCORE_EXPORT IllegalConnectionException : public PlvException
        {
        public:
            IllegalConnectionException(const QString& str) : PlvException(str) {}
            virtual ~IllegalConnectionException() throw() {}
        };

        class PLVCORE_EXPORT IncompatibleTypeException : public PlvException
        {
        public:
            IncompatibleTypeException(const QString& str) : PlvException(str) {}
            virtual ~IncompatibleTypeException() throw() {}
        };

        class PLVCORE_EXPORT DuplicateConnectionException : public PlvException
        {
        public:
            DuplicateConnectionException(const QString& str) : PlvException(str) {}
            virtual ~DuplicateConnectionException() throw() {}
        };

        PinConnection( IOutputPin* producer, IInputPin* consumer )
                throw ( IllegalConnectionException,
                        IncompatibleTypeException,
                        DuplicateConnectionException );
        ~PinConnection();

        bool hasData();
        int size();
        inline ConnectionType getType();
        RefPtr<Data> get() throw ( PlvRuntimeException );
        RefPtr<Data> peek() const throw ( PlvRuntimeException );
        void put( Data* data );

        bool fastforward( unsigned int target );

        /** Throw away all data in this connection. */
        void flush();

        /** @return the output pin which provides the input to this connection */
        const IOutputPin* fromPin() const;

        /** @return the input pin which provides output from connection */
        const IInputPin*  toPin() const;

    protected:
        void connect() throw ( IllegalConnectionException,
                               IncompatibleTypeException,
                               DuplicateConnectionException );

        /** Disconnect this connection from its pins
          * this makes it a "floating" connection that can safely be removed.
          * this does not notify the pipeline or remove itself from it.
          * you probably won't call this directly,
          * use Pipeline::disconnect(connection) instead
          */
        void disconnect();

        IOutputPin* m_producer;
        IInputPin*  m_consumer;
        std::queue< RefPtr<Data> > m_queue;
        ConnectionType m_type;
        mutable QMutex m_connectionMutex;
    };
}

namespace plv
{
    PinConnection::ConnectionType PinConnection::getType() { return m_type; }
}

#endif // PINCONNECTION_H
