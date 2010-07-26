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

namespace plv
{
    class PinConnection;
}

namespace plvgui
{
//    class PipelineElementWidget;
    class PinWidget;

    /** A line that represents the connection between two pins.
      */
    class ConnectionLine : public QGraphicsLineItem
    {
//        Q_OBJECT

    public:
        ConnectionLine(PinWidget* fromPin,
                              PinWidget* toPin,
                              PinConnection* connection,
                              QGraphicsItem *parent,
                              QGraphicsScene *scene);

        QPainterPath shape() const;
        RefPtr<PinConnection> getPinConnection() const { return connection; }

    public slots:
        void updatePosition();

    protected:
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget = 0);

    private:
        PinWidget* fromPin;
        PinWidget* toPin;
        RefPtr<PinConnection> connection;
    };
}
#endif // CONNECTIONLINE_H
