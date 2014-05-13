/******************************************************************************
    
	This file is part of XSetup, which is part of UserLib.

    Copyright (C) 1995-2014  Oliver Kreis (okdev10@arcor.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by 
	the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/
#pragma once

#include "PDEFS.H"

#include "CppObject.h"
#include "StringLiteral.h"
#include "StringBuffer.h"
#include "ByteBuffer.h"
#include "DataVector.h"

#ifdef OK_COMP_MSC
#ifdef __DEBUG__
#include <conio.h>
#endif
#endif

#include <iostream>
#include <iomanip>

#ifdef _UNICODE
#define COUT std::wcout
#define CERR std::wcerr
#define OSTREAM std::wostream
#else
#define COUT std::cout
#define CERR std::cerr
#define OSTREAM std::ostream
#endif
using std::endl;

#define FLAG_NoRequiredBy      0x0001
#define FLAG_Installed		   0x0002
#define FLAG_NotInstalled      0x0004
#define FLAG_InstallPackages   0x0008
#define FLAG_UninstallPackages 0x0010
#define FLAG_OnlyPrint         0x0020
#define FLAG_FindCycles        0x0040
#define FLAG_Update            0x0080

#define CYGWIN_SETUP_DIR _T("/etc/setup/")
#define CYGWIN_PREREMOVE_DIR _T("/etc/preremove/")
#define CYGWIN_POSTINSTALL_DIR _T("/etc/postinstall/")
#define CYGWIN_TMP_DIR _T("/tmp/")

void ScanSetupIni(CStringLiteral pInstallDir, CStringLiteral psSetupIniScanDir, CDataVectorT<mbchar>& psSetupIniFiles,
	CDataVectorT<CStringBuffer>& psSearchPackage, CDataVectorT<mbchar>& psSearchKey, WUInt flags, WInt iVerbose);
