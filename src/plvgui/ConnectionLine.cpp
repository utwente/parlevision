#include "ConnectionLine.h"
#include <QtGui>

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
//! [3]

//! [4]
void ConnectionLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
          QWidget *)
{
    if (fromItem->collidesWithItem(toItem))
        return;

    QPen myPen = pen();
    myPen.setColor(Qt::black);
    qreal arrowSize = 20;
    painter->setPen(myPen);
    painter->setBrush(Qt::black);
//! [4] //! [5]

    QLineF centerLine(fromItem->pos(), toItem->pos());

    setLine(centerLine);
//! [5] //! [6]

    double angle = ::acos(line().dx() / line().length());
    if (line().dy() >= 0)
        angle = (Pi * 2) - angle;

//        QPointF arrowP1 = line().p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
//                                        cos(angle + Pi / 3) * arrowSize);
//        QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
//                                        cos(angle + Pi - Pi / 3) * arrowSize);

//        arrowHead.clear();
//        arrowHead << line().p1() << arrowP1 << arrowP2;
//! [6] //! [7]
        painter->drawLine(line());
//        painter->drawPolygon(arrowHead);
        if (isSelected()) {
            painter->setPen(QPen(Qt::black, 1, Qt::DashLine));
        QLineF myLine = line();
        myLine.translate(0, 4.0);
        painter->drawLine(myLine);
        myLine.translate(0,-8.0);
        painter->drawLine(myLine);
    }
}
