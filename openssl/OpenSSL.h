/******************************************************************************
    
	This file is part of openssl, which is part of UserLib.

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
#ifndef __OPENSSL_H__
#define __OPENSSL_H__

#include "PDEFS.H"

// Generic helper definitions for shared library support
#if defined(OK_COMP_MSC) || defined(__CYGWIN__)
  #define OPENSSL_HELPER_DLL_IMPORT __declspec(dllimport)
  #define OPENSSL_HELPER_DLL_EXPORT __declspec(dllexport)
  #define OPENSSL_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define OPENSSL_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define OPENSSL_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define OPENSSL_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define OPENSSL_HELPER_DLL_IMPORT
    #define OPENSSL_HELPER_DLL_EXPORT
    #define OPENSSL_HELPER_DLL_LOCAL
  #endif
#endif

// Now we use the generic helper definitions above to define OPENSSL_API and OPENSSL_LOCAL.
// OPENSSL_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// OPENSSL_LOCAL is used for non-api symbols.

#ifdef DLL_EXPORT // defined if OPENSSL is compiled as a DLL
  #ifdef OPENSSL_DLL_EXPORTS // defined if we are building the OPENSSL DLL (instead of using it)
    #define OPENSSL_API OPENSSL_HELPER_DLL_EXPORT
  #else
    #define OPENSSL_API OPENSSL_HELPER_DLL_IMPORT
  #endif // OPENSSL_DLL_EXPORTS
  #define OPENSSL_LOCAL OPENSSL_HELPER_DLL_LOCAL
#else // DLL_EXPORT is not defined: this means OPENSSL is a static lib.
  #define OPENSSL_API
  #define OPENSSL_LOCAL
#endif // DLL_EXPORT

#include "CppObject.h"
#include "StringLiteral.h"
#include "StringBuffer.h"
#include "ByteBuffer.h"
#include "DataVector.h"
#include "DataHashtable.h"

#include <stdio.h>
#include "OpenSSLClass.h"
#include "MISC.h"

#endif