#include "plvopencvplugin.h"
#include <QtPlugin>
#include <QtDebug>

#include "CameraProducer.h"
//#include "Add.h"
//#include "Sub.h"
//#include "Diff.h"
//#include "DelayImage.h"
#include "DummyProcessor.h"
#include "EdgeDetectorLaplace.h"
//#include "EdgeDetectorCanny.h"
#include "EdgeDetectorSobel.h"
//#include "ImageColorConvert.h"
//#include "ImageCornerHarris.h"
//#include "ImageFlip.h"
#include "ImageSmooth.h"
//#include "ImageThreshold.h"
//#include "Snapshot.h"
//#include "ViolaJonesFaceDetector.h"
//#include "Trigger.h"
//#include "SaveImageToFile.h"
//#include "ImageLoader.h"

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
    plvRegisterPipelineElement<plvopencv::CameraProducer>("plvopencv::CameraProducer", "Camera");

    //processors
//    plvRegisterPipelineElement<plvopencv::Add>("plvopencv::Add", "A add B");
//    plvRegisterPipelineElement<plvopencv::Sub>("plvopencv::Sub", "A sub B");
//    plvRegisterPipelineElement<plvopencv::Diff>("plvopencv::Diff", "AbsDiff(A, B)");
//    plvRegisterPipelineElement<plvopencv::DelayImage>("plvopencv::DelayImage", "Delay");
    plvRegisterPipelineElement<plvopencv::DummyProcessor>("plvopencv::DummyProcessor", "Dummy");
//    plvRegisterPipelineElement<plvopencv::EdgeDetectorCanny>("plvopencv::EdgeDetectorCanny", "Edge Canny");
    plvRegisterPipelineElement<plvopencv::EdgeDetectorLaplace>("plvopencv::EdgeDetectorLaplace", "Edge Laplace");
    plvRegisterPipelineElement<plvopencv::EdgeDetectorSobel>("plvopencv::EdgeDetectorSobel", "Edge Sobel");
//    plvRegisterPipelineElement<plvopencv::ImageColorConvert>("plvopencv::ImageColorConvert", "Color Conversion");
//    plvRegisterPipelineElement<plvopencv::ImageCornerHarris>("plvopencv::ImageCornerHarris", "Harris Corner Detection");
//    plvRegisterPipelineElement<plvopencv::ImageFlip>("plvopencv::ImageFlip", "Flip Image");
    plvRegisterPipelineElement<plvopencv::ImageSmooth>("plvopencv::ImageSmooth", "Smooth Image");
//    plvRegisterPipelineElement<plvopencv::Snapshot>("plvopencv::Snapshot", "Snapshot");
//    plvRegisterPipelineElement<plvopencv::ViolaJonesFaceDetector>("plvopencv::ViolaJonesFaceDetector", "Face Detect V.-J.");
//    plvRegisterPipelineElement<plvopencv::Trigger>("plvopencv::Trigger", "Trigger");
//    plvRegisterPipelineElement<plvopencv::SaveImageToFile>("plvopencv::SaveImageToFile", "Save Image");
//    plvRegisterPipelineElement<plvopencv::ImageLoader>("plvopencv::ImageLoader", "Load Image");
//    plvRegisterPipelineElement<plvopencv::ImageThreshold>("plvopencv::ImageThreshold", "Threshold");

    //consumers
}

Q_EXPORT_PLUGIN2(plv_opencv_plugin, PlvOpenCVPlugin)
