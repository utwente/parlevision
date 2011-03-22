#ifndef PLV_OPENCV_PLUGIN_GLOBAL_H
#define PLV_OPENCV_PLUGIN_GLOBAL_H

#include <QtCore/qglobal.h>
#include <plvcore/plvglobal.h>

#if defined(PLV_OPENCV_PLUGIN_LIBRARY)
#  define PLVOPENCV_EXPORT Q_DECL_EXPORT
#else
#  define PLVOPENCV_EXPORT Q_DECL_IMPORT
#endif

#endif // PLV_OPENCV_PLUGIN_GLOBAL_H
