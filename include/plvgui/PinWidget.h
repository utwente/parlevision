#ifndef PINWIDGET_H
#define PINWIDGET_H

#include <QGraphicsItemGroup>
#include "RefPtr.h"
#include "Pin.h"


class QGraphicsItem;

using namespace plv;

//namespace plv {
//    class Pin;
//    class IInputPin;
//    class IOutputPin;
//}

namespace plvgui
{
    class MainWindow;
    class PipelineElementWidget;

    class PinWidget : public QObject, public QGraphicsItemGroup
    {
        Q_OBJECT

    public:
        PinWidget(PipelineElementWidget* parent, RefPtr<IInputPin> p);
        PinWidget(PipelineElementWidget* parent, RefPtr<IOutputPin> p);
        virtual QRectF boundingRect() const;
        RefPtr<Pin> getPin() const {return m_pin;}
        const QGraphicsItem* getCircle() const {return circle;}
        void handleMouseDoubleClick();

    protected:
        virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
        virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
        virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
//        virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
        virtual bool sceneEvent ( QEvent * event );
//        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    private:
        PipelineElementWidget* m_parent;
        RefPtr<Pin> m_pin;
        void init(bool isInput);
        QGraphicsEllipseItem* circle;
//        MainWindow* findMainWindow(QWidget*);
    };
}

#endif // PINWIDGET_H
