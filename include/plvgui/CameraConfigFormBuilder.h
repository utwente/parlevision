#ifndef CAMERACONFIGFORMBUILDER_H
#define CAMERACONFIGFORMBUILDER_H

#include "ElementConfigFormBuilder.h"
#include <QWidget>
#include "RefPtr.h"

namespace Ui
{
    class CameraConfigForm;
}

namespace plv
{
    class CameraProducer;
}

namespace plvgui
{
    class CameraForm : public QWidget
    {
    public:
        CameraForm(CameraProducer* producer, QWidget* parent=0);
    private:
        Ui::CameraConfigForm* ui;
        RefPtr<CameraProducer> producer;
    };

    class CameraConfigFormBuilder
    {
    public:
        QWidget* buildForm(PipelineElement* element, QWidget* parent=0);
    };
}

#endif // CAMERACONFIGFORMBUILDER_H
