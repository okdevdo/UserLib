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
#include "Service.h"
#include "UTLPTR.H"

IMPL_WINEXCEPTION(CServiceException, CWinException)

CServiceInfo::CServiceInfo(CConstPointer pServiceName, CConstPointer pDisplayName, CConstPointer pDescription,
TServiceType pServiceType, TStartType pStartType, TErrorControl pErrorControl,
CConstPointer pLoginAccount, CConstPointer pPassword,
LPSERVICE_MAIN_FUNCTION mainfunction, LPHANDLER_FUNCTION_EX handlerfunction) :
_DisplayName(__FILE__LINE__ pDisplayName),
_Description(__FILE__LINE__ pDescription),
_ServiceType(pServiceType),
_StartType(pStartType),
_ErrorControl(pErrorControl),
_BinaryPathName(),
_LoadOrderGroup(),
_Dependencies(__FILE__LINE__ 16, 16),
_LoginAccount(__FILE__LINE__ pLoginAccount),
_Password(__FILE__LINE__ pPassword),
_MainFunction(mainfunction),
_HandlerFunction(handlerfunction)
{
	TCHAR szPath[MAX_PATH];

	if (!GetModuleFileName(nullptr, szPath, MAX_PATH))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[GetModuleFileName] failed in %s"), _T("CServiceInfo::CServiceInfo"), CWinException::WinExtError);
	_BinaryPathName.SetString(__FILE__LINE__ szPath);

	CStringConstIterator it = _BinaryPathName.Begin();

	it.Find(_T(" "));
	if (!(it.IsEnd()))
	{
		_BinaryPathName.PrependString(_T("\""));
		_BinaryPathName.AppendString(_T("\""));
	}
	if (pServiceName)
	{
		_BinaryPathName.AppendString(_T(" -s "));
		_BinaryPathName.AppendString(pServiceName);
	}
}

CServiceInfo::CServiceInfo(CConstPointer pServiceName, CConstPointer pDisplayName, CConstPointer pDescription, CConstPointer pBinaryPath,
	TServiceType pServiceType, TStartType pStartType, TErrorControl pErrorControl, CConstPointer pLoginAccount) :
	_DisplayName(__FILE__LINE__ pDisplayName),
	_Description(__FILE__LINE__ pDescription),
	_ServiceType(pServiceType),
	_StartType(pStartType),
	_ErrorControl(pErrorControl),
	_BinaryPathName(__FILE__LINE__ pBinaryPath),
	_LoadOrderGroup(),
	_Dependencies(__FILE__LINE__ 16, 16),
	_LoginAccount(__FILE__LINE__ pLoginAccount),
	_Password(),
	_MainFunction(nullptr),
	_HandlerFunction(nullptr)
{
}

CServiceInfo::~CServiceInfo() {}

CByteBuffer CServiceInfo::get_DependenciesAsDataString() const
{
	CByteBuffer result;
	CDataVectorT<CStringBuffer>::Iterator it = _Dependencies.Begin();
	BPointer p;
	dword datasize = 0;
	dword cnt = 0;

	while (it)
	{
		datasize += (*it).GetLength();
		++cnt;
		++it;
	}
	if (!cnt)
		return result;
	datasize += cnt + 1;
	datasize *= szchar;
	result.set_BufferSize(__FILE__LINE__ datasize);
	p = result.get_Buffer();
	it = _Dependencies.Begin();
	while (it)
	{
		CStringBuffer sBuf(*it);
		dword l = sBuf.GetLength() * szchar;

		s_memcpy_s(p, datasize, sBuf.GetString(), l);
		p += l;
		datasize -= l;
		*p++ = 0;
		--datasize;
#ifdef UNICODE
		*p++ = 0;
		--datasize;
#endif
		++it;
	}
#ifdef UNICODE
	*p++ = 0;
#endif
	*p = 0;
	return result;
}

void CServiceInfo::set_DependenciesAsDataString(ConstRef(CByteBuffer) buf)
{
#ifdef UNICODE
	byte vF[2] = { 0, 0 };
	CByteBuffer buf2;
	CStringBuffer tmp;
	dword startPos = 0;
	int pos;

	pos = buf.find_Text(vF, 2, startPos);
	while (pos >= 0)
	{
		if ((pos - startPos) > 0)
		{
			buf.get_SubBuffer(startPos, pos - startPos, buf2);
			tmp.SetSize(__FILE__LINE__ (buf2.get_BufferSize() / 2) + 1);
			s_memcpy_s(CastMutable(CPointer, tmp.GetString()), buf2.get_BufferSize() + 2, buf2.get_Buffer(), buf2.get_BufferSize());
			append_Dependency(tmp);
		}
		startPos = pos + 2;
		pos = buf.find_Text(vF, 2, startPos);
	}
#else
	CByteBuffer buf2;
	CStringBuffer tmp;
	dword startPos = 0;
	int pos;

	pos = buf.find_Char(0, startPos);
	while (pos >= 0)
	{
		if ((pos - startPos) > 0)
		{
			buf.get_SubBuffer(startPos, pos - startPos, buf2);
			tmp.SetSize(__FILE__LINE__ buf2.get_BufferSize() + 1);
			s_memcpy_s(CastMutable(CPointer, tmp.GetString()), buf2.get_BufferSize() + 1, buf2.get_Buffer(), buf2.get_BufferSize());
			append_Dependency(tmp);
		}
		startPos = pos + 1;
		pos = buf.find_Char(0, startPos);
	}
#endif
}

void CServiceInfo::Print()
{
	_tprintf(_T("Anzeigename: %s\n"), _DisplayName.GetString());
	_tprintf(_T("Beschreibung: %s\n"), _Description.GetString());
	switch (_ServiceType)
	{
	case eSingleService:
		_tprintf(_T("Diensttyp: Single\n"));
		break;
	case eMultiServices:
		_tprintf(_T("Diensttyp: Multi\n"));
		break;
	default:
		break;
	}
	switch (_StartType)
	{
		case eDisabledStartType:
		_tprintf(_T("Starttyp: Disabled\n"));
		break;
		case eManualStartType:
		_tprintf(_T("Starttyp: Manual\n"));
		break;
		case eAutoStartType:
		_tprintf(_T("Starttyp: Auto\n"));
		break;
	default:
		break;
	}
	switch (_ErrorControl)
	{
	case eErrorIgnore:
		_tprintf(_T("Fehlertyp: Ignore\n"));
		break;
	case eErrorNormal:
		_tprintf(_T("Fehlertyp: Normal\n"));
		break;
	case eErrorSevere:
		_tprintf(_T("Fehlertyp: Severe\n"));
		break;
	case eErrorCritical:
		_tprintf(_T("Fehlertyp: Critical\n"));
		break;
	default:
		break;
	}
	_tprintf(_T("Ausführbare Datei: %s\n"), _BinaryPathName.GetString());
	_tprintf(_T("Start Gruppe: %s\n"), _LoadOrderGroup.GetString());

	CDataVectorT<CStringBuffer>::Iterator it = _Dependencies.Begin();

	while (it)
	{
		_tprintf(_T("Abhängiger Dienst/Gruppe: %s\n"), (*it).GetString());
		++it;
	}
	_tprintf(_T("Start Name: %s\n"), _LoginAccount.GetString());
}

CService::CService(Ptr(CServiceManager) man) :
_pSCManager(man),
_pService(nullptr),
_ServiceName(),
_pInfo(nullptr),
_ServiceStatusHandle(nullptr),
_ServiceStatus(),
_checkPoint(1),
_StopSignal()
{
}

CService::CService(ConstRef(CStringBuffer) name, Ptr(CServiceInfo) pInfo, Ptr(CServiceManager) man) :
_pSCManager(man),
_pService(nullptr),
_ServiceName(name),
_pInfo(pInfo),
_ServiceStatusHandle(nullptr),
_ServiceStatus(),
_checkPoint(1),
_StopSignal()
{
}

CService::~CService()
{
	if (_pInfo)
		_pInfo->release();
	if (_pService)
		CloseServiceHandle(_pService);
}

void CService::Create(CConstPointer name, ConstPtr(CServiceInfo) info)
{	
	if (PtrCheck(_pSCManager) || PtrCheck(name) || PtrCheck(info))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CService::Create] Invalid arguments or programming sequence error"));

	_pService = CreateService(
		_pSCManager->get_Handle(),			// SCM database 
		name,								// name of service 
		info->get_DisplayName(),			// service name to display 
		SERVICE_ALL_ACCESS,					// desired access 
		info->get_ServiceType(),			// service type 
		info->get_StartType(),				// start type 
		info->get_ErrorControl(),			// error control type 
		info->get_BinaryPathName(),			// path to service's binary 
		info->get_LoadOrderGroup(),			// load ordering group 
		nullptr,								// no tag identifier 
		CastAny(LPCTSTR, info->get_DependenciesAsDataString().get_Buffer()),	// dependencies 
		info->get_LoginAccount(),			// account 
		info->get_Password());				// password 
	if (!_pService)
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CreateService] failed in %s"), _T("CService::Create"), CWinException::WinExtError);

	if (!(info->get_Description().IsEmpty()))
	{
		SERVICE_DESCRIPTION sd;

		sd.lpDescription = CastAny(LPTSTR, CastMutable(CPointer, info->get_Description().GetString()));

		if (!ChangeServiceConfig2(
			_pService,                 // handle to service
			SERVICE_CONFIG_DESCRIPTION, // change: description
			&sd))                      // OK_NEW_OPERATOR description
			throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[ChangeServiceConfig2] failed in %s"), _T("CService::Create"), CWinException::WinExtError);
	}

	_ServiceName.SetString(__FILE__LINE__ name);
	_pInfo = CastMutablePtr(CServiceInfo, info);
}

void CService::Open(DWORD accesstype)
{
	if (PtrCheck(_pSCManager) || _ServiceName.IsEmpty())
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CService::Open] Invalid arguments or programming sequence error"));

	_pService = OpenService(
		_pSCManager->get_Handle(),  // SCM database 
		_ServiceName,				// name of service 
		accesstype);				// access 

	if (PtrCheck(_pService))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[OpenService] failed in %s, Service '%s'"), _T("CService::Open"), _ServiceName.GetString(), CWinException::WinExtError);
}

void CService::Update(ConstPtr(CServiceInfo) info)
{
	if (PtrCheck(_pSCManager) || PtrCheck(_pService) || _ServiceName.IsEmpty() || PtrCheck(_pInfo))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CService::Update] Invalid arguments or programming sequence error"));

	CStringBuffer vDisplayName;
	CStringBuffer vDescription;
	DWORD vServiceType = SERVICE_NO_CHANGE;
	DWORD vStartType = SERVICE_NO_CHANGE;
	DWORD vErrorControl = SERVICE_NO_CHANGE;
	CStringBuffer vBinaryPathName;
	CStringBuffer vLoadOrderGroup;
	CByteBuffer vDependencies1 = _pInfo->get_DependenciesAsDataString();
	CByteBuffer vDependencies2 = info->get_DependenciesAsDataString();
	CStringBuffer vLoginAccount;
	CStringBuffer vPassword;

	if (_pInfo->get_DisplayName().Compare(info->get_DisplayName(), 0, CStringLiteral::cIgnoreCase) != 0)
		vDisplayName = info->get_DisplayName();
	if (_pInfo->get_ServiceType() != info->get_ServiceType())
		vServiceType = info->get_ServiceType();
	if (_pInfo->get_StartType() != info->get_StartType())
		vStartType = info->get_StartType();
	if (_pInfo->get_ErrorControl() != info->get_ErrorControl())
		vErrorControl = info->get_ErrorControl();
	if (_pInfo->get_BinaryPathName().Compare(info->get_BinaryPathName(), 0, CStringLiteral::cIgnoreCase) != 0)
		vBinaryPathName = info->get_BinaryPathName();
	if (_pInfo->get_LoadOrderGroup().Compare(info->get_LoadOrderGroup(), 0, CStringLiteral::cIgnoreCase) != 0)
		vLoadOrderGroup = info->get_LoadOrderGroup();
	if (vDependencies1 == vDependencies2)
		vDependencies2.set_BufferSize(__FILE__LINE__ 0);
	if (_pInfo->get_LoginAccount().Compare(info->get_LoginAccount()) != 0)
		vLoginAccount = info->get_LoginAccount();
	if (_pInfo->get_Password().Compare(info->get_Password()) != 0)
		vPassword = info->get_Password();

	if (!ChangeServiceConfig(
		_pService,          // handle of service 
		vServiceType,		// service type
		vStartType,			// service start type 
		vErrorControl,		// error control
		vBinaryPathName,    // binary path
		vLoadOrderGroup,    // load order group
		nullptr,				// tag ID: no change 
		CastAny(LPTSTR, vDependencies2.get_Buffer()),	// dependencies
		vLoginAccount,      // account name
		vPassword,          // password
		vDisplayName))      // display name
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[ChangeServiceConfig] failed in %s"), _T("CService::Update"), CWinException::WinExtError);

	if (_pInfo->get_Description().Compare(info->get_Description(), 0, CStringLiteral::cIgnoreCase) != 0)
	{
		vDescription = info->get_Description();

		SERVICE_DESCRIPTION sd;

		sd.lpDescription = CastAny(LPTSTR, CastMutable(CPointer, vDescription.GetString()));

		if (!ChangeServiceConfig2(
			_pService,          // handle of service 
			SERVICE_CONFIG_DESCRIPTION,
			&sd
			))
			throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[ChangeServiceConfig2] failed in %s"), _T("CService::Update"), CWinException::WinExtError);
	}
	if (!(vDisplayName.IsEmpty()))
		_pInfo->set_DisplayName(vDisplayName);
	if (!(vDescription.IsEmpty()))
		_pInfo->set_Description(vDescription);
	if (vServiceType != SERVICE_NO_CHANGE)
		_pInfo->set_ServiceType(Cast(CServiceInfo::TServiceType, vServiceType));
	if (vStartType != SERVICE_NO_CHANGE)
		_pInfo->set_StartType(Cast(CServiceInfo::TStartType, vStartType));
	if (vErrorControl != SERVICE_NO_CHANGE)
		_pInfo->set_ErrorControl(Cast(CServiceInfo::TErrorControl, vErrorControl));
	if (!(vBinaryPathName.IsEmpty()))
		_pInfo->set_BinaryPathName(vBinaryPathName);
	if (!(vLoadOrderGroup.IsEmpty()))
		_pInfo->set_LoadOrderGroup(vLoadOrderGroup);
	if (vDependencies2.get_BufferSize() > 0)
		_pInfo->set_DependenciesAsDataString(vDependencies2);
	if (!(vLoginAccount.IsEmpty()))
		_pInfo->set_LoginAccount(vLoginAccount);
	if (!(vPassword.IsEmpty()))
		_pInfo->set_Password(vPassword);
}

void CService::Query()
{
	if (PtrCheck(_pSCManager) || PtrCheck(_pService) || _ServiceName.IsEmpty())
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CService::Query] Invalid arguments or programming sequence error"));

	if (_pInfo)
	{
		_pInfo->release();
		_pInfo = nullptr;
	}

	LPQUERY_SERVICE_CONFIG lpsc = nullptr;
	LPSERVICE_DESCRIPTION lpsd = nullptr;
	DWORD dwBytesNeeded = 0;
	DWORD cbBufSize = 0;
	DWORD dwError = 0;

	if (!QueryServiceConfig(
		_pService,
		nullptr,
		0,
		&dwBytesNeeded))
	{
		dwError = GetLastError();
		switch (dwError)
		{
		case ERROR_INSUFFICIENT_BUFFER:
			cbBufSize = dwBytesNeeded;
			lpsc = CastAnyPtr(QUERY_SERVICE_CONFIG, TFalloc(cbBufSize));
			break;
		default:
			throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[QueryServiceConfig] failed in %s"), _T("CService::Query"), CWinException::WinExtError, dwError);
		}
	}

	if (!QueryServiceConfig(
		_pService,
		lpsc,
		cbBufSize,
		&dwBytesNeeded))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[QueryServiceConfig] failed in %s"), _T("CService::Query"), CWinException::WinExtError);

	if (!QueryServiceConfig2(
		_pService,
		SERVICE_CONFIG_DESCRIPTION,
		nullptr,
		0,
		&dwBytesNeeded))
	{
		dwError = GetLastError();
		switch (dwError)
		{
		case ERROR_INSUFFICIENT_BUFFER:
			cbBufSize = dwBytesNeeded;
			lpsd = CastAnyPtr(SERVICE_DESCRIPTION, TFalloc(cbBufSize));
			break;
		default:
			throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[QueryServiceConfig2] failed in %s"), _T("CService::Query"), CWinException::WinExtError, dwError);
		}
	}

	if (!QueryServiceConfig2(
		_pService,
		SERVICE_CONFIG_DESCRIPTION,
		CastAnyPtr(BYTE, lpsd),
		cbBufSize,
		&dwBytesNeeded))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[QueryServiceConfig2] failed in %s"), _T("CService::Query"), CWinException::WinExtError);

	_pInfo = OK_NEW_OPERATOR CServiceInfo(
		_ServiceName,
		lpsc->lpDisplayName, 
		lpsd->lpDescription, 
		lpsc->lpBinaryPathName,
		Cast(CServiceInfo::TServiceType, lpsc->dwServiceType), 
		Cast(CServiceInfo::TStartType, lpsc->dwStartType), 
		Cast(CServiceInfo::TErrorControl, lpsc->dwErrorControl),
		lpsc->lpServiceStartName);

	_pInfo->set_LoadOrderGroup(CStringBuffer(__FILE__LINE__ lpsc->lpLoadOrderGroup));
	if ((lpsc->lpDependencies) && (DerefCPointer(lpsc->lpDependencies) != 0))
	{
		CConstPointer p = lpsc->lpDependencies;
		CByteBuffer buf;

		while ((*p) || (*(p + 1)))
			++p;
		buf.set_Buffer(__FILE__LINE__ CastAnyConstPtr(byte, p), Castdword(p - lpsc->lpDependencies));
		_pInfo->set_DependenciesAsDataString(buf);
	}
	if (lpsc)
		TFfree(lpsc);
	if (lpsd)
		TFfree(lpsd);
}

void CService::Register()
{
	if (PtrCheck(_pInfo) || PtrCheck(_pInfo->get_HandlerFunction()) || _ServiceName.IsEmpty())
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CService::Register] Invalid arguments or programming sequence error"));

	_ServiceStatusHandle = RegisterServiceCtrlHandlerEx(_ServiceName, _pInfo->get_HandlerFunction(), this);
	if (!_ServiceStatusHandle)
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[RegisterServiceCtrlHandlerEx] failed in %s"), _T("CService::Register"), CWinException::WinExtError);

	s_memset(&_ServiceStatus, 0, sizeof(SERVICE_STATUS));
	_ServiceStatus.dwServiceType = _pInfo->get_ServiceType();

	SendStatus(eServiceStartPending, NO_ERROR, 3000);
}

void CService::SendStatus(TCurrentState state, DWORD resultCode, DWORD waitHint)
{
	if (PtrCheck(_ServiceStatusHandle))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CService::SendStatus] Invalid arguments or programming sequence error"));

	_ServiceStatus.dwCurrentState = state;
	_ServiceStatus.dwWin32ExitCode = resultCode;
	_ServiceStatus.dwWaitHint = waitHint;

	if (state == eServiceStartPending)
		_ServiceStatus.dwControlsAccepted = 0;
	else 
		_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;

	if ((state == eServiceRunning) || (state == eServiceStopped))
		_ServiceStatus.dwCheckPoint = 0;
	else 
		_ServiceStatus.dwCheckPoint = _checkPoint++;

	SetServiceStatus(_ServiceStatusHandle, &_ServiceStatus);
}

void CService::SendCurrentStatus()
{
	if (PtrCheck(_ServiceStatusHandle))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CService::SendCurrentStatus] Invalid arguments or programming sequence error"));
	SetServiceStatus(_ServiceStatusHandle, &_ServiceStatus);
}

void CService::SendStopSignal()
{
	SendStatus(eServiceStopPending, NO_ERROR, 3000);
	_StopSignal.set();
}

bool CService::GetStopSignal(DWORD millsec)
{
	return _StopSignal.wait(millsec);
}

DWORD CService::DoCtrlHandler(DWORD dwControl)
{
	switch (dwControl)
	{
	case SERVICE_CONTROL_INTERROGATE:
		SendCurrentStatus();
		return NO_ERROR;
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
		SendStopSignal();
		return NO_ERROR;
	default:
		break;
	}
	return ERROR_CALL_NOT_IMPLEMENTED;
}

void CService::Close() 
{
	if (NotPtrCheck(_pService))
	{
		CloseServiceHandle(_pService);
		_pService = nullptr;
	}
}

void CService::Print()
{
	_tprintf(_T("\n\n ************** Service: %s ************** \n\n"), _ServiceName.GetString());

	if (_pInfo)
		_pInfo->Print();
}

CServices::CServices(DECL_FILE_LINE0) : super(ARGS_FILE_LINE 16, 16)
{
}

CServices::~CServices() 
{
}

CServiceManager::CServiceManager() : _pSCManager(nullptr), _Services(__FILE__LINE__0) {}
CServiceManager::~CServiceManager() {}

void CServiceManager::StartUp() 
{
	_pSCManager = OpenSCManager(
		nullptr,                    // local computer
		nullptr,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (PtrCheck(_pSCManager))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[OpenSCManager] failed in %s"), _T("CServiceManager::StartUp"), CWinException::WinExtError);
}

void CServiceManager::Install(Ptr(CServiceInfoTable) sTable)
{
	if (PtrCheck(_pSCManager) || PtrCheck(sTable))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CServiceManager::Install] Invalid arguments or programming sequence error"));

	while (NotPtrCheck(sTable->_service) && NotPtrCheck(sTable->_info))
	{
		Ptr(CService) pService = nullptr;

		Create(sTable->_service, sTable->_info, &pService);
		++sTable;
	}
}

void CServiceManager::Load(Ptr(CServiceInfoTable) sTable)
{
	if (PtrCheck(_pSCManager) || PtrCheck(sTable))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CServiceManager::Load] Invalid arguments or programming sequence error"));

	while (NotPtrCheck(sTable->_service) && NotPtrCheck(sTable->_info))
	{
		Ptr(CService) pService = OK_NEW_OPERATOR CService(CStringBuffer(__FILE__LINE__ sTable->_service), nullptr, this);

		pService->Open();
		pService->Query();
		pService->get_pInfo()->set_MainFunction(sTable->_info->get_MainFunction());
		pService->get_pInfo()->set_HandlerFunction(sTable->_info->get_HandlerFunction());
		pService->Close();
		_Services.InsertSorted(pService);
		++sTable;
	}
}

void CServiceManager::EnumAll()
{
	if (PtrCheck(_pSCManager))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CServiceManager::EnumAll] Invalid arguments or programming sequence error"));

	DWORD resumeC = 0;

	do
	{
		BPointer buf = nullptr;
		DWORD bufSize = 0;
		DWORD svcCnt = 0;

		if (!EnumServicesStatusEx(
			_pSCManager,
			SC_ENUM_PROCESS_INFO,
			SERVICE_WIN32,
			SERVICE_STATE_ALL,
			buf,
			0,
			&bufSize,
			&svcCnt,
			&resumeC,
			nullptr
			))
		{
			if (ERROR_MORE_DATA != GetLastError())
				throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[EnumServicesStatusEx] failed in %s"), _T("CServiceManager::EnumAll"), CWinException::WinExtError);

			buf = CastAnyPtr(byte, TFalloc(bufSize));

			EnumServicesStatusEx(
				_pSCManager,
				SC_ENUM_PROCESS_INFO,
				SERVICE_WIN32,
				SERVICE_STATE_ALL,
				buf,
				bufSize,
				&bufSize,
				&svcCnt,
				&resumeC,
				nullptr
				);
		}

		LPENUM_SERVICE_STATUS_PROCESS pSvcStatus = CastAnyPtr(ENUM_SERVICE_STATUS_PROCESS, buf);

		for (DWORD i = 0; i < svcCnt; ++i)
		{
			Ptr(CService) pService = OK_NEW_OPERATOR CService(CStringBuffer(__FILE__LINE__ pSvcStatus->lpServiceName), nullptr, this);

			try
			{
				pService->Open(CService::eServiceQueryConfig);
				pService->Query();
				pService->Close();
			}
			catch (CBaseException* ex)
			{
				_tprintf(_T("%s\n"), ex->GetExceptionMessage().GetString());
			}
			_Services.InsertSorted(pService);
			++pSvcStatus;
		}
		if (buf)
			TFfree(buf);
	} while (resumeC != 0);
}

void CServiceManager::Run(ConstRef(TMBCharList) services)
{
	if (PtrCheck(_pSCManager) || (services.Count() == 0))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CServiceManager::Start] Invalid arguments or programming sequence error"));

	TMBCharList::Iterator it = services.Begin();
	LPSERVICE_TABLE_ENTRY pDispatchTable = CastAnyPtr(SERVICE_TABLE_ENTRY, TFalloc(sizeof(SERVICE_TABLE_ENTRY) * services.Count()));
	LPSERVICE_TABLE_ENTRY p = pDispatchTable;

	while (it)
	{
		Ptr(CService) pService = nullptr;

		Open(*it, &pService);
		if (!pService)
			throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CServiceManager::Run] Service %s not found"), *it);
		p->lpServiceName = *it;
		p->lpServiceProc = pService->get_pInfo()->get_MainFunction();
		p = CastAnyPtr(SERVICE_TABLE_ENTRY, l_ptradd(p, sizeof(SERVICE_TABLE_ENTRY)));
		++it;
	}
	if (!StartServiceCtrlDispatcher(pDispatchTable))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[StartServiceCtrlDispatcher] failed in %s"), _T("CServiceManager::Run"), CWinException::WinExtError);
	TFfree(pDispatchTable);
}

void CServiceManager::Start(CConstPointer name)
{
	if (PtrCheck(_pSCManager) || PtrCheck(name))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CServiceManager::Start] Invalid arguments or programming sequence error"));

	SC_HANDLE pService = nullptr;
	SERVICE_STATUS_PROCESS ssStatus;
	DWORD dwOldCheckPoint;
	DWORD dwStartTickCount;
	DWORD dwWaitTime;
	DWORD dwBytesNeeded;

	pService = OpenService(
		_pSCManager,			// SCM database 
		name,					// name of service 
		SERVICE_QUERY_STATUS | SERVICE_START);	// access 

	if (pService == nullptr)
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[OpenService] failed in %s"), _T("CServiceManager::Start"), CWinException::WinExtError);

	if (!QueryServiceStatusEx(
		pService,							// handle to service 
		SC_STATUS_PROCESS_INFO,				// information level
		CastAnyPtr(BYTE, &ssStatus),		// address of structure
		sizeof(SERVICE_STATUS_PROCESS),		// size of structure
		&dwBytesNeeded))					// size needed if buffer is too small
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[QueryServiceStatusEx] failed in %s"), _T("CServiceManager::Start"), CWinException::WinExtError);

	if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
	{
		CloseServiceHandle(pService);
		return;
	}
		
	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
	{
		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		if (!QueryServiceStatusEx(
			pService,							// handle to service 
			SC_STATUS_PROCESS_INFO,				// information level
			CastAnyPtr(BYTE, &ssStatus),		// address of structure
			sizeof(SERVICE_STATUS_PROCESS),		// size of structure
			&dwBytesNeeded))					// size needed if buffer is too small
			throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[QueryServiceStatusEx] failed in %s"), _T("CServiceManager::Start"), CWinException::WinExtError);

		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
		{
			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
		{
			CloseServiceHandle(pService);
			return;
		}
	}

	if (!StartService(
		pService,  // handle to service 
		0,           // number of arguments 
		nullptr))      // no arguments 
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[StartService] failed in %s"), _T("CServiceManager::Start"), CWinException::WinExtError);

	if (!QueryServiceStatusEx(
		pService,							// handle to service 
		SC_STATUS_PROCESS_INFO,				// information level
		CastAnyPtr(BYTE, &ssStatus),		// address of structure
		sizeof(SERVICE_STATUS_PROCESS),		// size of structure
		&dwBytesNeeded))					// size needed if buffer is too small
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[QueryServiceStatusEx] failed in %s"), _T("CServiceManager::Start"), CWinException::WinExtError);

	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
	{
		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		if (!QueryServiceStatusEx(
			pService,							// handle to service 
			SC_STATUS_PROCESS_INFO,				// information level
			CastAnyPtr(BYTE, &ssStatus),		// address of structure
			sizeof(SERVICE_STATUS_PROCESS),		// size of structure
			&dwBytesNeeded))					// size needed if buffer is too small
			throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[QueryServiceStatusEx] failed in %s"), _T("CServiceManager::Start"), CWinException::WinExtError);

		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
		{
			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
			break;
	}

	CloseServiceHandle(pService);
}

void CServiceManager::Stop(CConstPointer name)
{
	if (PtrCheck(_pSCManager) || PtrCheck(name))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CServiceManager::Stop] Invalid arguments or programming sequence error"));

	SC_HANDLE pService = nullptr;
	SERVICE_STATUS_PROCESS ssStatus;
	DWORD dwStartTime = GetTickCount();
	DWORD dwBytesNeeded;
	DWORD dwTimeout = 30000; // 30-second time-out
	DWORD dwWaitTime;

	pService = OpenService(
		_pSCManager,		// SCM database 
		name,				// name of service 
		SERVICE_QUERY_STATUS | SERVICE_STOP | SERVICE_ENUMERATE_DEPENDENTS); // access 

	if (PtrCheck(pService))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[OpenService] failed in %s"), _T("CServiceManager::Stop"), CWinException::WinExtError);

	if (!QueryServiceStatusEx(
		pService,							// handle to service 
		SC_STATUS_PROCESS_INFO,				// information level
		CastAnyPtr(BYTE, &ssStatus),		// address of structure
		sizeof(SERVICE_STATUS_PROCESS),		// size of structure
		&dwBytesNeeded))					// size needed if buffer is too small
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[QueryServiceStatusEx] failed in %s"), _T("CServiceManager::Stop"), CWinException::WinExtError);

	if (ssStatus.dwCurrentState == SERVICE_STOPPED)
	{
		CloseServiceHandle(pService);
		return;
	}

	while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
	{
		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		if (!QueryServiceStatusEx(
			pService,							// handle to service 
			SC_STATUS_PROCESS_INFO,				// information level
			CastAnyPtr(BYTE, &ssStatus),		// address of structure
			sizeof(SERVICE_STATUS_PROCESS),		// size of structure
			&dwBytesNeeded))					// size needed if buffer is too small
			throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[QueryServiceStatusEx] failed in %s"), _T("CServiceManager::Stop"), CWinException::WinExtError);

		if (ssStatus.dwCurrentState == SERVICE_STOPPED)
		{
			CloseServiceHandle(pService);
			return;
		}

		if ((GetTickCount() - dwStartTime) > dwTimeout)
		{
			CloseServiceHandle(pService);
			return;
		}
	}

	StopDependentServices(pService);

	if (!ControlService(
		pService,
		SERVICE_CONTROL_STOP,
		CastAnyPtr(SERVICE_STATUS, &ssStatus)))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[ControlService] failed in %s"), _T("CServiceManager::Stop"), CWinException::WinExtError);

	while (ssStatus.dwCurrentState != SERVICE_STOPPED)
	{
		Sleep(ssStatus.dwWaitHint);

		if (!QueryServiceStatusEx(
			pService,							// handle to service 
			SC_STATUS_PROCESS_INFO,				// information level
			CastAnyPtr(BYTE, &ssStatus),		// address of structure
			sizeof(SERVICE_STATUS_PROCESS),		// size of structure
			&dwBytesNeeded))					// size needed if buffer is too small
			throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[QueryServiceStatusEx] failed in %s"), _T("CServiceManager::Stop"), CWinException::WinExtError);

		if (ssStatus.dwCurrentState == SERVICE_STOPPED)
			break;

		if (GetTickCount() - dwStartTime > dwTimeout)
			break;
	}
	CloseServiceHandle(pService);
}

BOOL CServiceManager::StopDependentServices(SC_HANDLE pService)
{
	DWORD i;
	DWORD dwBytesNeeded;
	DWORD dwCount;

	LPENUM_SERVICE_STATUS   lpDependencies = nullptr;
	ENUM_SERVICE_STATUS     ess;
	SC_HANDLE               hDepService;
	SERVICE_STATUS_PROCESS  ssp;

	DWORD dwStartTime = GetTickCount();
	DWORD dwTimeout = 30000; // 30-second time-out

	// Pass a zero-length buffer to get the required buffer size.
	if (EnumDependentServices(
		pService, 
		SERVICE_ACTIVE,
		lpDependencies, 
		0, 
		&dwBytesNeeded, 
		&dwCount))
		return TRUE;

	if (GetLastError() != ERROR_MORE_DATA)
		return FALSE;

	lpDependencies = CastAnyPtr(ENUM_SERVICE_STATUS, TFalloc(dwBytesNeeded));

	if (!EnumDependentServices(
		pService,
		SERVICE_ACTIVE,
		lpDependencies,
		dwBytesNeeded,
		&dwBytesNeeded,
		&dwCount))
	{
		TFfree(lpDependencies);
		return FALSE;
	}

	for (i = 0; i < dwCount; i++)
	{
		ess = *(lpDependencies + i);

		hDepService = OpenService(
			_pSCManager,
			ess.lpServiceName,
			SERVICE_STOP | SERVICE_QUERY_STATUS);

		if (!hDepService)
		{
			TFfree(lpDependencies);
			return FALSE;
		}

		if (!ControlService(
			hDepService,
			SERVICE_CONTROL_STOP,
			CastAnyPtr(SERVICE_STATUS, &ssp)))
		{
			TFfree(lpDependencies);
			return FALSE;
		}

		while (ssp.dwCurrentState != SERVICE_STOPPED)
		{
			Sleep(ssp.dwWaitHint);

			if (!QueryServiceStatusEx(
				hDepService,
				SC_STATUS_PROCESS_INFO,
				CastAnyPtr(BYTE, &ssp),
				sizeof(SERVICE_STATUS_PROCESS),
				&dwBytesNeeded))
			{
				TFfree(lpDependencies);
				CloseServiceHandle(hDepService);
				return FALSE;
			}

			if (ssp.dwCurrentState == SERVICE_STOPPED)
				break;

			if (GetTickCount() - dwStartTime > dwTimeout)
			{
				TFfree(lpDependencies);
				CloseServiceHandle(hDepService);
				return FALSE;
			}
		}
		CloseServiceHandle(hDepService);
	}
	return TRUE;
}

void CServiceManager::Disable(CConstPointer name)
{
	if (PtrCheck(_pSCManager) || PtrCheck(name))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CServiceManager::Disable] Invalid arguments or programming sequence error"));

	SC_HANDLE pService = nullptr;

	pService = OpenService(
		_pSCManager,			// SCM database 
		name,					// name of service 
		SERVICE_CHANGE_CONFIG);	// need delete access 

	if (pService == nullptr)
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[OpenService] failed in %s"), _T("CServiceManager::Disable"), CWinException::WinExtError);

	if (!ChangeServiceConfig(
		pService,          // handle of service 
		SERVICE_NO_CHANGE, // service type: no change 
		SERVICE_DISABLED,  // service start type 
		SERVICE_NO_CHANGE, // error control: no change 
		nullptr,              // binary path: no change 
		nullptr,              // load order group: no change 
		nullptr,              // tag ID: no change 
		nullptr,              // dependencies: no change 
		nullptr,              // account name: no change 
		nullptr,              // password: no change 
		nullptr))             // display name: no change
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[ChangeServiceConfig] failed in %s"), _T("CServiceManager::Disable"), CWinException::WinExtError);

	CloseServiceHandle(pService);
}

void CServiceManager::Enable(CConstPointer name)
{
	if (PtrCheck(_pSCManager) || PtrCheck(name))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CServiceManager::Enable] Invalid arguments or programming sequence error"));

	SC_HANDLE pService = nullptr;

	pService = OpenService(
		_pSCManager,			// SCM database 
		name,					// name of service 
		SERVICE_CHANGE_CONFIG);	// need delete access 

	if (pService == nullptr)
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[OpenService] failed in %s"), _T("CServiceManager::Enable"), CWinException::WinExtError);

	if (!ChangeServiceConfig(
		pService,              // handle of service 
		SERVICE_NO_CHANGE,     // service type: no change 
		SERVICE_DEMAND_START,  // service start type 
		SERVICE_NO_CHANGE,     // error control: no change 
		nullptr,                  // binary path: no change 
		nullptr,                  // load order group: no change 
		nullptr,                  // tag ID: no change 
		nullptr,                  // dependencies: no change 
		nullptr,                  // account name: no change 
		nullptr,                  // password: no change 
		nullptr))                 // display name: no change
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[ChangeServiceConfig] failed in %s"), _T("CServiceManager::Enable"), CWinException::WinExtError);


	CloseServiceHandle(pService);
}

void CServiceManager::Delete(CConstPointer name)
{
	if (PtrCheck(_pSCManager) || PtrCheck(name))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CServiceManager::Delete] Invalid arguments or programming sequence error"));

	SC_HANDLE pService = nullptr;

	pService = OpenService(
		_pSCManager,			// SCM database 
		name,					// name of service 
		DELETE);	// need delete access 

	if (pService == nullptr)
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[OpenService] failed in %s"), _T("CServiceManager::Delete"), CWinException::WinExtError);

	if (!DeleteService(pService))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[DeleteService] failed in %s"), _T("CServiceManager::Delete"), CWinException::WinExtError);

	CloseServiceHandle(pService);
}

void CServiceManager::CleanUp() 
{
	if (_pSCManager)
		CloseServiceHandle(_pSCManager);

	CServices::Iterator it = _Services.Begin();

	while (it)
	{
		(*it)->Close();
		++it;
	}
}

void CServiceManager::Create(CConstPointer name, ConstPtr(CServiceInfo) info, CService** pItem) 
{
	if (PtrCheck(_pSCManager) || PtrCheck(name) || PtrCheck(info) || PtrCheck(pItem))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CServiceManager::Create] Invalid arguments or programming sequence error"));

	*pItem = OK_NEW_OPERATOR CService(this);

	(*pItem)->Create(name, info);
	_Services.InsertSorted(*pItem);
}

void CServiceManager::Open(CConstPointer name, CService** pItem) 
{
	if (PtrCheck(_pSCManager) || PtrCheck(name) || PtrCheck(pItem))
		throw OK_NEW_OPERATOR CServiceException(__FILE__LINE__ _T("[CServiceManager::Open] Invalid arguments or programming sequence error"));

	*pItem = nullptr;

	Ptr(CService) pSearchData = OK_NEW_OPERATOR CService(CStringBuffer(__FILE__LINE__ name));
	CServices::Iterator it = _Services.FindSorted(pSearchData);

	if (_Services.MatchSorted(it, pSearchData))
	{
		*pItem = *it;
		(*pItem)->Open();
	}
	pSearchData->release();
}

void CServiceManager::Print()
{
	CServices::Iterator it = _Services.Begin();

	while (it)
	{
		(*it)->Print();
		++it;
	}
}
