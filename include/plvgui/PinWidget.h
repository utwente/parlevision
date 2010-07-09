#ifndef PINWIDGET_H
#define PINWIDGET_H

#include <QGraphicsItemGroup>
#include "RefPtr.h"


using namespace plv;

namespace plv {
    class Pin;
    class IInputPin;
    class IOutputPin;
}

namespace plvgui
{
    class PipelineElementWidget;

    class PinWidget : public QGraphicsItemGroup
    {
    public:
        PinWidget(PipelineElementWidget* parent, RefPtr<IInputPin> p);
        PinWidget(PipelineElementWidget* parent, RefPtr<IOutputPin> p);
    private:
        PipelineElementWidget* m_parent;
        RefPtr<Pin> m_pin;
        void init(bool isInput);
    };
}

#endif // PINWIDGET_H
