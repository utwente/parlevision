#include "PinClickedEvent.h"

using namespace plvgui;

QEvent::Type PinClickedEvent::sPinClickedEventType = QEvent::Type( QEvent::registerEventType() );
QEvent::Type PinDoubleClickedEvent::sPinDoubleClickedEventType = QEvent::Type( QEvent::registerEventType() );

QEvent::Type PinClickedEvent::user_type()
{
    return sPinClickedEventType;
}

QEvent::Type PinDoubleClickedEvent::user_type()
{
    return sPinDoubleClickedEventType;
}

PinClickedEvent::PinClickedEvent(PinWidget* pw) :
        QEvent( PinClickedEvent::user_type() ),
        source( pw )
{
}

PinDoubleClickedEvent::PinDoubleClickedEvent(PinWidget* pw) :
        QEvent( PinDoubleClickedEvent::user_type() ),
        source( pw )
{
}
