/******************************************************************************
    
	This file is part of ConSources, which is part of UserLib.

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
#ifndef __CONSOURCES_H__
#define __CONSOURCES_H__

#include "PDEFS.H"

// Generic helper definitions for shared library support
#if defined(OK_COMP_MSC) || defined(__CYGWIN__)
  #define CONSOURCES_HELPER_DLL_IMPORT __declspec(dllimport)
  #define CONSOURCES_HELPER_DLL_EXPORT __declspec(dllexport)
  #define CONSOURCES_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define CONSOURCES_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define CONSOURCES_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define CONSOURCES_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define CONSOURCES_HELPER_DLL_IMPORT
    #define CONSOURCES_HELPER_DLL_EXPORT
    #define CONSOURCES_HELPER_DLL_LOCAL
  #endif
#endif

// Now we use the generic helper definitions above to define CONSOURCES_API and CONSOURCES_LOCAL.
// CONSOURCES_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// CONSOURCES_LOCAL is used for non-api symbols.

#ifdef DLL_EXPORT // defined if WINSOURCES is compiled as a DLL
  #ifdef CONSOURCES_DLL_EXPORTS // defined if we are building the WINSOURCES DLL (instead of using it)
    #define CONSOURCES_API CONSOURCES_HELPER_DLL_EXPORT
  #else
    #define CONSOURCES_API CONSOURCES_HELPER_DLL_IMPORT
  #endif // CONSOURCES_DLL_EXPORTS
  #define CONSOURCES_LOCAL CONSOURCES_HELPER_DLL_LOCAL
#else // DLL_EXPORT is not defined: this means WINSOURCES is a static lib.
  #define CONSOURCES_API
  #define CONSOURCES_LOCAL
#endif // DLL_EXPORT

#include "TFMEM.H"
#include "STRUTIL.H"

#include "CppObject.h"
#include "StringLiteral.h"
#include "StringBuffer.h"
#include "ByteBuffer.h"

#ifdef OK_SYS_WINDOWS
typedef WORD color_t;

#define CASE_KEY_ESCAPE    case VK_ESCAPE
#define CASE_KEY_SPACE     case VK_SPACE
#define CASE_KEY_TAB       case VK_TAB
#define CASE_KEY_RETURN    case VK_RETURN
#define CASE_KEY_DELETE    case VK_DELETE
#define CASE_KEY_BACKSPACE case VK_BACK
#define CASE_KEY_HOME      case VK_HOME
#define CASE_KEY_END       case VK_END
#define CASE_KEY_UP        case VK_UP
#define CASE_KEY_DOWN      case VK_DOWN
#define CASE_KEY_LEFT      case VK_LEFT
#define CASE_KEY_RIGHT     case VK_RIGHT
#define CASE_KEY_PAGEUP    case VK_PRIOR
#define CASE_KEY_PAGEDOWN  case VK_NEXT
#define CASE_KEY_F1        case VK_F1
#define CASE_KEY_F2        case VK_F2
#define CASE_KEY_F3        case VK_F3
#define CASE_KEY_F4        case VK_F4
#define CASE_KEY_F5        case VK_F5
#define CASE_KEY_F6        case VK_F6
#define CASE_KEY_F7        case VK_F7
#define CASE_KEY_F8        case VK_F8
#define CASE_KEY_F9        case VK_F9
#define CASE_KEY_F10       case VK_F10
#define CASE_KEY_F11       case VK_F11
#define CASE_KEY_F12       case VK_F12

#endif
#ifdef OK_SYS_UNIX
#ifdef _UNICODE
#include <ncursesw/curses.h>
#else
#include <ncurses/curses.h>
#endif

typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct _COORD {
    sword X;
    sword Y;
} COORD, *PCOORD;

typedef struct _SMALL_RECT {
    sword Left;
    sword Top;
    sword Right;
    sword Bottom;
} SMALL_RECT, *PSMALL_RECT;

typedef struct _CHAR_INFO {
    union {
        sword UnicodeChar;
        char   AsciiChar;
    } Char;
    attr_t Attributes;
} CHAR_INFO, *PCHAR_INFO;

typedef attr_t color_t;

#define CASE_KEY_ESCAPE    case 0x1B
#define CASE_KEY_SPACE     case 0x20
#define CASE_KEY_TAB       case 0x09
#define CASE_KEY_RETURN    case KEY_ENTER
#define CASE_KEY_DELETE    case KEY_DC
#define CASE_KEY_BACKSPACE case KEY_BACKSPACE
#define CASE_KEY_HOME      case KEY_HOME
#define CASE_KEY_END       case KEY_END
#define CASE_KEY_UP        case KEY_UP
#define CASE_KEY_DOWN      case KEY_DOWN
#define CASE_KEY_LEFT      case KEY_LEFT
#define CASE_KEY_RIGHT     case KEY_RIGHT
#define CASE_KEY_PAGEUP    case KEY_PPAGE
#define CASE_KEY_PAGEDOWN  case KEY_NPAGE
#define CASE_KEY_F1        case KEY_F(1)
#define CASE_KEY_F2        case KEY_F(2)
#define CASE_KEY_F3        case KEY_F(3)
#define CASE_KEY_F4        case KEY_F(4)
#define CASE_KEY_F5        case KEY_F(5)
#define CASE_KEY_F6        case KEY_F(6)
#define CASE_KEY_F7        case KEY_F(7)
#define CASE_KEY_F8        case KEY_F(8)
#define CASE_KEY_F9        case KEY_F(9)
#define CASE_KEY_F10       case KEY_F(10)
#define CASE_KEY_F11       case KEY_F(11)
#define CASE_KEY_F12       case KEY_F(12)
#endif

typedef struct _LCOORD {
    sdword X;
    sdword Y;
} LCOORD, *PLCOORD;

#include "Debug.h"

#endif