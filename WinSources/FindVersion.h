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
/*
* Find Windows version using bisection method and VerifyVersionInfo.
*
* Author:   M1xA, www.m1xa.com
* Date:     2013.07.07
* Licence:  MIT
* Version:  1.0
*
* API:
*
* BOOL GetVersionExEx(OSVERSIONINFOEX * osVer);
* Returns: 0 if fails.
*
* Supported OS: Windows 2000 .. Windows 8.1.
*/
#pragma once
#ifndef __FIND_VERSION__
#define __FIND_VERSION__
#include "WinSources.h"

	/*
	Operating system		Version number	dwMajorVersion	dwMinorVersion	Other

	Windows 8.1				6.3*			6				3				OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
	Windows Server 2012 R2	6.3*			6				3				OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
	Windows 8				6.2				6				2				OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
	Windows Server 2012		6.2				6				2				OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
	Windows 7				6.1				6				1				OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
	Windows Server 2008 R2	6.1				6				1				OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
	Windows Server 2008		6.0				6				0				OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
	Windows Vista			6.0				6				0				OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
	Windows Server 2003 R2	5.2				5				2				GetSystemMetrics(SM_SERVERR2) != 0
	Windows Home Server		5.2				5				2				OSVERSIONINFOEX.wSuiteMask & VER_SUITE_WH_SERVER
	Windows Server 2003		5.2				5				2				GetSystemMetrics(SM_SERVERR2) == 0
	Windows XP Professional
			x64 Edition		5.2				5				2				(OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION) && (SYSTEM_INFO.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
	Windows XP				5.1				5				1				Not applicable
	Windows 2000			5.0				5				0				Not applicable
	*/

	BOOL __stdcall GetVersionEx2(OSVERSIONINFOEX * osVer);

#endif