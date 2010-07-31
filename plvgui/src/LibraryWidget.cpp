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
    ui(new Ui::LibraryWidget)
{
    ui->setupUi(this);

    std::list<QString> types = plv::PipelineElement::types();

    for(std::list<QString>::iterator iter = types.begin();
        iter != types.end(); iter++)
    {
        qDebug() << "LibraryWidget: Adding type " << *iter;
        addItem(*iter);
    }
}

LibraryWidget::~LibraryWidget()
{
    delete ui;
}

void LibraryWidget::addItem(QString typeName)
{
    int id = QMetaType::type(typeName.toAscii());
    if(!QMetaType::isRegistered(id))
    {
        qWarning() << "Ignoring unknown element " << typeName;
        return;
    }

    RefPtr<PipelineElement> element = static_cast<PipelineElement*>(QMetaType::construct(id));

    LibraryElement* w = new LibraryElement(element, this);
    ui->container->addWidget(w);
}

void plvgui::LibraryWidget::on_pushButton_clicked()
{
    addItem("test");
}

void LibraryWidget::mousePressEvent(QMouseEvent *event)
{
    LibraryElement* element = dynamic_cast<LibraryElement*>(childAt(event->pos()));

    if (!element) return;

    ui->infoBox->setText(infoFor(element->getElement()));

    QPoint hotSpot = event->pos() - element->pos();

    QString elementName = element->getElement()->metaObject()->className();
    qDebug() << "starting drag of " << elementName;

    QByteArray itemData;
    itemData.append(elementName);

    QMimeData* mimeData = new QMimeData();
//    mimeData->setText(elementName);
    mimeData->setData("x-plv-element-name", itemData);

    QPixmap pixmap(element->size());
    element->render(&pixmap);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(hotSpot);

    drag->exec(Qt::CopyAction, Qt::CopyAction);
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
