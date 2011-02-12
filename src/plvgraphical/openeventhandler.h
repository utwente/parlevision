/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvgraphical module of ParleVision.
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
#ifndef OPENEVENTHANDLER_H
#define OPENEVENTHANDLER_H

#include <QObject>

namespace plv
{
    class Application;
}

class OpenEventHandler : public QObject
{
    Q_OBJECT
public:
    explicit OpenEventHandler(plv::Application* app );

protected:
    bool eventFilter(QObject* obj, QEvent *event);

signals:

public slots:

private:
    plv::Application* m_app;

};

#endif // OPENEVENTHANDLER_H
