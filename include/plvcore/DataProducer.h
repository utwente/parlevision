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

#ifndef PLV_DATA_PRODUCER_H
#define PLV_DATA_PRODUCER_H

#include "plvglobal.h"
#include "PipelineElement.h"
#include "IOutputPin.h"

#include <QHash>
#include <QSet>
#include <QStringList>

namespace plv
{
    /** forward declarations */
    class PipelineElement;

    /** typedefs to make code more readable */
    typedef QHash< int, RefPtr< IOutputPin > > OutputPinMap;

    /** interface for pipeline producers */
    class PLVCORE_EXPORT DataProducer : public PipelineElement
    {
        Q_OBJECT

    public:
        DataProducer();
        virtual ~DataProducer();

        /** initializes this producer. Should be called after pins have been
            added but before processing starts */
        void initOutputPins();

        /** returns true if this element has no outgoing connections */
        virtual bool isEndNode() const;

        virtual bool visit( QList<PipelineElement*>& ordering, QSet<PipelineElement*>& visited );

        /** @returns the set of pipeline elements which are connected to
          * this element via the output pins. */
        QSet<DataProducer*> getConnectedElementsToOutputs() const;

        /** Adds the output pin to this processing element.
            Returns id on success or -1 on failure. */
         int addOutputPin( IOutputPin* pin );

         void removeOutputPin( int id );

        /** @returns the ouput pin with that id, or null if none exists */
         IOutputPin* getOutputPin( int id ) const;

        /** @returns the summed total of all connections in all output pins */
         int outputPinsConnectionCount() const;

        /** returns a reference to the output pin map. */
         const OutputPinMap& getOutputPins() const;

        /** @returns a list of names of output pins added to this PipelineElement */
         QStringList getOutputPinNames() const;

        /** Returns the largest queue size of the connections connected
          * to the output pins. Returns 0 when there are no output pins with
          * a connection. */
         int maxOutputQueueSize() const;

        /** calls the pre method on all output pins. This method should be called
            before processing starts */
         void preOutput();

        /** calls the post method on all output pins. This method should be called
            adter processing is finished
        */
         void postOutput();

         virtual bool isDataConsumer() const { return false; }
         virtual bool isDataProducer() const { return true; }

         virtual void onOutputConnectionAdded(IOutputPin* pin, PinConnection* connection);
         virtual void onOutputConnectionRemoved(IOutputPin* pin, PinConnection* connection);

    protected:
        int getNextOutputPinId() const;

    private:
        mutable QMutex m_dpMutex;
        OutputPinMap m_outputPins;

    signals:
        void outputPinAdded( plv::IOutputPin* pin );
        void outputPinRemoved( int id );
    };
}

#endif // PLV_DATA_PRODUCER_H
