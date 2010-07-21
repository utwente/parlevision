#include "CameraConfigFormBuilder.h"
#include "ui_cameraconfigform.h"

#include "PipelineElement.h"
#include "CameraProducer.h"
#include "OpenCVCamera.h"

using namespace plvgui;
using namespace plv;

CameraForm::CameraForm(CameraProducer *producer, QWidget *parent) :
        ui(new Ui::CameraConfigForm()),
        producer(producer)
{
    ui->setupUi(this);
    int numCameras = 1; //TODO find out a way to do this right
    for(int i = 0; i < numCameras; i++)
    {
        ui->cameraSelector->addItem(QVariant(i).toString());
    }
    ui->cameraSelector->setDisabled(true);

//    RefPtr<OpenCVCamera> cam = producer->getCamera();
//    ui->activeCheckBox->setChecked(cam->getState() == OpenCVCamera::CAM_RUNNING);

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
