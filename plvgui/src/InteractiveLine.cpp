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
