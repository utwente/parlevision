#ifndef PLVCORE_GLOBAL_H
#define PLVCORE_GLOBAL_H

//#include <QtCore/QtGlobal>
//#if defined(PLVCORE_LIBRARY)
//#  define PLVCORE_EXPORT Q_DECL_EXPORT
//#else
//#  define PLVCORE_EXPORT Q_DECL_IMPORT
//#endif

// Generic helper definitions for shared library support
// Adapted from http://gcc.gnu.org/wiki/Visibility
#if defined PLVCORE_LIBRARY || defined _WIN32 || defined __CYGWIN__
  #define PLVCORE_HELPER_DLL_IMPORT __declspec(dllimport)
  #define PLVCORE_HELPER_DLL_EXPORT __declspec(dllexport)
  #define PLVCORE_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define PLVCORE_HELPER_DLL_IMPORT __attribute__ ((visibility("default")))
    #define PLVCORE_HELPER_DLL_EXPORT __attribute__ ((visibility("default")))
    #define PLVCORE_HELPER_DLL_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define PLVCORE_HELPER_DLL_IMPORT
    #define PLVCORE_HELPER_DLL_EXPORT
    #define PLVCORE_HELPER_DLL_LOCAL
  #endif
#endif

// Now we use the generic helper definitions above to define PLVCORE_API and PLVCORE_LOCAL.
// PLVCORE_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// PLVCORE_LOCAL is used for non-api symbols.

#ifdef PLVCORE_DLL // defined if PLVCORE is compiled as a DLL
  #ifdef PLVCORE_DLL_EXPORTS // defined if we are building the PLVCORE DLL (instead of using it)
    #define PLVCORE_API PLVCORE_HELPER_DLL_EXPORT
  #else
    #define PLVCORE_API PLVCORE_HELPER_DLL_IMPORT
  #endif // PLVCORE_DLL_EXPORTS
  #define PLVCORE_LOCAL PLVCORE_HELPER_DLL_LOCAL
#else // PLVCORE_DLL is not defined: this means PLVCORE is a static lib.
  #define PLVCORE_API
  #define PLVCORE_LOCAL
#endif // PLVCORE_DLL

#endif // PLVCORE_GLOBAL_H
