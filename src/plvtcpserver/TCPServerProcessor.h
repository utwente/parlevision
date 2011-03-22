/**
  * Copyright (C)2011 by Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvtcpserver plugin of ParleVision.
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

#ifndef TCPSERVERPROCESSOR_H
#define TCPSERVERPROCESSOR_H

#include <QObject>
#include <plvcore/PipelineProcessor.h>
#include <plvcore/RefPtr.h>
#include <plvcore/Pin.h>
#include <opencv/cv.h>

#include "Server.h"

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
    class IInputPin;
    class IOutputPin;
    class PinConnection;
}

class TCPServerProcessor : public plv::PipelineProcessor
{
    Q_OBJECT
    Q_DISABLE_COPY( TCPServerProcessor )

    Q_CLASSINFO("author", "Richard Loos")
    Q_CLASSINFO("name", "TCP Server")
    Q_CLASSINFO("description", "TCP server")

    Q_PROPERTY( int port READ getPort WRITE setPort NOTIFY portChanged  )
    Q_PROPERTY( bool convertCvMatDataToQImage READ getConvertCvMatDataToQImage WRITE setConvertCvMatDataToQImage NOTIFY convertCvMatDataToQImageChanged )
    Q_PROPERTY( bool lossless READ getLossless WRITE setLossless NOTIFY losslessChanged )
    Q_PROPERTY( int maxFramesInQueue  READ getMaxFramesInQueue  WRITE setMaxFramesInQueue NOTIFY maxFramesInQueueChanged )
    Q_PROPERTY( int maxFramesInFlight READ getMaxFramesInFlight WRITE setMaxFramesInFlight NOTIFY maxFramesInFlightChanged )

    /** required standard method declaration for plv::PipelineProcessor */
    PLV_PIPELINE_PROCESSOR

public:
    TCPServerProcessor();
    virtual ~TCPServerProcessor();

    virtual bool init();
    virtual bool deinit() throw();
    virtual bool start();
    virtual bool stop();

    /** propery methods */
    int getPort() const;
    bool getConvertCvMatDataToQImage() const;
    int getMaxFramesInFlight() const;
    int getMaxFramesInQueue() const;
    bool getLossless() const;

    virtual bool isReadyForProcessing() const;

    virtual void inputConnectionSet(plv::IInputPin* pin, plv::PinConnection* connection);
    virtual void inputConnectionRemoved(plv::IInputPin* pin, plv::PinConnection* connection);

    virtual void outputConnectionAdded(plv::IOutputPin* pin, plv::PinConnection* connection);
    virtual void outputConnectionRemoved(plv::IOutputPin* pin, plv::PinConnection* connection);

signals:
    void portChanged(int port);
    void convertCvMatDataToQImageChanged(bool b);
    void maxFramesInQueueChanged(int max);
    void maxFramesInFlightChanged(int max);
    void losslessChanged(bool lossless);

public slots:
    void setPort(int port, bool doEmit=false );
    void setConvertCvMatDataToQImage(bool doConvert, bool doEmit=false);
    void stalled(ServerConnection* connection);
    void unstalled(ServerConnection* connection);
    void setMaxFramesInQueue(int max);
    void setMaxFramesInFlight(int max);
    void setLossless(bool lossless);
    void serverError(PlvErrorType type, const QString& msg);

private:
    void acceptConfigurationRequest();

    int m_port;
    Server* m_server;
    bool m_waiting;
    bool m_convertCvMatToQImage;
    int m_cvMatDataTypeId;
};

#endif // TCPSERVERPROCESSOR_H
