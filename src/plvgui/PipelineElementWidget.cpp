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
        QGraphicsWidget(parent,wFlags)
{
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout;

    QGraphicsProxyWidget* proxy = new QGraphicsProxyWidget(this);

    QWidget* rep = new QWidget();
    QVBoxLayout* outerContainer = new QVBoxLayout(rep);
    outerContainer->setAlignment(Qt::AlignHCenter);
    rep->setLayout(outerContainer);

    QLabel* titleLabel = new QLabel(element->metaObject()->className(), rep);
    outerContainer->addWidget(titleLabel);

    QHBoxLayout* pinsContainer = new QHBoxLayout(rep);
    outerContainer->addLayout(pinsContainer);
    QVBoxLayout* inputPinsContainer = new QVBoxLayout(rep);
    QVBoxLayout* outputPinsContainer = new QVBoxLayout(rep);
    outputPinsContainer->setAlignment(Qt::AlignRight);
    pinsContainer->addLayout(inputPinsContainer);
    pinsContainer->addLayout(outputPinsContainer);

    std::list<QString>* inPinNames = element->getInputPinNames();
    for(std::list<QString>::iterator itr = inPinNames->begin();
        itr != inPinNames->end();
        ++itr)
    {
        inputPinsContainer->addWidget(new QLabel(*itr));
    }

    std::list<QString>* outPinNames = element->getOutputPinNames();
    for(std::list<QString>::iterator itr = outPinNames->begin();
        itr != outPinNames->end();
        ++itr)
    {
        outputPinsContainer->addWidget(new QLabel(*itr));
    }


    proxy->setWidget(rep);

    layout->addItem(proxy);

    this->setLayout(layout);
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
        foreach (ConnectionLine* line, lines) {
            line->updatePosition();
        }
        qDebug() << "Element changed";
    }

    return value;
}
