#include "PipelineElementWidget.h"

#include <list>
#include <QtGui>

#include "PipelineElement.h"
#include "ConnectionLine.h"
#include "PinWidget.h"
#include "Pin.h"
#include "RefPtr.h"

using namespace plvgui;
using namespace plv;

PipelineElementWidget::PipelineElementWidget(PipelineElement* element,
                                             QGraphicsItem* parent,
                                             Qt::WindowFlags wFlags) :
        QGraphicsItemGroup(parent),
        element(element)
{

    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
//    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical);

//    QGraphicsProxyWidget* proxy = new QGraphicsProxyWidget(this);

//    QWidget* rep = new QWidget();
//    QVBoxLayout* outerContainer = new QVBoxLayout(rep);
//    outerContainer->setAlignment(Qt::AlignHCenter);
//    rep->setLayout(outerContainer);
    QGraphicsTextItem* titleLabel = new QGraphicsTextItem(element->getName(), this);
//    layout->addItem(titleLabel);
//    titleLabel->setParentItem(this);
    this->addToGroup(titleLabel);

//    titleLabel->setFlag(QGraphicsItem::ItemIsMovable, true);
//    titleLabel->setFlag(QGraphicsItem::ItemIsSelectable, true);

//    QGraphicsLinearLayout* pinsContainer = new QGraphicsLinearLayout(Qt::Horizontal);
//    layout->addItem(pinsContainer);
//    QGraphicsLinearLayout* inputPinsContainer = new QGraphicsLinearLayout(Qt::Vertical);
//    QGraphicsLinearLayout* outputPinsContainer = new QGraphicsLinearLayout(Qt::Vertical);
//    outputPinsContainer->setAlignment(Qt::AlignRight);
//    pinsContainer->addItem(inputPinsContainer);
//    pinsContainer->addItem(outputPinsContainer);



//    std::list<QString>* inPinNames = element->getInputPinNames();
    std::list< RefPtr<IInputPin> >* inPins = element->getInputPins();
    qDebug() << "inpins: " << inPins->size();
    int y = 20;
    for(std::list< RefPtr<IInputPin> >::iterator itr = inPins->begin();
        itr != inPins->end();
        ++itr)
    {
//        inputPinsContainer->addWidget(new QLabel(*itr));;
        qDebug() << "Pin " << y << ": " << (int)(*itr).getPtr();
        RefPtr<IInputPin> pin = *itr;
        assert(pin.isNotNull());
        PinWidget* label = new PinWidget(this, pin);
//        QGraphicsTextItem* label = new QGraphicsTextItem(*itr);
        label->translate(0, y);
        this->addToGroup(label);
        y+=10;
    }


    y = 20;
    std::list<QString>* outPinNames = element->getOutputPinNames();
    for(std::list<QString>::iterator itr = outPinNames->begin();
        itr != outPinNames->end();
        ++itr)
    {
//        outputPinsContainer->addWidget(new QLabel(*itr));
        QGraphicsTextItem* label = new QGraphicsTextItem(*itr);
        label->translate(100, y);
        this->addToGroup(label);
        y+=10;
    }

//    proxy->setWidget(rep);

//    layout->addItem(proxy);

//    this->setLayout(layout);
}

void PipelineElementWidget::addLine(ConnectionLine *line, QString pin)
{
    Q_UNUSED(pin);
    this->lines.append(line);
}

QVariant PipelineElementWidget::itemChange(GraphicsItemChange change,
                     const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
//        qDebug() << "Element changed " << change;
        foreach (ConnectionLine* line, lines) {
            line->updatePosition();
        }
    }

    return value;
}

void PipelineElementWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    if(this->isSelected())
    {
        //TODO move this outside paint
        QPen pen;
        pen.setStyle(Qt::DashDotLine);
        pen.setWidth(3);
        pen.setColor(Qt::black);
        painter->setPen(pen);
    }
    else
    {
        painter->setPen(Qt::black);
    }

    painter->setBrush(Qt::green);
    painter->drawRoundedRect(this->boundingRect(),8,8, Qt::AbsoluteSize);
}
