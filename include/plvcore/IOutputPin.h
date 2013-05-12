#ifndef IOUTPUTPIN_H
#define IOUTPUTPIN_H

#include "Pin.h"
#include "PinConnection.h"

namespace plv
{
    class DataProducer;
    class PinConnection;

    class PLVCORE_EXPORT IOutputPin : public Pin
    {
        Q_OBJECT

    public:
        IOutputPin( const QString& name, DataProducer* producer );
        virtual ~IOutputPin();

        /** Adds a connection to the set of connections this pin outputs to
          * @ensure this->isConnected();
          */
        void addConnection( PinConnection* connection );

        /** Removes the connection from this pin.
          * Assumes it has been disconnect()'ed
          */
        void removeConnection( PinConnection* connection );
        void removeConnections();

        std::list< RefPtr<PinConnection> > getConnections();
        int connectionCount() const;

        int maxDataOnConnection() const;

        /** @returns true when pin is connected */
        inline bool isConnected() const { return !m_connections.empty(); }

        /** this method is called before each call to process() in the owner */
        void pre();

        /** this method is called after each call to process() in the owner */
        void post();

        void putVariant( unsigned int serial, const QVariant& data );

        /** returns wheter put() has been called since last pre() */
        inline bool isCalled() const { return m_called; }

        /** checks if this pin is compatible with input pin */
        virtual bool acceptsConnectionWith( const IInputPin* pin, QString& errStr ) const = 0;

        /** methods not implemented from abstract Pin class */
        virtual int getTypeId() const = 0;
        virtual QString getTypeName() const = 0;

    protected:
        DataProducer* m_producer;

        std::list< RefPtr<PinConnection > > m_connections;

        /** true when put() has been called */
        bool m_called;
    };
}

#endif
