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
#include "WS_PCH.H"
#include "SecurityContext.h"

#include <aclapi.h>

IMPL_WINEXCEPTION(CSecurityContextException, CWinException)

CSecurityContext* CSecurityContext::m_instance = NULL;

CSecurityContext::CSecurityContext(void):
    hToken(INVALID_HANDLE_VALUE),
	pEveryoneSID(NULL),
	pAdminSID(NULL),
	pNullSID(NULL),
	cr_ownerSID(NULL),
	cr_groupSID(NULL),
	ownerSID(NULL),
	ownerPtr(NULL),
	primaryGroupSID(NULL),
	primaryGroupPtr(NULL)
{
}

CSecurityContext::~CSecurityContext(void)
{
}

CSecurityContext* CSecurityContext::instance()
{
	if ( PtrCheck(m_instance) )
	{
		m_instance = OK_NEW_OPERATOR CSecurityContext();
		m_instance->Initialize();
	}
	return m_instance;
}

void CSecurityContext::free_instance()
{
	if ( PtrCheck(m_instance) )
		return;
	m_instance->Cleanup();
	delete m_instance;
	m_instance = NULL;
}

void CSecurityContext::Initialize()
{
	DWORD size;
	TOKEN_OWNER to;

	if ( !OpenProcessToken(GetCurrentProcess(), TOKEN_READ | TOKEN_ADJUST_DEFAULT | TOKEN_ADJUST_PRIVILEGES, &hToken) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::Initialize"), CWinException::WinExtError);
	if ( SetPrivilege(hToken, SE_BACKUP_NAME, TRUE) )
	{
		if ( !SetPrivilege(hToken, SE_RESTORE_NAME, TRUE) )
			SetPrivilege(hToken, SE_BACKUP_NAME, FALSE);
	}
	if ( (!GetTokenInformation(hToken, TokenUser, NULL, 0, &size)) && (GetLastError() != ERROR_INSUFFICIENT_BUFFER) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::Initialize"), CWinException::WinExtError);
	if ( PtrCheck(ownerPtr = LocalAlloc(LPTR, size)) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::Initialize"), CWinException::WinExtError);
	if ( !GetTokenInformation(hToken, TokenUser, ownerPtr, size, &size) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::Initialize"), CWinException::WinExtError);
	ownerSID = CastAnyPtr(TOKEN_USER, ownerPtr)->User.Sid;
	to.Owner = ownerSID;
	if ( !SetTokenInformation(hToken, TokenOwner, &to, sizeof(to)) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::Initialize"), CWinException::WinExtError);
	if ( (!GetTokenInformation(hToken, TokenPrimaryGroup, NULL, 0, &size)) && (GetLastError() != ERROR_INSUFFICIENT_BUFFER) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::Initialize"), CWinException::WinExtError);
	if ( PtrCheck(primaryGroupPtr = LocalAlloc(LPTR, size)) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::Initialize"), CWinException::WinExtError);
	if ( !GetTokenInformation(hToken, TokenPrimaryGroup, primaryGroupPtr, size, &size) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::Initialize"), CWinException::WinExtError);
	primaryGroupSID = CastAnyPtr(TOKEN_PRIMARY_GROUP, primaryGroupPtr)->PrimaryGroup;

	SID_IDENTIFIER_AUTHORITY SIDAuthWorld = { SECURITY_WORLD_SID_AUTHORITY };
	SID_IDENTIFIER_AUTHORITY SIDAuthNT = { SECURITY_NT_AUTHORITY };
	SID_IDENTIFIER_AUTHORITY n_sid_auth = { SECURITY_NULL_SID_AUTHORITY };
	SID_IDENTIFIER_AUTHORITY c_sid_auth = { SECURITY_CREATOR_SID_AUTHORITY };

	if( !AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pEveryoneSID) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::Initialize"), CWinException::WinExtError);
	if( !AllocateAndInitializeSid(&SIDAuthNT, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pAdminSID) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::Initialize"), CWinException::WinExtError);
	if ( !AllocateAndInitializeSid(&n_sid_auth, 1, SECURITY_NULL_RID, 0, 0, 0, 0, 0, 0, 0, &pNullSID) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::Initialize"), CWinException::WinExtError);
	if ( !AllocateAndInitializeSid(&c_sid_auth, 1, SECURITY_CREATOR_OWNER_RID, 0, 0, 0, 0, 0, 0, 0, &cr_ownerSID) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::Initialize"), CWinException::WinExtError);
	if ( !AllocateAndInitializeSid(&c_sid_auth, 1, SECURITY_CREATOR_GROUP_RID, 0, 0, 0, 0, 0, 0, 0, &cr_groupSID) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::Initialize"), CWinException::WinExtError);
}

void CSecurityContext::SetProcessPrivilege(
	LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
	BOOL bEnablePrivilege)   // to enable or disable privilege
{
	if (!SetPrivilege(hToken, lpszPrivilege, bEnablePrivilege))
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::SetProcessPrivilege"), CWinException::WinExtError);
}

BOOL CSecurityContext::SetPrivilege(
	HANDLE hToken,          // access token handle
	LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
	BOOL bEnablePrivilege   // to enable or disable privilege
	) 
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if ( !LookupPrivilegeValue( 
		NULL,            // lookup privilege on local system
		lpszPrivilege,   // privilege to lookup 
		&luid ) )        // receives LUID of privilege
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::SetPrivilege"), CWinException::WinExtError);

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	if ( !AdjustTokenPrivileges(
		hToken,                     // HANDLE TokenHandle
		FALSE,                      // BOOL DisableAllPrivileges,
		&tp,                        // PTOKEN_PRIVILEGES NewState,
		sizeof(TOKEN_PRIVILEGES),   // DWORD BufferLength,
		(PTOKEN_PRIVILEGES) NULL,   // PTOKEN_PRIVILEGES PreviousState,
		(PDWORD) NULL) )            // PDWORD ReturnLength
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::SetPrivilege"), CWinException::WinExtError);

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
		return FALSE;
	return TRUE;
}

void CSecurityContext::Cleanup()
{
	if (cr_groupSID) 
		FreeSid(cr_groupSID);
	cr_groupSID = NULL;
	if (cr_ownerSID) 
		FreeSid(cr_ownerSID);
	cr_ownerSID = NULL;
	if (pNullSID) 
		FreeSid(pNullSID);
	pNullSID = NULL;
	if (pAdminSID) 
		FreeSid(pAdminSID);
	pAdminSID = NULL;
	if (pEveryoneSID) 
		FreeSid(pEveryoneSID);
	pEveryoneSID = NULL;
	if (primaryGroupPtr) 
		LocalFree(primaryGroupPtr);
	primaryGroupPtr = NULL;
	if (ownerPtr) 
		LocalFree(ownerPtr);
	ownerPtr = NULL;
	if ( hToken != INVALID_HANDLE_VALUE )
		CloseHandle(hToken);
	hToken = INVALID_HANDLE_VALUE;
}

void CSecurityContext::GetFileSecurity(mode_t mode, Ref(SECURITY_ATTRIBUTES) sa)
{
	DWORD u_attribute, g_attribute, o_attribute;
	DWORD dwRes;
	PACL pACL;
	PSECURITY_DESCRIPTOR pSD;
	EXPLICIT_ACCESS ea[3];

	if ( mode == 0 )
		mode = 0644;
	u_attribute = STANDARD_RIGHTS_ALL | FILE_GENERIC_READ | FILE_GENERIC_WRITE;
	if (mode & 0100) // S_IXUSR
		u_attribute |= FILE_GENERIC_EXECUTE;
	if ((mode & 0300) == 0300) // S_IWUSR | S_IXUSR
		u_attribute |= FILE_DELETE_CHILD;
	g_attribute = STANDARD_RIGHTS_READ | FILE_READ_ATTRIBUTES;
	if (mode & 0040) // S_IRGRP
		g_attribute |= FILE_GENERIC_READ;
	if (mode & 0020) // S_IWGRP
		g_attribute |= FILE_GENERIC_WRITE;
	if (mode & 0010) // S_IXGRP
		g_attribute |= FILE_GENERIC_EXECUTE;
	if ((mode & 01030) == 00030) // S_IWGRP | S_IXGRP, !S_ISVTX
		g_attribute |= FILE_DELETE_CHILD;
	o_attribute = STANDARD_RIGHTS_READ | FILE_READ_ATTRIBUTES;
	if (mode & 0004) // S_IROTH
		o_attribute |= FILE_GENERIC_READ;
	if (mode & 0002) // S_IWOTH
		o_attribute |= FILE_GENERIC_WRITE;
	if (mode & 0001) // S_IXOTH
		o_attribute |= FILE_GENERIC_EXECUTE;
	if ((mode & 01003) == 00003) // S_IWOTH | S_IXOTH, !S_ISVTX
		o_attribute |= FILE_DELETE_CHILD;

	::ZeroMemory(&ea, 3 * sizeof(EXPLICIT_ACCESS));
	ea[0].grfAccessPermissions = o_attribute;
	ea[0].grfAccessMode = SET_ACCESS;
	ea[0].grfInheritance = NO_INHERITANCE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[0].Trustee.ptstrName  = (LPTSTR) pEveryoneSID;

	ea[1].grfAccessPermissions = u_attribute;
	ea[1].grfAccessMode = SET_ACCESS;
	ea[1].grfInheritance = NO_INHERITANCE;
	ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	//ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	//ea[1].Trustee.ptstrName  = (LPTSTR) pAdminSID;
	ea[1].Trustee.TrusteeType = TRUSTEE_IS_USER;
	ea[1].Trustee.ptstrName  = (LPTSTR) ownerSID;

	ea[2].grfAccessPermissions = g_attribute;
	ea[2].grfAccessMode = SET_ACCESS;
	ea[2].grfInheritance = NO_INHERITANCE;
	ea[2].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	//ea[2].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	//ea[2].Trustee.ptstrName  = (LPTSTR) pNullSID;
	ea[2].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[2].Trustee.ptstrName  = (LPTSTR) primaryGroupSID;

	sa.nLength = sizeof (SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = FALSE;

	pACL = NULL;
	dwRes = SetEntriesInAcl(3, ea, NULL, &pACL);
	if (ERROR_SUCCESS != dwRes) 
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::GetFileSecurity"), CWinException::WinExtError);
	if ( PtrCheck(pSD = CastAnyPtr(SECURITY_DESCRIPTOR, LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH))) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::GetFileSecurity"), CWinException::WinExtError);
	if ( !InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::GetFileSecurity"), CWinException::WinExtError);
	if ( !SetSecurityDescriptorDacl(pSD, TRUE, pACL, FALSE) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::GetFileSecurity"), CWinException::WinExtError);
	sa.lpSecurityDescriptor = pSD;
}

void CSecurityContext::GetDirectorySecurity(mode_t mode, Ref(SECURITY_ATTRIBUTES) sa)
{
	DWORD u_attribute, g_attribute, o_attribute;
	DWORD dwRes;
	PACL pACL;
	PSECURITY_DESCRIPTOR pSD;
	EXPLICIT_ACCESS ea[5];

	if ( mode == 0 )
		mode = 0755;
	u_attribute = STANDARD_RIGHTS_ALL | FILE_GENERIC_READ | FILE_GENERIC_WRITE;
	if (mode & 0100) // S_IXUSR
		u_attribute |= FILE_GENERIC_EXECUTE;
	if ((mode & 0300) == 0300) // S_IWUSR | S_IXUSR
		u_attribute |= FILE_DELETE_CHILD;
	g_attribute = STANDARD_RIGHTS_READ | FILE_READ_ATTRIBUTES;
	if (mode & 0040) // S_IRGRP
		g_attribute |= FILE_GENERIC_READ;
	if (mode & 0020) // S_IWGRP
		g_attribute |= FILE_GENERIC_WRITE;
	if (mode & 0010) // S_IXGRP
		g_attribute |= FILE_GENERIC_EXECUTE;
	if ((mode & 01030) == 00030) // S_IWGRP | S_IXGRP, !S_ISVTX
		g_attribute |= FILE_DELETE_CHILD;
	o_attribute = STANDARD_RIGHTS_READ | FILE_READ_ATTRIBUTES;
	if (mode & 0004) // S_IROTH
		o_attribute |= FILE_GENERIC_READ;
	if (mode & 0002) // S_IWOTH
		o_attribute |= FILE_GENERIC_WRITE;
	if (mode & 0001) // S_IXOTH
		o_attribute |= FILE_GENERIC_EXECUTE;
	if ((mode & 01003) == 00003) // S_IWOTH | S_IXOTH, !S_ISVTX
		o_attribute |= FILE_DELETE_CHILD;
	if (mode & 01000) // S_ISVTX
	{
		g_attribute = STANDARD_RIGHTS_READ | FILE_READ_ATTRIBUTES;
		if (mode & 0040) // S_IRGRP
			g_attribute |= FILE_GENERIC_READ;
		if (mode & 0010) // S_IXGRP
			g_attribute |= FILE_GENERIC_EXECUTE;
		o_attribute = STANDARD_RIGHTS_READ | FILE_READ_ATTRIBUTES;
		if (mode & 0004) // S_IROTH
			o_attribute |= FILE_GENERIC_READ;
		if (mode & 0001) // S_IXOTH
			o_attribute |= FILE_GENERIC_EXECUTE;
	}

	::ZeroMemory(&ea, 5 * sizeof(EXPLICIT_ACCESS));
	ea[0].grfAccessPermissions = o_attribute;
	ea[0].grfAccessMode = SET_ACCESS;
	ea[0].grfInheritance = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[0].Trustee.ptstrName  = (LPTSTR) pEveryoneSID;

	ea[1].grfAccessPermissions = u_attribute;
	ea[1].grfAccessMode = SET_ACCESS;
	ea[1].grfInheritance = NO_INHERITANCE;
	ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	//ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	//ea[1].Trustee.ptstrName  = (LPTSTR) pAdminSID;
	ea[1].Trustee.TrusteeType = TRUSTEE_IS_USER;
	ea[1].Trustee.ptstrName  = (LPTSTR) ownerSID;

	ea[2].grfAccessPermissions = g_attribute;
	ea[2].grfAccessMode = SET_ACCESS;
	ea[2].grfInheritance = NO_INHERITANCE;
	ea[2].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	//ea[2].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	//ea[2].Trustee.ptstrName  = (LPTSTR) pNullSID;
	ea[2].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[2].Trustee.ptstrName  = (LPTSTR) primaryGroupSID;

	ea[3].grfAccessPermissions = u_attribute;
	ea[3].grfAccessMode = GRANT_ACCESS;
	ea[3].grfInheritance = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE | INHERIT_ONLY_ACE;
	ea[3].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[3].Trustee.TrusteeType = TRUSTEE_IS_USER;
	ea[3].Trustee.ptstrName  = (LPTSTR) cr_ownerSID;

	ea[4].grfAccessPermissions = g_attribute;
	ea[4].grfAccessMode = GRANT_ACCESS;
	ea[4].grfInheritance = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE | INHERIT_ONLY_ACE;
	ea[4].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[4].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[4].Trustee.ptstrName  = (LPTSTR) cr_groupSID;

	sa.nLength = sizeof (SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = FALSE;

	pACL = NULL;
	dwRes = SetEntriesInAcl(5, ea, NULL, &pACL);
	if (ERROR_SUCCESS != dwRes) 
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::GetFileSecurity"), CWinException::WinExtError);
	if ( PtrCheck(pSD = CastAnyPtr(SECURITY_DESCRIPTOR, LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH))) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::GetFileSecurity"), CWinException::WinExtError);
	if ( !InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::GetFileSecurity"), CWinException::WinExtError);
	if ( !SetSecurityDescriptorDacl(pSD, TRUE, pACL, FALSE) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::GetFileSecurity"), CWinException::WinExtError);
	sa.lpSecurityDescriptor = pSD;
}

void CSecurityContext::FreeSecurity(SECURITY_ATTRIBUTES sa)
{
	BOOL bDaclPresent;
	BOOL bDaclDefaulted;
	PACL pDacl = NULL;

	if ( PtrCheck(sa.lpSecurityDescriptor) )
		return;
	if ( !GetSecurityDescriptorDacl((PSECURITY_DESCRIPTOR)(sa.lpSecurityDescriptor), &bDaclPresent, &pDacl, &bDaclDefaulted) )
		throw OK_NEW_OPERATOR CSecurityContextException(__FILE__LINE__ _T("%s Exception"), _T("CSecurityContext::FreeSecurity"), CWinException::WinExtError);
	if ( NotPtrCheck(pDacl) )
		LocalFree(pDacl);
	LocalFree(sa.lpSecurityDescriptor);
}