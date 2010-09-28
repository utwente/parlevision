/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvgui module of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

#include "CameraConfigFormBuilder.h"
#include "ui_cameraconfigform.h"

#include <plvcore/PipelineElement.h>
#include <plvopencv/CameraProducer.h>

//#include "OpenCVCamera.h"

using namespace plvopencv;
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
