#include "CameraConfigFormBuilder.h"
#include "ui_cameraconfigform.h"

#include "PipelineElement.h"
#include "CameraProducer.h"

using namespace plvgui;
using namespace plv;

CameraForm::CameraForm(CameraProducer *producer, QWidget *parent) :
        ui(new Ui::CameraConfigForm()),
        producer(producer)
{
    ui->setupUi(this);
//    connect(ui->cameraSelector, SIGNAL(activated(int)),
//            producer, SLOT(todo(int)));
}

QWidget* CameraConfigFormBuilder::buildForm(PipelineElement* element, QWidget* parent)
{
    RefPtr<CameraProducer> cp = dynamic_cast<CameraProducer*>(element);
    if(cp.isNull())
    {
        //TODO handle this better
        return new QWidget(parent);
    }

    return new CameraForm(cp, parent);
}
