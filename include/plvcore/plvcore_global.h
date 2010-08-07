/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvcore module of ParleVision.
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
