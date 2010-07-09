#include "PipelineElementWidget.h"

#include <list>
#include <QtGui>

#include "PipelineElement.h"
#include "ConnectionLine.h"

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

    std::list<QString>* inPinNames = element->getInputPinNames();
    for(std::list<QString>::iterator itr = inPinNames->begin();
        itr != inPinNames->end();
        ++itr)
    {
//        inputPinsContainer->addWidget(new QLabel(*itr));
        this->addToGroup(new QGraphicsTextItem(*itr));

    }

    std::list<QString>* outPinNames = element->getOutputPinNames();
    for(std::list<QString>::iterator itr = outPinNames->begin();
        itr != outPinNames->end();
        ++itr)
    {
//        outputPinsContainer->addWidget(new QLabel(*itr));
    }


//    proxy->setWidget(rep);

//    layout->addItem(proxy);

//    this->setLayout(layout);
}

void PipelineElementWidget::addLine(ConnectionLine *line, QString pin)
{
    pin;
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
