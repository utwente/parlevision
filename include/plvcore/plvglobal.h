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

#include <QMetaType>

enum PlvErrorType {
    PlvNoError,
    PlvNonFatalError,
    PlvPipelineInitError,   /** failed to init or free resources during pipeline initialisation */
    PlvPipelineRuntimeError,/** something went wrong during pipeline execution */
    PlvFatalError           /** a fatal error will close the application */
};
Q_DECLARE_METATYPE( PlvErrorType );

enum PlvMessageType {
    PlvDebugMessage,
    PlvNotifyMessage,
    PlvWarningMessage,
    PlvErrorMessage
};
Q_DECLARE_METATYPE( PlvMessageType );

// disbale warnings specific to Microsoft compilers
#if defined _WIN32
// C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#pragma warning( disable : 4290 )
#endif

// Generic helper definitions for shared library support
// Adapted from http://gcc.gnu.org/wiki/Visibility
#if defined _WIN32 || defined __CYGWIN__
  #define PLV_DECL_IMPORT __declspec(dllimport)
  #define PLV_DECL_EXPORT __declspec(dllexport)
  #define PLV_DECL_LOCAL
#else
  #if __GNUC__ >= 4
    #define PLV_DECL_IMPORT __attribute__ ((visibility("default")))
    #define PLV_DECL_EXPORT __attribute__ ((visibility("default")))
    #define PLV_DECL_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define PLV_DECL_IMPORT
    #define PLV_DECL_EXPORT
    #define PLV_DECL_LOCAL
  #endif
#endif

// Now we use the generic helper definitions above to define PLVCORE_EXPORT and PLVCORE_LOCAL.
// PLVCORE_EXPORT is used for the public API symbols. It either DLL imports or
// DLL exports (or does nothing for static build)
// PLVCORE_LOCAL is used for non-api symbols.
#ifdef PLV_SHARED_LIBRARY // defined if PLV is compiled as a DLL or shared object
  #ifdef PLV_DLL_EXPORTS // defined if we are building the PLVx DLLs (instead of using it)
    #define PLVCORE_EXPORT PLV_DECL_EXPORT
  #else
    #define PLVCORE_EXPORT PLV_DECL_IMPORT
  #endif // PLV_DLL_EXPORTS
  #define PLVCORE_LOCAL PLV_DECL_LOCAL
#else // PLV_SHARED_LIBRARY is not defined: this means PLV is a static lib.
  #define PLVCORE_EXPORT
  #define PLVCORE_LOCAL
#endif // PLV_SHARED_LIBRARY

#endif // PLVCORE_GLOBAL_H
