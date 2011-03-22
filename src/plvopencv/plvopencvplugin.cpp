#include "plvopencvplugin.h"
#include <QtPlugin>
#include <QtDebug>

#include "CameraProducer.h"
#include "Add.h"
#include "Sub.h"
#include "Diff.h"
#include "DelayImage.h"
#include "ExampleProcessor.h"
#include "EdgeDetectorLaplace.h"
#include "EdgeDetectorCanny.h"
#include "EdgeDetectorSobel.h"
#include "ImageColorConvert.h"
#include "ImageCornerHarris.h"
#include "ImageFlip.h"
#include "GaussianSmooth.h"
#include "ImageThreshold.h"
#include "CropSubImage.h"

#include "Snapshot.h"
#include "ViolaJonesFaceDetector.h"
#include "Trigger.h"
#include "SaveImageToFile.h"
//#include "ImageLoader.h"
#include "PixelSum.h"
#include "Xor.h"
#include "Multiply.h"
#include "Split.h"
#include "ImageProducer.h"
#include "ImageDirectoryProducer.h"

PlvOpenCVPlugin::PlvOpenCVPlugin()
{
}

PlvOpenCVPlugin::~PlvOpenCVPlugin()
{
}

void PlvOpenCVPlugin::onLoad()
{
    qDebug() << "Loading PlvOpenCVPlugin plugin processors";

    //producers
    plvRegisterPipelineElement<plvopencv::CameraProducer>();

    //processors
    plvRegisterPipelineElement<plvopencv::ExampleProcessor>();
    plvRegisterPipelineElement<plvopencv::EdgeDetectorLaplace>();
    plvRegisterPipelineElement<plvopencv::EdgeDetectorSobel>();
    plvRegisterPipelineElement<plvopencv::GaussianSmooth>();
    plvRegisterPipelineElement<plvopencv::ImageFlip>();
    plvRegisterPipelineElement<plvopencv::Add>();
    plvRegisterPipelineElement<plvopencv::Sub>();
    plvRegisterPipelineElement<plvopencv::Diff>();

    plvRegisterPipelineElement<plvopencv::DelayImage>();
    plvRegisterPipelineElement<plvopencv::EdgeDetectorCanny>();
    plvRegisterPipelineElement<plvopencv::ImageColorConvert>();
    plvRegisterPipelineElement<plvopencv::ImageCornerHarris>();

    plvRegisterPipelineElement<plvopencv::PixelSum>();

    plvRegisterPipelineElement<plvopencv::Snapshot>();
    plvRegisterPipelineElement<plvopencv::ViolaJonesFaceDetector>();
    plvRegisterPipelineElement<plvopencv::CropSubImage>();
    plvRegisterPipelineElement<plvopencv::Trigger>();
    plvRegisterPipelineElement<plvopencv::SaveImageToFile>();
    plvRegisterPipelineElement<plvopencv::ImageThreshold>();

    //Producers
    plvRegisterPipelineElement<plvopencv::ImageProducer>();
    plvRegisterPipelineElement<plvopencv::ImageDirectoryProducer>();

    //Processors
    plvRegisterPipelineElement<plvopencv::Xor>();
    plvRegisterPipelineElement<plvopencv::Multiply>();
    plvRegisterPipelineElement<plvopencv::Split>();

    //consumers
}
Q_EXPORT_PLUGIN2(plv_opencv_plugin, PlvOpenCVPlugin)

