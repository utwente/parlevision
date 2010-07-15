#include "PipelineScene.h"

#include <QtGui>
#include <QDebug>

#include "Pipeline.h"
#include "Pin.h"
#include "PinConnection.h"
#include "PipelineElementWidget.h"
#include "ConnectionLine.h"
#include "InteractiveLine.h"
#include "PlvExceptions.h"
#include "PinClickedEvent.h"
#include "PinWidget.h"

using namespace plvgui;
using namespace plv;

PipelineScene::PipelineScene(plv::Pipeline* pipeline, QObject* parent) :
        QGraphicsScene(parent),
        m_pipeline(pipeline),
        line(0)
{
    std::list< RefPtr<PipelineElement> > elements = m_pipeline->getChildren();

    // add all elements from the pipeline to this scene
    for( std::list< RefPtr<PipelineElement> >::iterator itr = elements.begin()
        ; itr != elements.end(); ++itr )
    {
        this->add(*itr);
    }

    const std::list< RefPtr<PinConnection> > connections = m_pipeline->getConnections();
    // add all connections from the pipeline to this scene
    for( std::list< RefPtr<PinConnection> >::const_iterator itr = connections.begin()
        ; itr != connections.end(); ++itr )
    {
        this->add(*itr);
    }


    // make sure future additions to pipeline get added as well
    connect(m_pipeline, SIGNAL(elementAdded(plv::RefPtr<plv::PipelineElement>)),
            this, SLOT(add(plv::RefPtr<plv::PipelineElement>)));

    connect(m_pipeline, SIGNAL(connectionAdded(plv::RefPtr<plv::PinConnection>)),
            this, SLOT(add(plv::RefPtr<plv::PinConnection>)));

}

void PipelineScene::add(plv::PipelineElement* e)
{
    add(RefPtr<PipelineElement>(e));
}

void PipelineScene::add(plv::RefPtr<plv::PipelineElement> e)
{
    //TODO
    qDebug() << "PipelineScene: adding element " << e;
//    QGraphicsTextItem* item = this->addText(e->metaObject()->className());
//    item->setFlag(QGraphicsItem::ItemIsMovable, true);
//    item->setFlag(QGraphicsItem::ItemIsSelectable, true);

    if(this->elementWidgets.contains(e))
        throw new PipelineException("Cannot add duplicate pipeline element to the scene.");


    PipelineElementWidget* pew = new PipelineElementWidget(e.getPtr());
    this->addItem(pew);
    this->elementWidgets[e] = pew;
}

void PipelineScene::add(plv::PinConnection* c)
{
    add(RefPtr<PinConnection>(c));
}

void PipelineScene::add(plv::RefPtr<plv::PinConnection> c)
{
    //TODO
    qDebug() << "PipelineScene: adding connection ";
    RefPtr<const Pin> from = c->fromPin().getPtr();
//    const QString& desc = from->getName();
//    QGraphicsTextItem* item = this->addText(desc);
    ConnectionLine* item = new ConnectionLine(getWidgetFor(c->fromPin().getPtr()),
                                              getWidgetFor(c->toPin().getPtr()),
                                              c.getPtr(),
                                              0,
                                              this);
    getWidgetFor(c->fromPin()->getOwner())->addLine(item, c->fromPin()->getName());
    getWidgetFor(c->toPin()->getOwner())->addLine(item, c->fromPin()->getName());
}

PipelineElementWidget* PipelineScene::getWidgetFor(PipelineElement* e) const
{
    return elementWidgets[e];
}

PinWidget* PipelineScene::getWidgetFor(const Pin* p) const
{
    return getWidgetFor(p->getOwner())->getWidgetFor(p);
}

bool PipelineScene::event(QEvent* event)
{
//    qDebug() << "Scene got event " << event << " ut=" << PinClickedEvent::user_type();
//    return QObject::event(event);
    if(event->type() == PinClickedEvent::user_type())
    {
        PinClickedEvent* pce = static_cast<PinClickedEvent*>(event);
        qDebug() << pce->getSource()->getPin()->getName();

        // if the event originated from an outputpin, start dragging a line
        if(ref_ptr_dynamic_cast<IOutputPin>(pce->getSource()->getPin()).isNotNull())
        {
            event->accept();
            clearLine();
            this->line = new InteractiveLine(pce->getSource(), 0, this);
        }
    }

    return QGraphicsScene::event(event);
}

//void PipelineScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
//{
//    QGraphicsScene::mousePressEvent(mouseEvent);
//}

void PipelineScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if(this->line != 0)
    {
        this->line->updateTarget(mouseEvent->scenePos());
    }
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void PipelineScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    // Detect if the mouse was released while dragging a line
    if(this->line != 0)
    {
        // find any items below the mouse
        // and see if one of them is a PinWidget
        foreach(QGraphicsItem* item, this->items(mouseEvent->scenePos()))
        {
            PinWidget* pw = dynamic_cast<PinWidget*>(item);
            if(pw != 0)
            {
                mouseEvent->accept();

                handleConnectionCreation(line->getFromPin(), pw);
            }
        }
    }

    clearLine();

    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void PipelineScene::clearLine()
{
    if(this->line != 0)
    {
        this->removeItem(this->line);
        delete this->line;
        this->line = 0;
    }
}

void PipelineScene::handleConnectionCreation(PinWidget* source, PinWidget* target)
{
    RefPtr<IOutputPin> fromPin = ref_ptr_dynamic_cast<IOutputPin>(source->getPin());
    assert(fromPin.isNotNull());

    RefPtr<IInputPin> toPin = ref_ptr_dynamic_cast<IInputPin>(target->getPin());
    if(toPin.isNull())
    {
        qWarning() << "Cannot make connection: Target is not an input pin";
        return;
    }
    assert(toPin.isNotNull());

    qDebug() << "Making connection "
            << fromPin->getOwner()->getName() << "/" << fromPin->getName()
            << " -> "
            << toPin->getOwner()->getName() << "/" << toPin->getName();

    this->m_pipeline->connectPins(fromPin,toPin);
}

void PipelineScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "Scene Enter";
    if(event->mimeData()->hasFormat("x-plv-element-name"))
    {
        event->accept();
    }
}

void PipelineScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if(event->mimeData()->hasFormat("x-plv-element-name"))
    {
        event->accept();
    }
}

void PipelineScene::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    qDebug() << "PipelineScene::dropEvent" << event->mimeData()->formats();

    if(event->mimeData()->hasFormat("x-plv-element-name"))
    {
    //    qDebug() << event->mimeData()->data("x-plv-element-name");
        QString elementName = QString(event->mimeData()->data("x-plv-element-name"));
        qDebug() << elementName;

        int typeId = QMetaType::type(elementName.toAscii());

        if(typeId == 0)
            throw new ElementCreationException(
                    QString("Tried to create unknown element "+elementName).toStdString());

        RefPtr<PipelineElement> pe = static_cast<PipelineElement*>(QMetaType::construct(typeId));

        if(m_pipeline.isNotNull())
        {
            m_pipeline->add(pe);
        }
    }
}

