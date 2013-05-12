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

#include "PipelineProcessor.h"
#include "Pin.h"
#include "Pipeline.h"

using namespace plv;

PipelineProcessor::PipelineProcessor()
{
}

PipelineProcessor::~PipelineProcessor()
{
}

bool PipelineProcessor::requiredPinsConnected() const
{
    return this->requiredInputPinsConnected();
}

//void PipelineProcessor::acceptData(QVariant &data)
//{
//    if( m_pipeline != 0 )
//    {
//        m_pipeline->pipelineProcessorReady(this);
//    }
//}

bool PipelineProcessor::__init()
{
    this->initInputPins();
    return PipelineElement::__init();
}

bool PipelineProcessor::__ready( unsigned int& serial )
{
    if( getState() >= PLE_DISPATCHED )
        return false;

    // see if data is available and the processor is ready for processing
    unsigned int nextSerial;
    bool retval = dataAvailableOnInputPins(nextSerial) && nextSerial == serial;
    return retval;
}

bool PipelineProcessor::__process( unsigned int serial )
{
    assert( requiredPinsConnected() );
    assert( getState() == PLE_RUNNING );

    QMutexLocker lock( &m_pleMutex );
    //assert( serial > getProcessingSerial() || serial == 0 );

    // TODO remove debug
    unsigned int processingSerial = getProcessingSerial();
    QString msg = QString("PipelineProcessor:serial: %1 processing serial: %2").arg(serial).arg(processingSerial);
    qDebug() << msg;

    //assert( serial > getProcessingSerial() || serial == 0 );
    if(!(serial > processingSerial || serial == 0)) {
        qDebug() << "HERE!!!!!! ======== <<<<<<<<<<";
    }

    // set the serial number
    setProcessingSerial( serial );

    bool nullDetected = false;

    // call pre on input pins and look for null data items
    this->preInput( nullDetected );

    // if one data item is a null
    // we throw away all data from all synchronous pins
    if( nullDetected )
    {
        this->flushFirstOnSynchronousPins();

        // call post on output pins to propagate NULL down pipeline
        this->preOutput();
        this->postOutput();
        return true;
    }

    // call pre on output pins
    this->preOutput();

    // do the actual processing
    lock.unlock();

    // we do not want properties to change in the middle of an operation
    QMutexLocker lock2( m_propertyMutex );
    bool retval = this->process();
    lock2.unlock();
    lock.relock();

    // call post on input pins
    this->postInput();

    // call post on output pins
    this->postOutput();
    lock.unlock();

    if(!retval && getState() != PLE_ERROR)
    {
        QString msg = tr("Method process() on PipelineProcessor %1 returned false "
                         "but error state was not set.").arg(this->getName());
        qWarning() << msg;
    }

    return retval;
}


