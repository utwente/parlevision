#include "PipelineScene.h"

#include <QtGui>

#include "Pipeline.h"
#include "Pin.h"
#include "PinConnection.h"
#include "PipelineElementWidget.h"
#include "ConnectionLine.h"
#include "PlvExceptions.h"
#include "PinClickedEvent.h"

using namespace plvgui;
using namespace plv;

PipelineScene::PipelineScene(plv::Pipeline* pipeline, QObject* parent) :
        QGraphicsScene(parent),
        m_pipeline(pipeline)
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
    pew->setFlag(QGraphicsItem::ItemIsMovable, true);
    pew->setFlag(QGraphicsItem::ItemIsSelectable, true);

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
    ConnectionLine* item = new ConnectionLine(getWidgetFor(c->fromPin()->getOwner()),
                                              c->fromPin()->getName(),
                                              getWidgetFor(c->toPin()->getOwner()),
                                              c->toPin()->getName(),
                                              0,
                                              this);
    getWidgetFor(c->fromPin()->getOwner())->addLine(item, c->fromPin()->getName());
    getWidgetFor(c->toPin()->getOwner())->addLine(item, c->fromPin()->getName());
}

PipelineElementWidget* PipelineScene::getWidgetFor(PipelineElement* e)
{
    return elementWidgets[e];
}

bool PipelineScene::event(QEvent* event)
{
//    qDebug() << "Scene got event " << event << " ut=" << PinClickedEvent::user_type();
//    return QObject::event(event);
    if(event->type() == PinClickedEvent::user_type())
    {
        qDebug() << "Scene got PinClickedEvent";
        event->accept();
        PinClickedEvent* pce = static_cast<PinClickedEvent>(event);
        qDebug() << pce->getSource();
    }

    return QGraphicsScene::event(event);
}
