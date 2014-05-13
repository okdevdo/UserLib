/******************************************************************************
    
	This file is part of CppSources, which is part of UserLib.

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
#ifndef __CPPSOURCES_H__
#define __CPPSOURCES_H__

#include "PDEFS.H"

// Generic helper definitions for shared library support
#if defined(OK_COMP_MSC) || defined(__CYGWIN__)
  #define CPPSOURCES_HELPER_DLL_IMPORT __declspec(dllimport)
  #define CPPSOURCES_HELPER_DLL_EXPORT __declspec(dllexport)
  #define CPPSOURCES_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define CPPSOURCES_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define CPPSOURCES_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define CPPSOURCES_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define CPPSOURCES_HELPER_DLL_IMPORT
    #define CPPSOURCES_HELPER_DLL_EXPORT
    #define CPPSOURCES_HELPER_DLL_LOCAL
  #endif
#endif

// Now we use the generic helper definitions above to define CPPSOURCES_API and CPPSOURCES_LOCAL.
// CPPSOURCES_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// CPPSOURCES_LOCAL is used for non-api symbols.

#ifdef DLL_EXPORT // defined if CPPSOURCES is compiled as a DLL
  #ifdef CPPSOURCES_DLL_EXPORTS // defined if we are building the CPPSOURCES DLL (instead of using it)
    #define CPPSOURCES_API CPPSOURCES_HELPER_DLL_EXPORT
  #else
    #define CPPSOURCES_API CPPSOURCES_HELPER_DLL_IMPORT
  #endif // CPPSOURCES_DLL_EXPORTS
  #define CPPSOURCES_LOCAL CPPSOURCES_HELPER_DLL_LOCAL
#else // DLL_EXPORT is not defined: this means CPPSOURCES is a static lib.
  #define CPPSOURCES_API
  #define CPPSOURCES_LOCAL
#endif // DLL_EXPORT

#include "TFMEM.H"
#include "STRUTIL.H"

#include "CppObject.h"
#include "StringLiteral.h"
#include "StringBuffer.h"
#include "ByteBuffer.h"

#endif