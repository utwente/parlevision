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
    class PinCircle;

    class PinWidget : public QObject, public QGraphicsItemGroup
    {
        Q_OBJECT

    public:
        PinWidget(PipelineElementWidget* parent, RefPtr<IInputPin> p);
        PinWidget(PipelineElementWidget* parent, RefPtr<IOutputPin> p);
        virtual QRectF boundingRect() const;
        RefPtr<Pin> getPin() const {return m_pin;}
        const PinCircle* getCircle() const {return circle;}
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
        PinCircle* circle;
//        MainWindow* findMainWindow(QWidget*);
    };


    class PinCircle : public QGraphicsEllipseItem
    {
    public:
        PinCircle(qreal x, qreal y, qreal width, qreal height, PinWidget* parent);
        virtual QPainterPath shape() const;
        QPointF getCenter() const;
    private:
        RefPtr<Pin> m_pin;
        qreal m_width, m_height;
    };
}

#endif // PINWIDGET_H
