#ifndef TCP_SERVER_PLUGIN_GLOBAL_H
#define TCP_SERVER_PLUGIN_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QDebug>

#if defined(TCP_SERVER_PLUGIN_LIBRARY)
#  define TCP_SERVER_EXPORT Q_DECL_EXPORT
#else
#  define TCP_SERVER_EXPORT Q_DECL_IMPORT
#endif

#endif // TCP_SERVER_PLUGIN_GLOBAL_H
