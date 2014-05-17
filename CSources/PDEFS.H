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
#ifndef __PDEFS_H__
#define __PDEFS_H__
//
// debug environment
//
#ifdef _DEBUG
	#define __DEBUG__
	#define __FILE__LINE__ __FILE__, __LINE__, 
	#define DECL_FILE_LINE const char* file, int line, 
	#define ARGS_FILE_LINE file, line, 
	#define __FILE__LINE__0 __FILE__, __LINE__
	#define __FILE__LINE__0P (__FILE__, __LINE__)
	#define DECL_FILE_LINE0 const char* file, int line
	#define ARGS_FILE_LINE0 file, line
#else
	#define __FILE__LINE__
	#define DECL_FILE_LINE 
	#define ARGS_FILE_LINE
	#define __FILE__LINE__0
	#define __FILE__LINE__0P
	#define DECL_FILE_LINE0
	#define ARGS_FILE_LINE0
	#ifndef NDEBUG
		#define NDEBUG
	#endif
#endif
#ifdef NDEBUG
	#ifdef __DEBUG__
		#error mixed compilation options defined (NDEBUG && _DEBUG)
	#endif
	#undef __DEBUG__
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stddef.h>
#include <stdlib.h>

#ifdef _MSC_VER
#define OK_COMP_MSC 1
#define OK_SYS_WINDOWS 1
#ifdef _WIN32
#define OK_SYS_WINDOWS32 1
#endif
#ifdef _WIN64
#undef OK_SYS_WINDOWS32
#define OK_SYS_WINDOWS64 1
#endif
#ifdef _M_IX86
#define OK_CPU_32BIT 1
#endif
#ifdef _M_X64 
#define OK_CPU_64BIT 1
#endif
#define _CRT_SECURE_NO_WARNINGS 1
#pragma warning( disable : 4996)
#endif
#ifdef __MINGW32_MAJOR_VERSION
#if __MINGW32_MAJOR_VERSION < 4
#undef __MSVCRT_VERSION__
#define __MSVCRT_VERSION__ 0x0710
#endif
#define OK_SYS_WINDOWS 1
#define OK_SYS_WINDOWS32 1
#define OK_CPU_32BIT 1
#define OK_COMP_GNUC 1
#endif
#ifdef __MINGW64_VERSION_MAJOR
#undef OK_CPU_32BIT
#undef OK_SYS_WINDOWS32
#define OK_SYS_WINDOWS 1
#define OK_SYS_WINDOWS64 1
#define OK_CPU_64BIT 1
#define OK_COMP_GNUC 1
#endif
#ifdef __CYGWIN__
#undef OK_SYS_WINDOWS
#undef OK_SYS_WINDOWS32
#undef OK_SYS_WINDOWS64
#define OK_SYS_UNIX 1
#ifdef __x86_64__
#undef OK_CPU_32BIT
#define OK_CPU_64BIT 1
#else
#undef OK_CPU_64BIT
#define OK_CPU_32BIT 1
#endif
#define OK_COMP_GNUC 1
#endif

//
// software plattform
//
#ifdef _CONSOLE
	#define	__CONSOLE__
	#include <winsdkver.h>
	#define _WIN32_WINNT _WIN32_WINNT_VISTA
	#define WINVER _WIN32_WINNT
	#define NTDDI_VERSION NTDDI_VISTA
	#include <SDKDDKVer.h>
	#define WIN32_LEAN_AND_MEAN
	#define NOGDI
	#define _WINSOCKAPI_
	#include <windows.h>
#endif
#ifdef _MINGW64_CONSOLE
	#define	__CONSOLE__
	#undef _WIN32_WINNT
	#define _WIN32_WINNT_VISTA 0x0600
	#define _WIN32_WINNT _WIN32_WINNT_VISTA
	#undef WINVER
	#define WINVER _WIN32_WINNT
	#undef NTDDI_VERSION
	#define NTDDI_VISTA 0x06000000
	#define NTDDI_VERSION NTDDI_VISTA
	#define WIN32_LEAN_AND_MEAN
//	#define NOGDI
	#include <winsock2.h>
	#include <windows.h>
	#ifndef _WIN32
		#define _WIN32
	#endif
	#ifndef _WIN64
		#define _WIN64
	#endif
#endif
#ifdef _MINGW_CONSOLE
	#define	__CONSOLE__
	#undef _WIN32_WINNT
    #define _WIN32_WINNT_WINXP 0x0501
	#define _WIN32_WINNT _WIN32_WINNT_WINXP
	#undef WINVER
	#define WINVER _WIN32_WINNT
	#undef NTDDI_VERSION
    #define NTDDI_WINXPSP2 0x05010200
	#define NTDDI_VERSION NTDDI_WINXPSP2
	#undef __MSVCRT_VERSION__
	#define __MSVCRT_VERSION__ 0x0710
	#define WIN32_LEAN_AND_MEAN
	#define NOGDI
	#define __USE_W32_SOCKETS
	#include <windows.h>
	#ifndef _WIN32
		#define _WIN32
	#endif
#endif
#ifdef _WINDOWS
	#ifdef __CONSOLE__
		#error mixed compilation options defined (_CONSOLE && _WINDOWS)
	#endif
	#define	__WINDOWS__
	#include <winsdkver.h>
	#define _WIN32_WINNT _WIN32_WINNT_VISTA
	#include <SDKDDKVer.h>
	#define WIN32_LEAN_AND_MEAN
	#define _WINSOCKAPI_
	#include <windows.h>
#endif
#ifdef _MT
	#define __MT__
#endif

// Scope Settings for debugging and testing purposes
#undef __LOCAL__
#ifdef __DEBUG__
	#define __LOCAL__
#else
	#define __LOCAL__ static
#endif
#ifdef __DEBUG__
	#define __PUBLIC public:
	#define __PROTECTED public:
	#define __PRIVATE public:
#else
	#define __PUBLIC public:
	#define __PROTECTED protected:
	#define __PRIVATE private:
#endif

// Unicode Settings
#ifdef UNICODE
	#ifndef _UNICODE
		#define _UNICODE
	#endif
#endif
#ifdef OK_SYS_WINDOWS
	#include <tchar.h>
#endif
#ifdef OK_SYS_UNIX
	#ifdef _UNICODE
		#include <wchar.h>
        #define _TCHAR wchar_t
		#define TCHAR _TCHAR
		#define _T(x) L ## x
		#define _tprintf wprintf
	#else
        #define _TCHAR char
		#define TCHAR _TCHAR
		#define _T(x) x
		#define _tprintf printf
	#endif
#endif

// Assert Settings
#ifdef OK_COMP_MSC
#ifdef __DEBUG__
#include <crtdbg.h>
#else
#define _ASSERTE __noop()
#endif
#endif
#ifndef NDEBUG
	#include <assert.h>
#else
#ifdef OK_COMP_MSC
	#define assert(s) __noop()
#else
    #define assert(s) ((void)0)
#endif
#endif
#ifndef OK_COMP_MSC
#define _ASSERTE assert
#endif

// Includes
#include <errno.h>
#ifndef _ERRNO_T_DEFINED
	#define _ERRNO_T_DEFINED
	typedef int errno_t;
#endif
#include <stdarg.h>
#include "pdeftype.h"
#endif