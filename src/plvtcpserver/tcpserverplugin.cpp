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

#include "tcpserverplugin.h"
#include <QtPlugin>
#include <QtDebug>

#include "TCPServerProcessor.h"
#include "TCPClientProducer.h"

using namespace plv;

TCPServerPlugin::TCPServerPlugin()
{
    qDebug() << "TCPServerPlugin constructor";
}

TCPServerPlugin::~TCPServerPlugin()
{
    qDebug() << "TCPServerPlugin destructor";
}

void TCPServerPlugin::onLoad()
{
    qDebug() << "TCPServerPlugin onLoad";
    plvRegisterPipelineElement<TCPServerProcessor>();
    plvRegisterPipelineElement<TCPClientProducer>();
}

Q_EXPORT_PLUGIN2(tcp_server_plugin, TCPServerPlugin)
