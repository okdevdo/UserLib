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
#pragma once

#include "WinSources.h"
#include "WinException.h"
#include "SecurityFile.h"

#ifdef OK_SYS_WINDOWS
#undef GetFileSecurity
class WINSOURCES_API CSecurityContext: public CCppObject
{
public:
	CSecurityContext(void);
	virtual ~CSecurityContext(void);

	static CSecurityContext* instance();
	static void free_instance();

	void GetFileSecurity(mode_t mode, Ref(SECURITY_ATTRIBUTES) sa);
	void GetDirectorySecurity(mode_t mode, Ref(SECURITY_ATTRIBUTES) sa);
	void FreeSecurity(SECURITY_ATTRIBUTES sa);

	void SetProcessPrivilege(
		LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
		BOOL bEnablePrivilege);   // to enable or disable privilege

private:
	void Initialize();
	void Cleanup();

	BOOL SetPrivilege(
		HANDLE hToken,          // access token handle
		LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
		BOOL bEnablePrivilege   // to enable or disable privilege
    );

	HANDLE hToken;
	PSID pEveryoneSID;
	PSID pAdminSID;
	PSID pNullSID;
	PSID cr_ownerSID;
	PSID cr_groupSID;
	PSID ownerSID;
	Pointer ownerPtr;
	PSID primaryGroupSID;
	Pointer primaryGroupPtr;
	
	static CSecurityContext* m_instance;
};

DECL_WINEXCEPTION(WINSOURCES_API, CSecurityContextException, CWinException)
#define CSecurityContext_FreeInstance CSecurityContext::free_instance();
#else
#define CSecurityContext_FreeInstance
#endif