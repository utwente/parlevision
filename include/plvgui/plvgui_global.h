#ifndef PLVGUI_GLOBAL_H
#define PLVGUI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PLVGUI_LIBRARY)
#  define PLVGUI_EXPORT Q_DECL_EXPORT
#else
#  define PLVGUI_EXPORT Q_DECL_IMPORT
#endif

#endif // PLVGUI_GLOBAL_H
