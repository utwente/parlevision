#include "LibraryWidget.h"
#include "ui_librarywidget.h"
#include "PipelineElement.h"
#include "LibraryElement.h"
#include "RefPtr.h"
#include <QDebug>
#include <QtGui>

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
