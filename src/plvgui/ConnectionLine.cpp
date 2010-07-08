#include "ConnectionLine.h"
#include <QtGui>

#include <QDebug>

#include "PipelineElementWidget.h"

const qreal Pi = 3.14;

using namespace plv;
using namespace plvgui;

ConnectionLine::ConnectionLine(PipelineElementWidget* fromItem,
                               QString fromPin,
                               PipelineElementWidget* toItem,
                               QString toPin,
                               QGraphicsItem *parent,
                               QGraphicsScene *scene)
                                   : QGraphicsLineItem(parent, scene),
                                   fromItem(fromItem),
                                   toItem(toItem)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
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
    QLineF line(mapFromItem(fromItem, 0, 0), mapFromItem(toItem, 0, 0));
    setLine(line);
}

//void ConnectionLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
//          QWidget *)
//{
////    if (fromItem->collidesWithItem(toItem))
////        return;

//    QPen myPen = pen();
//    myPen.setColor(Qt::black);
//    qreal arrowSize = 20;
//    painter->setPen(myPen);
//    painter->setBrush(Qt::black);

//    QLineF centerLine(fromItem->scenePos(), toItem->scenePos());

//    qDebug() << centerLine;

//    setLine(centerLine);

//    painter->drawLine(centerLine);
//}
