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
//! [2]

//! [3]
void ConnectionLine::updatePosition()
{
    QLineF line(mapFromItem(fromPin, 0, 0), mapFromItem(toPin, 0, 0));
    setLine(line);
}

void ConnectionLine::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem* style,
                           QWidget * w)
{
    QLineF line(mapFromItem(fromPin, 0, 0), mapFromItem(toPin, 0, 0));
    setLine(line);
    QGraphicsLineItem::paint(painter, style, w);
////    if (fromPin->collidesWithItem(toPin))
////        return;

//    QPen myPen = pen();
//    myPen.setColor(Qt::black);
//    qreal arrowSize = 20;
//    painter->setPen(myPen);
//    painter->setBrush(Qt::black);

//    QLineF centerLine(fromPin->scenePos(), toPin->scenePos());

//    qDebug() << centerLine;

//    setLine(centerLine);

//    painter->drawLine(centerLine);
}
