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
    setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
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

    QLineF line(fromPin->getCircle()->mapToScene(fromPin->getCircle()->getCenter()),
                toPin->getCircle()->mapToScene(toPin->getCircle()->getCenter()));

    setLine(line);
    QGraphicsLineItem::paint(painter, style, w);

}
