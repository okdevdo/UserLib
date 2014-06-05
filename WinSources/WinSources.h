/******************************************************************************
    
	This file is part of WinSources, which is part of UserLib.

    Copyright (C) 1995-2014  Oliver Kreis (okdev10@arcor.de)

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published 
	by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/
#ifndef __WINSOURCES_H__
#define __WINSOURCES_H__

#include "pdefs.h"

// Generic helper definitions for shared library support
#if defined(OK_COMP_MSC) || defined(__CYGWIN__)
  #define WINSOURCES_HELPER_DLL_IMPORT __declspec(dllimport)
  #define WINSOURCES_HELPER_DLL_EXPORT __declspec(dllexport)
  #define WINSOURCES_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define WINSOURCES_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define WINSOURCES_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define WINSOURCES_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define WINSOURCES_HELPER_DLL_IMPORT
    #define WINSOURCES_HELPER_DLL_EXPORT
    #define WINSOURCES_HELPER_DLL_LOCAL
  #endif
#endif

// Now we use the generic helper definitions above to define WINSOURCES_API and WINSOURCES_LOCAL.
// WINSOURCES_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// WINSOURCES_LOCAL is used for non-api symbols.

#ifdef DLL_EXPORT // defined if WINSOURCES is compiled as a DLL
  #ifdef WINSOURCES_DLL_EXPORTS // defined if we are building the WINSOURCES DLL (instead of using it)
    #define WINSOURCES_API WINSOURCES_HELPER_DLL_EXPORT
  #else
    #define WINSOURCES_API WINSOURCES_HELPER_DLL_IMPORT
  #endif // WINSOURCES_DLL_EXPORTS
  #define WINSOURCES_LOCAL WINSOURCES_HELPER_DLL_LOCAL
#else // DLL_EXPORT is not defined: this means WINSOURCES is a static lib.
  #define WINSOURCES_API
  #define WINSOURCES_LOCAL
#endif // DLL_EXPORT

#include "tfmem.h"
#include "strutil.h"

#include "CppObject.h"
#include "StringLiteral.h"
#include "StringBuffer.h"
#include "ByteBuffer.h"

#endif