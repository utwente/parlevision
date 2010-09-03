/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvgui module of ParleVision.
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

#ifndef PINCLICKEDEVENT_H
#define PINCLICKEDEVENT_H

#include <QEvent>

static inline int getPinClickedEventType()
{
    static int sPinClickedEventType= QEvent::registerEventType();
    return sPinClickedEventType;
}

static inline int getPinDoubleClickedEventType()
{
    static int sPinDoubleClickedEventType= QEvent::registerEventType();
    return sPinDoubleClickedEventType;
}

namespace plvgui
{
    class PinWidget;

    class PinClickedEvent : public QEvent
    {
    public:
        PinClickedEvent(PinWidget* pw) :
                QEvent(PinClickedEvent::user_type()), source(pw) {}

        PinWidget* getSource() { return source; }

        /** returns the type of this event, for use in handling switches */
        static QEvent::Type user_type() {return QEvent::Type(getPinClickedEventType());}

    private:
        PinWidget* source;
    };

    class PinDoubleClickedEvent : public QEvent
    {
    public:
        PinDoubleClickedEvent(PinWidget* pw) :
                QEvent(PinDoubleClickedEvent::user_type()), source(pw) {}

        PinWidget* getSource() { return source; }

        /** returns the type of this event, for use in handilng switches */
        static QEvent::Type user_type() {return QEvent::Type(getPinDoubleClickedEventType());}

    private:
        PinWidget* source;
    };
}

#endif // PINCLICKEDEVENT_H
