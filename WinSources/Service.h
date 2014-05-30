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
#include "DataVector.h"
#include "Event.h"

class WINSOURCES_API CServiceInfo : public CCppObject
{
public:
	enum TServiceType
	{
		eSingleService = SERVICE_WIN32_OWN_PROCESS,
		eMultiServices = SERVICE_WIN32_SHARE_PROCESS
	};

	enum TStartType
	{
		eDisabledStartType = SERVICE_DISABLED,
		eManualStartType = SERVICE_DEMAND_START,
		eAutoStartType = SERVICE_AUTO_START
	};

	enum TErrorControl
	{
		eErrorIgnore = SERVICE_ERROR_IGNORE,
		eErrorNormal = SERVICE_ERROR_NORMAL,
		eErrorSevere = SERVICE_ERROR_SEVERE,
		eErrorCritical = SERVICE_ERROR_CRITICAL
	};

	CServiceInfo(CConstPointer pServiceName = NULL, CConstPointer pDisplayName = NULL, CConstPointer pDescription = NULL,
		TServiceType pServiceType = eSingleService, TStartType pStartType = eDisabledStartType, TErrorControl pErrorControl = eErrorIgnore,
		CConstPointer pLoginAccount = NULL, CConstPointer pPassword = NULL,
		LPSERVICE_MAIN_FUNCTION mainfunction = NULL, LPHANDLER_FUNCTION_EX handlerfunction = NULL);
	CServiceInfo(CConstPointer pServiceName, CConstPointer pDisplayName, CConstPointer pDescription, CConstPointer pBinaryPath, 
		TServiceType pServiceType, TStartType pStartType, TErrorControl pErrorControl, CConstPointer pLoginAccount);
	virtual ~CServiceInfo();

	__inline ConstRef(CStringBuffer) get_DisplayName() const { return _DisplayName; }
	__inline void set_DisplayName(ConstRef(CStringBuffer) value) { _DisplayName = value; }
	__inline ConstRef(CStringBuffer) get_Description() const { return _Description; }
	__inline void set_Description(ConstRef(CStringBuffer) value) { _Description = value; }
	__inline TServiceType get_ServiceType() const { return _ServiceType; }
	__inline void set_ServiceType(TServiceType value) { _ServiceType = value; }
	__inline TStartType get_StartType() const { return _StartType; }
	__inline void set_StartType(TStartType value) { _StartType = value; }
	__inline TErrorControl get_ErrorControl() const { return _ErrorControl; }
	__inline void set_ErrorControl(TErrorControl value) { _ErrorControl = value; }
	__inline ConstRef(CStringBuffer) get_BinaryPathName() const { return _BinaryPathName; }
	__inline void set_BinaryPathName(ConstRef(CStringBuffer) value) { _BinaryPathName = value; }
	__inline ConstRef(CStringBuffer) get_LoadOrderGroup() const { return _LoadOrderGroup; }
	__inline void set_LoadOrderGroup(ConstRef(CStringBuffer) value) { _LoadOrderGroup = value; }
	__inline ConstRef(CDataVectorT<CStringBuffer>) get_Dependencies() const { return _Dependencies; }
	__inline void append_Dependency(ConstRef(CStringBuffer) value) { _Dependencies.Append(value); }
	CByteBuffer get_DependenciesAsDataString() const;
	void set_DependenciesAsDataString(ConstRef(CByteBuffer) buf);
	__inline ConstRef(CStringBuffer) get_LoginAccount() const { return _LoginAccount; }
	__inline void set_LoginAccount(ConstRef(CStringBuffer) value) { _LoginAccount = value; }
	__inline ConstRef(CStringBuffer) get_Password() const { return _Password; }
	__inline void set_Password(ConstRef(CStringBuffer) value) { _Password = value; }
	__inline LPSERVICE_MAIN_FUNCTION get_MainFunction() const { return _MainFunction; }
	__inline void set_MainFunction(LPSERVICE_MAIN_FUNCTION value) { _MainFunction = value; }
	__inline LPHANDLER_FUNCTION_EX get_HandlerFunction() const { return _HandlerFunction; }
	__inline void set_HandlerFunction(LPHANDLER_FUNCTION_EX value) { _HandlerFunction = value; }

	void Print();

protected:
	CStringBuffer _DisplayName;
	CStringBuffer _Description;
	TServiceType _ServiceType;	
	TStartType _StartType;
	TErrorControl _ErrorControl;
	CStringBuffer _BinaryPathName;
	CStringBuffer _LoadOrderGroup;
	CDataVectorT<CStringBuffer> _Dependencies;
	CStringBuffer _LoginAccount;
	CStringBuffer _Password;
	LPSERVICE_MAIN_FUNCTION _MainFunction;
	LPHANDLER_FUNCTION_EX _HandlerFunction;
};


typedef struct tagServiceInfoTable
{
	CConstPointer _service;
	Ptr(CServiceInfo) _info;
} CServiceInfoTable;

class CServiceManager;
class WINSOURCES_API CService : public CCppObject
{
public:
	enum TCurrentState
	{
		eServiceStopped = SERVICE_STOPPED,
		eServiceStartPending = SERVICE_START_PENDING,
		eServiceStopPending = SERVICE_STOP_PENDING,
		eServiceRunning = SERVICE_RUNNING,
		eServiceContinuePending = SERVICE_CONTINUE_PENDING,
		eServicePausePending = SERVICE_PAUSE_PENDING,
		eServicePaused = SERVICE_PAUSED
	};

	enum TAccessType
	{
		eServiceQueryConfig = SERVICE_QUERY_CONFIG,
		eServiceChangeConfig = SERVICE_CHANGE_CONFIG,
		eServiceQueryStatus = SERVICE_QUERY_STATUS,
		eServiceEnumerateDependents = SERVICE_ENUMERATE_DEPENDENTS,
		eServiceStart = SERVICE_START,
		eServiceStop = SERVICE_STOP,
		eServicePauseContinue = SERVICE_PAUSE_CONTINUE,
		eServiceInterrogate = SERVICE_INTERROGATE,
		eServiceUserDefinedControl = SERVICE_USER_DEFINED_CONTROL,
		eServiceAllAccess = SERVICE_ALL_ACCESS
	};

	CService(Ptr(CServiceManager) man = NULL);
	CService(ConstRef(CStringBuffer) name, Ptr(CServiceInfo) pInfo = NULL, Ptr(CServiceManager) man = NULL);
	virtual ~CService();

	__inline SC_HANDLE get_Handle() const { return _pService; }
	__inline ConstRef(CStringBuffer) get_Name() const { return _ServiceName; }
	__inline ConstRef(CStringBuffer) get_ServiceName() const { return _ServiceName; }
	__inline void set_ServiceName(ConstRef(CStringBuffer) value) { _ServiceName = value; }
	__inline Ptr(CServiceInfo) get_pInfo() const { return _pInfo; }

	void Create(CConstPointer name, ConstPtr(CServiceInfo) info);
	void Open(DWORD accesstype = eServiceAllAccess);
	void Update(ConstPtr(CServiceInfo) info);
	void Query();
	void Register();
	void SendStatus(TCurrentState state, DWORD resultCode, DWORD waitHint);
	void SendCurrentStatus();
	void SendStopSignal();
	bool GetStopSignal(DWORD millsec = INFINITE);
	DWORD DoCtrlHandler(DWORD);
	void Close();

	void Print();

protected:
	Ptr(CServiceManager) _pSCManager;
	SC_HANDLE _pService;
	CStringBuffer _ServiceName;
	Ptr(CServiceInfo) _pInfo;
	SERVICE_STATUS_HANDLE _ServiceStatusHandle;
	SERVICE_STATUS _ServiceStatus;
	DWORD _checkPoint;
	CEvent _StopSignal;
};

class WINSOURCES_API CServices : public CDataVectorT<CService, CStringByNameIgnoreCaseLessFunctor<CService>>
{
	typedef CDataVectorT<CService, CStringByNameIgnoreCaseLessFunctor<CService>> super;

public:
	CServices(DECL_FILE_LINE0);
	virtual ~CServices();
};

class WINSOURCES_API CServiceManager : public CCppObject
{
public:
	CServiceManager();
	virtual ~CServiceManager();

	__inline SC_HANDLE get_Handle() const { return _pSCManager; }
	__inline ConstRef(CServices) get_Services() const { return _Services; }

	void StartUp();
	void Install(Ptr(CServiceInfoTable) sTable);
	void Load(Ptr(CServiceInfoTable) sTable);
	void Run(ConstRef(TMBCharList) services);
	void EnumAll();
	void Stop(CConstPointer name);
	void Start(CConstPointer name);
	void Disable(CConstPointer name);
	void Enable(CConstPointer name);
	void Delete(CConstPointer name);
	void CleanUp();

	void Create(CConstPointer name, ConstPtr(CServiceInfo) info, CService** pItem);
	void Open(CConstPointer name, CService** pItem);

	void Print();

protected:
	SC_HANDLE _pSCManager;
	CServices _Services;

	BOOL StopDependentServices(SC_HANDLE pService);
};

DECL_WINEXCEPTION(WINSOURCES_API, CServiceException, CWinException)