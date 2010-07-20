#include "ViewerWidget.h"
#include "ui_viewerwidget.h"

#include <QtGui>

#include "assert.h"

#include "Pin.h"
#include "DataRenderer.h"
#include "RendererFactory.h"

using namespace plvgui;
using namespace plv;

ViewerWidget::ViewerWidget(IOutputPin* pin, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ViewerWidget),
    pin(pin)
{
    ui->setupUi(this);
    assert(this->pin.isNotNull());

    DataRenderer* renderer = RendererFactory::create(pin->getTypeInfo().name(), this);
    renderer->setPin(pin);

//    QVBoxLayout*
    this->setWidget(renderer);
    this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    this->setWindowTitle(pin->getOwner()->getName() + "-" + pin->getName());
}

ViewerWidget::~ViewerWidget()
{
    delete ui;
}

void ViewerWidget::changeEvent(QEvent *e)
{
    QDockWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


RefPtr<IOutputPin> ViewerWidget::getPin()
{
    return this->pin;
}
