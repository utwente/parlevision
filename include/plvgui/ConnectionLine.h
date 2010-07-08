#ifndef CONNECTIONLINE_H
#define CONNECTIONLINE_H

#include <QGraphicsLineItem>

#include "RefPtr.h"

using namespace plv;

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
    class PipelineElementWidget;

    /** A line that represents the connection between two pins.
      */
    class ConnectionLine : public QGraphicsLineItem
    {
//        Q_OBJECT

    public:
        ConnectionLine(PipelineElementWidget* fromItem,
                              QString fromPin,
                              PipelineElementWidget* toItem,
                              QString toPin,
                              QGraphicsItem *parent,
                              QGraphicsScene *scene);

        QRectF boundingRect() const;
        QPainterPath shape() const;

    public slots:
        void updatePosition();

    protected:
//        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
//                   QWidget *widget = 0);

    private:
        PipelineElementWidget* fromItem;
        PipelineElementWidget* toItem;
    };
}
#endif // CONNECTIONLINE_H
