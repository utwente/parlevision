#include "ConnectionLine.h"
#include <QtGui>

#include <QDebug>

#include "PinWidget.h"

const qreal Pi = 3.14;

using namespace plv;
using namespace plvgui;

ConnectionLine::ConnectionLine(PinWidget* fromPin,
                               PinWidget* toPin,
                               PinConnection* connection,
                               QGraphicsItem *parent,
                               QGraphicsScene *scene)
                                   : QGraphicsLineItem(parent, scene),
                                   fromPin(fromPin),
                                   toPin(toPin),
                                   connection(connection)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

QRectF ConnectionLine::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

QPainterPath ConnectionLine::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
//    path.addPolygon(arrowHead);
    return path;
}

void ConnectionLine::updatePosition()
{
    QLineF line(mapFromItem(fromPin, 0, 0), mapFromItem(toPin, 0, 0));
    setLine(line);
}

void ConnectionLine::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem* style,
                           QWidget * w)
{
    // find the centers of the circles to use as coordinates for the line
    qreal width = fromPin->getCircle()->boundingRect().width();
    QPointF p1 = QPointF(fromPin->getCircle()->scenePos())
                    + QPointF(width/2.0,width/2.0);

    QPointF p2 = QPointF(toPin->getCircle()->scenePos())
                    + QPointF(width/2.0,width/2.0);

    QLineF line(p1, p2);
    setLine(line);
    QGraphicsLineItem::paint(painter, style, w);

}
