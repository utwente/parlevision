#ifndef PINCLICKEDEVENT_H
#define PINCLICKEDEVENT_H

#include <QEvent>

static QEvent::Type pinClickedEventType()
{
    static int sPinClickedEventType = 0;
    if(sPinClickedEventType == 0)
    {
        sPinClickedEventType = QEvent::registerEventType();
        qDebug() << "Registering custom event as " << sPinClickedEventType;
    }
    return QEvent::Type(sPinClickedEventType);
}

namespace plvgui
{
    class PinWidget;

    class PinClickedEvent : public QEvent
    {
    public:
        PinClickedEvent(PinWidget* pw) :
                QEvent(pinClickedEventType()), source(pw)
        {
            qDebug() << "Constructing PinClickedEvent " << type();
        }

        PinWidget* getSource() { return source; }

        /** returns the type of this event, for use in handilng switches */
        static QEvent::Type user_type() {return pinClickedEventType();}

    private:
        PinWidget* source;
    };
}

#endif // PINCLICKEDEVENT_H
