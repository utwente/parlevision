#ifndef PINCLICKEDEVENT_H
#define PINCLICKEDEVENT_H

#include <QEvent>

static inline int getPinClickedEventType()
{
    static int sPinClickedEventType= QEvent::registerEventType();
    return sPinClickedEventType;
}

namespace plvgui
{
    class PinWidget;

    class PinClickedEvent : public QEvent
    {
    public:
        PinClickedEvent(PinWidget* pw) :
                QEvent(PinClickedEvent::user_type()), source(pw)
        {
            qDebug() << "Constructing PinClickedEvent " << type();
        }

        PinWidget* getSource() { return source; }

        /** returns the type of this event, for use in handilng switches */
        static QEvent::Type user_type() {return QEvent::Type(getPinClickedEventType());}

    private:
        PinWidget* source;
    };
}

#endif // PINCLICKEDEVENT_H
