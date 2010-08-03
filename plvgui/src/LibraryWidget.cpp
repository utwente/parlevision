#include "LibraryWidget.h"
#include "ui_librarywidget.h"
#include "PipelineElement.h"
#include "LibraryElement.h"
#include "RefPtr.h"
#include "Pin.h"
#include <QDebug>
#include <QtGui>
#include <QStringBuilder>

using namespace plvgui;
using namespace plv;

LibraryWidget::LibraryWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LibraryWidget),
    draggedElement(0)
{
    ui->setupUi(this);

    std::list<QString> types = plv::PipelineElement::types();

    for(std::list<QString>::iterator iter = types.begin();
        iter != types.end(); iter++)
    {
        qDebug() << "LibraryWidget: Adding type " << *iter;
        createItem(*iter);
    }
    // add sorted by name
    foreach(LibraryElement* lw, this->allElements.values())
    {
        ui->container->addWidget(lw);
    }
}

LibraryWidget::~LibraryWidget()
{
    delete ui;
}

void LibraryWidget::createItem(QString typeName)
{
    int id = QMetaType::type(typeName.toAscii());
    if(!QMetaType::isRegistered(id))
    {
        qWarning() << "Ignoring unknown element " << typeName;
        return;
    }

    RefPtr<PipelineElement> element = static_cast<PipelineElement*>(QMetaType::construct(id));

    LibraryElement* w = new LibraryElement(element, this);
    connect(w, SIGNAL(pressed()), this, SLOT(elementPressed()));
    connect(w, SIGNAL(moved()), this, SLOT(elementMoved()));
    connect(w, SIGNAL(released()), this, SLOT(elementReleased()));
//    ui->container->addWidget(w);
    this->allElements.insert(w->getElement()->getName().toLower(), w);
}

void LibraryWidget::mousePressEvent(QMouseEvent *event)
{
    QDockWidget::mousePressEvent(event);
}

void LibraryWidget::elementPressed()
{
    // Currently not needed
}

void LibraryWidget::elementMoved()
{
    if(this->draggedElement != 0)
        return;

    // start drag
    this->draggedElement = qobject_cast<LibraryElement*>(sender());

    QPoint hotSpot = QPoint(0,0);
    QString elementName = draggedElement->getElement()->metaObject()->className();
//    qDebug() << "starting drag of " << elementName << " hotspot: "<< hotSpot;

    QByteArray itemData;
    itemData.append(elementName);

    QMimeData* mimeData = new QMimeData();
    mimeData->setData("x-plv-element-name", itemData);

    QPixmap pixmap(draggedElement->size());
    draggedElement->render(&pixmap);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(hotSpot);
    drag->exec(Qt::CopyAction, Qt::CopyAction);
}

void LibraryWidget::elementReleased()
{
    LibraryElement* element = qobject_cast<LibraryElement*>(sender());
    if (!element) return;

    if(this->draggedElement != 0)
    {
        // we were dragging
        // don't do anything?
    }
    else
    {
        ui->infoBox->setText(infoFor(element->getElement()));
    }
    this->draggedElement = 0;
}

QString LibraryWidget::infoFor(plv::PipelineElement* element)
{
    QString desc = element->getClassProperty("description");
    if(desc.isEmpty()) { desc = "(no description)"; }

    QString inputPinRows;
    const PipelineElement::InputPinMap& inPins = element->getInputPins();
    for( PipelineElement::InputPinMap::const_iterator itr = inPins.begin()
        ; itr!=inPins.end(); ++itr)
    {
        RefPtr<IInputPin> pin = itr->second;
        inputPinRows = inputPinRows % "<tr>"
                        % "<td>" % QString(pin->getName()) %"</td>"
                        % "<td>" % QString(pin->getTypeInfo().name()) %"</td>"
                        % "</tr>";
    }


    QString outputPinRows;
    const PipelineElement::OutputPinMap& outPins = element->getOutputPins();
    for( PipelineElement::OutputPinMap::const_iterator itr = outPins.begin()
        ; itr!=outPins.end(); ++itr)
    {
        RefPtr<IOutputPin> pin = itr->second;
        outputPinRows = outputPinRows % "<tr>"
                        % "<td>" % QString(pin->getName()) %"</td>"
                        % "<td>" % QString(pin->getTypeInfo().name()) %"</td>"
                        % "</tr>";
    }

    return "<h1>" % element->getName() % "</h1>"
            % "<p>"
            % desc % "</p>"
            % "<h2>Input Pins</h2>"
            % "<table><tr><th>Name</th><th>Type</th>"
            % inputPinRows
            % "</table>"
            % "<h2>Output Pins</h2>"
            % "<table><tr><th>Name</th><th>Type</th>"
            % outputPinRows
            % "</table>";

}
