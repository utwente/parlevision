#ifndef INTERACTIVELINE_H
#define INTERACTIVELINE_H

#include <QGraphicsLineItem>
#include <QObject>


//QT_BEGIN_NAMESPACE
class QGraphicsPolygonItem;
class QGraphicsLineItem;
class QGraphicsScene;
class QRectF;
class QGraphicsSceneMouseEvent;
class QPainter;
class QPainterPath;
class QStyleOptionGraphicsItem;
//QT_END_NAMESPACE


namespace plvgui
{
//    class PipelineElementWidget;
    class PinWidget;

    /** A line that represents a connection being made
      * between a Pin and the mouse.
      */
    class InteractiveLine : public QObject, public QGraphicsLineItem
    {
        Q_OBJECT

    public:
        InteractiveLine(PinWidget* fromPin,
                              QGraphicsItem *parent,
                              QGraphicsScene *scene);

        QRectF boundingRect() const;
        QPainterPath shape() const;
        PinWidget* getFromPin() const {return this->fromPin;}

    public slots:
        void updateTarget(QPointF pos);

    protected:
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget = 0);


    private:
        PinWidget* fromPin;
        QPointF targetPos;
    };
}

#endif // INTERACTIVELINE_H
