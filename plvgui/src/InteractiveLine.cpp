#include "InteractiveLine.h"

#include <QtGui>

#include <QDebug>

#include "PinWidget.h"

using namespace plv;
using namespace plvgui;

InteractiveLine::InteractiveLine(PinWidget* fromPin,
                               QGraphicsItem *parent,
                               QGraphicsScene *scene)
                                   : QObject(scene),
                                   QGraphicsLineItem(parent, scene),
                                   fromPin(fromPin),
                                   targetPos(fromPin->getCircle()->mapToScene(fromPin->getCircle()->getCenter()))
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

QRectF InteractiveLine::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

QPainterPath InteractiveLine::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
//    path.addPolygon(arrowHead);
    return path;
}

void InteractiveLine::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem* style,
                           QWidget * w)
{
    QLineF line(fromPin->getCircle()->mapToScene(fromPin->getCircle()->getCenter()),
                this->targetPos);
    setLine(line);
    QGraphicsLineItem::paint(painter, style, w);

}

void InteractiveLine::updateTarget(QPointF pos)
{
    this->targetPos = pos;
    update();
}
