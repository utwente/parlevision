#ifndef PLVCORE_GLOBAL_H
#define PLVCORE_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined(PLVCORE_LIBRARY)
#  define PLVCORE_EXPORT Q_DECL_EXPORT
#else
#  define PLVCORE_EXPORT Q_DECL_IMPORT
#endif

#endif // PLVCORE_GLOBAL_H
