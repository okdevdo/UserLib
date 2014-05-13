/******************************************************************************
    
	This file is part of CSources, which is part of UserLib.

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
#ifndef __CSOURCES_H__
#define __CSOURCES_H__

#include "PDEFS.H"

// Generic helper definitions for shared library support
#if defined(OK_COMP_MSC) || defined(__CYGWIN__)
  #define CSOURCES_HELPER_DLL_IMPORT __declspec(dllimport)
  #define CSOURCES_HELPER_DLL_EXPORT __declspec(dllexport)
  #define CSOURCES_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define CSOURCES_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define CSOURCES_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define CSOURCES_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define CSOURCES_HELPER_DLL_IMPORT
    #define CSOURCES_HELPER_DLL_EXPORT
    #define CSOURCES_HELPER_DLL_LOCAL
  #endif
#endif

// Now we use the generic helper definitions above to define CSOURCES_API and CSOURCES_LOCAL.
// CSOURCES_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// CSOURCES_LOCAL is used for non-api symbols.

#ifdef DLL_EXPORT // defined if CSOURCES is compiled as a DLL
  #ifdef CSOURCES_DLL_EXPORTS // defined if we are building the CSOURCES DLL (instead of using it)
    #define CSOURCES_API CSOURCES_HELPER_DLL_EXPORT
  #else
    #define CSOURCES_API CSOURCES_HELPER_DLL_IMPORT
  #endif // CSOURCES_DLL_EXPORTS
  #define CSOURCES_LOCAL CSOURCES_HELPER_DLL_LOCAL
#else // DLL_EXPORT is not defined: this means CSOURCES is a static lib.
  #define CSOURCES_API
  #define CSOURCES_LOCAL
#endif // DLL_EXPORT

#include "TFMEM.H"
#include "STRUTIL.H"

#endif