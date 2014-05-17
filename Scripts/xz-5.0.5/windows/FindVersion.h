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
#ifndef __FIND_VERSION__
#define __FIND_VERSION__

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

BOOL GetVersionEx2(OSVERSIONINFOEX * osVer);

#endif
