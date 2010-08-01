#include "PipelineScene.h"

#include <QtGui>
#include <QtGlobal>
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
#include "MainWindow.h"

using namespace plvgui;
using namespace plv;

PipelineScene::PipelineScene(plv::Pipeline* pipeline, QObject* parent) :
        QGraphicsScene(parent),
        m_pipeline(pipeline),
        line(0)
{
    // add renderers for all elements in the pipeline
    const Pipeline::PipelineElementMap& elements = pipeline->getChildren();

    QMapIterator< int, RefPtr<PipelineElement> > itr( elements );
    while( itr.hasNext() )
    {
        itr.next();
        this->add( itr.value() );
    }

    const Pipeline::PipelineConnectionsList& connections = m_pipeline->getConnections();
    foreach( RefPtr<PinConnection> connection, connections )
    {
        this->add( connection );
    }

//    this->setSceneRect(0,0,100,100);

    // make sure future additions to underlying pipeline get added as well
    connect(m_pipeline, SIGNAL(elementAdded(plv::RefPtr<plv::PipelineElement>)),
            this, SLOT(add(plv::RefPtr<plv::PipelineElement>)));

    connect(m_pipeline, SIGNAL(connectionAdded(plv::RefPtr<plv::PinConnection>)),
            this, SLOT(add(plv::RefPtr<plv::PinConnection>)));


    // make sure we stay in sync with the underlying pipeline when stuff gets removed
    connect(m_pipeline, SIGNAL(elementRemoved(plv::RefPtr<plv::PipelineElement>)),
            this, SLOT(handleRemove(plv::RefPtr<plv::PipelineElement>)));

    connect(m_pipeline, SIGNAL(connectionRemoved(plv::RefPtr<plv::PinConnection>)),
            this, SLOT(handleRemove(plv::RefPtr<plv::PinConnection>)));

    connect(this, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(update(QRectF)));

    connect(this, SIGNAL(changed(QList<QRectF>)), this, SLOT(recalculateSceneRect()));

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
        throw PipelineException("Cannot add duplicate pipeline element to the scene.");


    PipelineElementWidget* pew = new PipelineElementWidget(e.getPtr());
    this->addItem(pew);
    this->elementWidgets[e] = pew;

    QVariant xVal = e->property("sceneCoordX");
    QVariant yVal = e->property("sceneCoordY");
    qreal x = xVal.isValid() ? xVal.toReal() : 0;
    qreal y = yVal.isValid() ? yVal.toReal() : 0;

    pew->translate(x,y);
    this->ensureFit();
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
    assert(!this->connectionLines.contains(c.getPtr()));
    this->connectionLines[c.getPtr()] = item;
    getWidgetFor(c->fromPin()->getOwner())->addLine(item, c->fromPin()->getName());
    getWidgetFor(c->toPin()->getOwner())->addLine(item, c->fromPin()->getName());
}

void PipelineScene::deleteSelected()
{
    foreach(QGraphicsItem* selectedItem, this->selectedItems())
    {
        ConnectionLine* connectionLine = dynamic_cast<ConnectionLine*> (selectedItem);
        if(connectionLine)
        {
            remove(connectionLine->getPinConnection());
        }
        else
        {
            PipelineElementWidget* pew = dynamic_cast<PipelineElementWidget*> (selectedItem);
            if(pew)
            {
                remove(pew->getElement());
            }
        }
    }
}

void PipelineScene::remove(plv::PipelineElement* e)
{
    remove(RefPtr<PipelineElement>(e));
}

void PipelineScene::remove(plv::RefPtr<plv::PipelineElement> e)
{
    this->m_pipeline->removeElement( e->getId() );
}

void PipelineScene::remove(plv::PinConnection* c)
{
    remove(RefPtr<PinConnection>(c));
}

void PipelineScene::remove(plv::RefPtr<plv::PinConnection> c)
{
    this->m_pipeline->disconnect(c);
}

void PipelineScene::handleRemove(plv::PipelineElement* e)
{
    handleRemove(RefPtr<PipelineElement>(e));
}

void PipelineScene::handleRemove(plv::RefPtr<plv::PipelineElement> e)
{
    QGraphicsItem* item = this->getWidgetFor(e);
    if(item)
    {
        this->removeItem(item);
    }
}

void PipelineScene::handleRemove(plv::PinConnection* c)
{
    handleRemove(RefPtr<PinConnection>(c));
}

void PipelineScene::handleRemove(plv::RefPtr<plv::PinConnection> c)
{
    QGraphicsItem* item = this->getWidgetFor(c);
    if(item)
    {
        this->removeItem(item);
    }
}



PipelineElementWidget* PipelineScene::getWidgetFor(PipelineElement* e) const
{
    return elementWidgets[e];
}

PinWidget* PipelineScene::getWidgetFor(const Pin* p) const
{
    return getWidgetFor(p->getOwner())->getWidgetFor(p);
}

ConnectionLine* PipelineScene::getWidgetFor(PinConnection* c) const
{
    return connectionLines[c];
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
                try
                {
                    handleConnectionCreation(line->getFromPin(), pw);
                }
                catch(NonFatalException e)
                {
                    MainWindow* mw = getMainWindow();

                    if(mw)
                        mw->handleMessage(QtCriticalMsg, e.what());
                    else
                        qCritical() << e.what();
                }
            }
        }
    }

    clearLine();
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
    ensureFit();
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
           throw (NonFatalException)
{
    RefPtr<IOutputPin> fromPin = ref_ptr_dynamic_cast<IOutputPin>(source->getPin());

    if(fromPin.isNull())
        throw NonFatalException("Cannot make connection: Source is not an output pin");

    RefPtr<IInputPin> toPin = ref_ptr_dynamic_cast<IInputPin>(target->getPin());

    if(toPin.isNull())
        throw NonFatalException("Cannot make connection: Target is not an input pin");

    qDebug() << "Making connection "
            << fromPin->getOwner()->getName() << "/" << fromPin->getName()
            << " -> "
            << toPin->getOwner()->getName() << "/" << toPin->getName();

    try
    {
        this->m_pipeline->connectPins(fromPin,toPin);
    }
    catch(IncompatibleTypeException e)
    {
        throw NonFatalException(e.what());
    }
    catch(DuplicateConnectionException e)
    {
        throw NonFatalException(e.what());
    }
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
        pe->setProperty("sceneCoordX", event->scenePos().x());
        pe->setProperty("sceneCoordY", event->scenePos().y());

        if(m_pipeline.isNotNull())
        {
            m_pipeline->addElement( pe );
        }
    }
}

MainWindow* PipelineScene::getMainWindow()
{
    foreach(QWidget* tlw, QApplication::topLevelWidgets())
    {
        MainWindow* mw = qobject_cast<MainWindow*>(tlw);
        if(mw != 0 && mw->isAncestorOf(this->views().first()))
        {
            return mw;
        }
    }

    return 0;
}

void PipelineScene::ensureFit()
{
    foreach(PipelineElementWidget* item, this->elementWidgets.values())
    {

        if(item->scenePos().x() < 0.0)
        {
            qDebug() << "moving " << item;
            item->translate(-item->scenePos().x(),0);
        }
        if(item->scenePos().y() < 0.0)
        {
            item->translate(0,-item->scenePos().y());
        }
    }
}

void PipelineScene::recalculateSceneRect()
{
    QRectF r = itemsBoundingRect();
    QRectF newRect = QRectF(0.0,
                  0.0,
                  qMax(320.0, r.width()),
                  qMax(240.0, r.height()));
    qDebug() << "new rect: " << newRect;
    setSceneRect(newRect);
}
