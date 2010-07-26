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

QRectF ConnectionLine::boundingRect() const
{
    qreal extra = (pen().width() - 10) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
 }


void ConnectionLine::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem* style,
                           QWidget * w)
{
    QLineF line(fromPin->getCircle()->mapToScene(fromPin->getCircle()->getCenter()),
                toPin->getCircle()->mapToScene(toPin->getCircle()->getCenter()));

    setLine(line);

    if(isSelected())
    {
        QPen dashPen = this->pen();
        dashPen.setStyle(Qt::DotLine);
        dashPen.setWidth(5);
        painter->setPen(dashPen);
        painter->drawLine(line);

        QPen whitePen = this->pen();
        whitePen.setColor(Qt::lightGray);
        painter->setPen(whitePen);
        painter->drawLine(line);
    }
    else
    {
        painter->setPen(this->pen());
    }

    painter->drawLine(line);
}
