/******************************************************************************
    
	This file is part of XService, which is part of UserLib.

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
#include "stdafx.h"
#include "Application.h"
#include "Configuration.h"
#include "AsyncIOManager.h"
#include "AsyncTCPServer.h"
#include "ScopedLock.h"
#include "HttpServer.h"
#include "SqLite3DB.h"

#define SERVICENAME1 _T("SqLiteService")
#define SERVICEDISPLAYNAME1 _T("Olivers SqLite3 Service")
#define SERVICEDESCRIPTION1 _T("Olivers SqLite3 TestService")

#define SERVICENAME2 _T("HttpServerService")
#define SERVICEDISPLAYNAME2 _T("Olivers Http Server Service")
#define SERVICEDESCRIPTION2 _T("Olivers Http Server TestService")

CCppObjectPtr<CServiceManager> gSCManager = OK_NEW_OPERATOR CServiceManager;

class CServiceAsyncTCPClient : public CAsyncTCPClient
{
public:
	CServiceAsyncTCPClient(Ptr(CAsyncIOData) pData) :
		CAsyncTCPClient(pData), _env(nullptr), _conn(nullptr), _stmt(nullptr), _resultset(__FILE__LINE__ 16, 32), _bQuit(false)
	{
	}

	CServiceAsyncTCPClient(Ptr(CAsyncIOManager) pManager = nullptr, Ptr(CSqLite3Environment) env = nullptr ) :
		CAsyncTCPClient(pManager), _env(env), _conn(nullptr), _stmt(nullptr), _resultset(__FILE__LINE__ 16, 32), _bQuit(false)
	{
	}

	virtual ~CServiceAsyncTCPClient() 
	{
	}

	__inline bool get_Quit() const { return _bQuit; }

	void process_client_request(ConstRef(CStringBuffer) request, Ref(CByteBuffer) response)
	{
		CStringBuffer tmp(request);
		CPointer output[64] = { nullptr };
		dword outputsize = 0;
		CStringBuffer dbfile;

		tmp.Split(_T("\n"), output, 16, &outputsize);
		for (dword i = 0; i < outputsize; ++i)
		{
			if (s_strnicmp(output[i], _T("CONNECT:"), 8) == 0)
			{
				CStringBuffer tmp1(__FILE__LINE__ output[i] + 8);
				CPointer output1[16] = { nullptr };
				dword output1size = 0;
				CStringBuffer user;
				CStringBuffer passwd;
				dword state = 0;

				tmp1.SplitQuoted(_T("\""), _T(" "), output1, 16, &output1size);
				for (dword i = 0; i < output1size; i++)
				{
					if (!(StrEmpty(output1[i])))
					{
						switch (state)
						{
						case 0:
							user.SetString(__FILE__LINE__ output1[i]);
							++state;
							break;
						case 1:
							passwd.SetString(__FILE__LINE__ output1[i]);
							++state;
							break;
						case 2:
							dbfile.SetString(__FILE__LINE__ output1[i]);
							++state;
							break;
						default:
							break;
						}
					}
				}
				if (dbfile.IsEmpty())
				{
					CConstPointer pStr[] = {
						_T("CONNECT: FAILED \"Database file name is empty\"\n"),
						_T("DISCONNECT: QUIT\n")
					};
					for (dword i = 0; i < (sizeof(pStr) / sizeof(CConstPointer)); ++i)
					{
						CStringBuffer tmp(__FILE__LINE__ pStr[i]);
						CByteBuffer bBuf;

						tmp.convertToUTF8(bBuf, false);
						response.concat_Buffer(bBuf);
					}
					_bQuit = true;
					return;
				}
				try
				{
					if (_env)
						_conn = _env->create_Connection();
					if (_conn)
						_conn->Open(dbfile);

					CConstPointer pStr = _T("CONNECT: GRANTED\n");
					CStringBuffer tmp(__FILE__LINE__ pStr);
					CByteBuffer bBuf;

					tmp.convertToUTF8(bBuf, false);
					response.concat_Buffer(bBuf);
				}
				catch (CBaseException*)
				{
					CConstPointer pStr[] = {
						_T("CONNECT: FAILED \"Database file cannot be opened\"\n"),
						_T("DISCONNECT: QUIT\n")
					};
					for (dword i = 0; i < (sizeof(pStr) / sizeof(CConstPointer)); ++i)
					{
						CStringBuffer tmp(__FILE__LINE__ pStr[i]);
						CByteBuffer bBuf;

						tmp.convertToUTF8(bBuf, false);
						response.concat_Buffer(bBuf);
					}
					_bQuit = true;
					return;
				}
			}
			else if (s_strnicmp(output[i], _T("STATEMENT:"), 10) == 0)
			{
				CStringBuffer tmp1(__FILE__LINE__ output[i] + 10);

				if (_stmt)
				{
					_conn->free_Statement(_stmt);
					_stmt = nullptr;
				}
				while (_resultset.Count() > 0)
					_resultset.Remove(_resultset.Last());
				if (_conn)
				{
					CStringBuffer sBuf;
					CByteBuffer bBuf;

					_stmt = _conn->create_Statement();
					_stmt->Execute(tmp1);

					if (_stmt->get_ColumnCount() > 0)
					{
						_stmt->BindColumns();
						while (_stmt->Fetch())
						{
							CStringBuffer row;

							row << _T("ROWDATA: \"");
							for (word i = 0; i < _stmt->get_ColumnCount(); ++i)
							{
								CByteBuffer buf(_stmt->get_ColumnInfo(i)->get_Data());
								CStringBuffer tmp;

								switch (_stmt->get_ColumnInfo(i)->get_DataType())
								{
								case CSqLite3Column::eSQL_NULL:
									row << _T("(null)");
									break;
								case CSqLite3Column::eSQL_BLOB:
									tmp.FormatString(__FILE__LINE__ _T("%.*s"), buf.get_BufferSize(), buf.get_Buffer());
									row << tmp;
									break;
								case CSqLite3Column::eSQL_Float:
									row << DerefAnyPtr(double, buf.get_Buffer());
									break;
								case CSqLite3Column::eSQL_Integer:
									row << DerefSQWPointer(buf.get_Buffer());
									break;
								case CSqLite3Column::eSQL_Text:
									tmp.FormatString(__FILE__LINE__ _T("%.*s"), buf.get_BufferSize(), buf.get_Buffer());
									row << tmp;
									break;
								}
								if (i < (_stmt->get_ColumnCount() - 1))
									row << _T("\", \"");
							}
							row << _T("\"\n");
							_resultset.Append(row);
						}
						sBuf.FormatString(__FILE__LINE__ _T("RESULT: COLUMNS=%hd ROWS=%ld\n"), _stmt->get_ColumnCount(), _resultset.Count());
					}
					else
						sBuf.FormatString(__FILE__LINE__ _T("RESULT: COLUMNS=%hd ROWS=%lld\n"), _stmt->get_ColumnCount(), _stmt->get_RowCount());
					sBuf.convertToUTF8(bBuf, false);
					response.concat_Buffer(bBuf);
				}
			}
			else if (s_strnicmp(output[i], _T("FETCHROWS:"), 10) == 0)
			{
				CStringBuffer tmp1(__FILE__LINE__ output[i] + 10);
				CByteBuffer bBuf;
				CPointer output1[16] = { nullptr };
				dword output1size = 0;
				sqword vRow = 0;

				tmp1.Trim();
				tmp1.Split(_T("="), output1, 16, &output1size);
				vRow = s_strtoll(output1[1], nullptr, 10);
				tmp1 = _resultset.GetData(_resultset.Index(Cast(TListIndex, vRow)));
				tmp1.convertToUTF8(bBuf, false);
				response.concat_Buffer(bBuf);
			}
			else if (s_strnicmp(output[i], _T("DISCONNECT:"), 11) == 0)
			{
				CStringBuffer tmp1(__FILE__LINE__ output[i] + 11);

				tmp1.Trim();
				if (tmp1.Compare(_T("CLOSE"), 0, CStringLiteral::cIgnoreCase) == 0)
				{
					response.concat_Buffer(CastAnyPtr(byte, "DISCONNECT: QUIT\n"), 16);
					_bQuit = true;
					return;
				}
			}
		}
	}

	void reset_database()
	{
		if (_stmt)
		{
			_conn->free_Statement(_stmt);
			_stmt = nullptr;
		}
		if (_conn)
		{
			_conn->Close();
			_conn->release();
		}
		_env = nullptr;
	}

protected:
	Ptr(CSqLite3Environment) _env;
	Ptr(CSqLite3Connection) _conn;
	Ptr(CSqLite3Statement) _stmt;
	CDataVectorT<CStringBuffer> _resultset;
	bool _bQuit;
};

class AsyncTCPClientListEqualFunctor
{
public:
	bool operator()(ConstPtr(CServiceAsyncTCPClient) r1, ConstPtr(CServiceAsyncTCPClient) r2) const
	{
		return r1->GetData() == r2->GetData();
	}
};

typedef CDataDoubleLinkedListT<CServiceAsyncTCPClient> CAsyncTCPClientList;

class CServiceAsyncTCPServer
{
public:
	CServiceAsyncTCPServer() :
		io_manager(),
		server(&io_manager),
		clientlist(__FILE__LINE__0),
		_env(OK_NEW_OPERATOR CSqLite3Environment),
		_verbose(true)
	{
		_env->Open();
	}

	~CServiceAsyncTCPServer()
	{
	}

	dword accept_callback(Ptr(CAsyncIOData) pData)
	{
		CScopedLock lock;
		CByteBuffer buffer(__FILE__LINE__ 8192);
		Ptr(CServiceAsyncTCPClient) pClient = OK_NEW_OPERATOR CServiceAsyncTCPClient(&io_manager, _env);

		server.CreateClientConnection(pClient);
		clientlist.Append(pClient);
#ifdef __DEBUG__
		if (_verbose)
			CEventLogger::WriteFormattedLog(CEventLogger::Information, _T("XService: Client %llx Accepted"), pClient);
#endif
		lock.unlock();
		read_callback(pClient->GetData());
		try
		{
			server.Accept(buffer, OK_NEW_OPERATOR CAsyncIODataCallback<CServiceAsyncTCPServer>(this, &CServiceAsyncTCPServer::accept_callback, server.GetData()));
		}
		catch (CBaseException* ex)
		{
			CEventLogger::WriteLog(CEventLogger::Error, ex->GetExceptionMessage());
			io_manager.Stop();
			return 1;
		}
		return 0;
	}

	dword read_callback(Ptr(CAsyncIOData) pData)
	{
		CScopedLock lock;
		CServiceAsyncTCPClient client(pData);
		CAsyncTCPClientList::Iterator it = clientlist.Find<AsyncTCPClientListEqualFunctor>(&client);

		if (it)
		{
			Ptr(CServiceAsyncTCPClient) pClient = *it;

			if ((pData->get_bytestransferred() == 0) || (pData->get_errorcode() != 0))
			{
				pClient->Close();
				clientlist.Remove(it);
				return 1;
			}

			CStringBuffer tmp;
			CByteBuffer buffer;

			try
			{
				tmp.convertFromUTF8(pData->get_buffer(), false);
#ifdef __DEBUG__
				if (_verbose)
					CEventLogger::WriteFormattedLog(CEventLogger::Information, _T("XService: Client %llx read data (%s)"), pClient, tmp.GetString());
#endif
				pClient->process_client_request(tmp, buffer);

				if (buffer.get_BufferSize() > 0)
					pClient->Write(buffer, OK_NEW_OPERATOR CAsyncIODataCallback<CServiceAsyncTCPServer>(this, &CServiceAsyncTCPServer::write_callback, pData));
			}
			catch (CBaseException* ex)
			{
				CEventLogger::WriteLog(CEventLogger::Error, ex->GetExceptionMessage());
				pClient->Close();
				clientlist.Remove(it);
				return 1;
			}
		}
		return 0;
	}

	dword write_callback(Ptr(CAsyncIOData) pData)
	{
		CScopedLock lock;
		CServiceAsyncTCPClient client(pData);
		CAsyncTCPClientList::Iterator it = clientlist.Find<AsyncTCPClientListEqualFunctor>(&client);

		if (it)
		{
			Ptr(CServiceAsyncTCPClient) pClient = *it;

			if (pData->get_errorcode() != 0)
			{
				pClient->Close();
				clientlist.Remove(it);
				return 1;
			}

			CStringBuffer tmp;

			tmp.convertFromUTF8(pData->get_buffer(), false);

#ifdef __DEBUG__
			if (_verbose)
				CEventLogger::WriteFormattedLog(CEventLogger::Information, _T("XService: Client %llx data written (%s)"), pClient, tmp.GetString());
#endif
			if (pClient->get_Quit())
			{
				pClient->Close();
				pClient->reset_database();
				clientlist.Remove(it);
#ifdef __DEBUG__
			if (_verbose)
				CEventLogger::WriteFormattedLog(CEventLogger::Information, _T("XService: Client %llx removed"), pClient);
#endif
				return 0;
			}

			CByteBuffer buffer(__FILE__LINE__ 8192);

			try
			{
				pClient->Read(buffer, OK_NEW_OPERATOR CAsyncIODataCallback<CServiceAsyncTCPServer>(this, &CServiceAsyncTCPServer::read_callback, pData));
			}
			catch (CBaseException* ex)
			{
				CEventLogger::WriteLog(CEventLogger::Error, ex->GetExceptionMessage());
				pClient->Close();
				clientlist.Remove(it);
				return 1;
			}
		}
		return 0;
	}

	void Run()
	{
		CByteBuffer buffer(__FILE__LINE__ 8192);

		try
		{
			io_manager.Create();
		}
		catch (CBaseException* ex)
		{
			CEventLogger::WriteLog(CEventLogger::Error, ex->GetExceptionMessage());
			return;
		}
		try
		{
			server.Open(_T("localhost"), _T("6001"));
			server.Accept(buffer, OK_NEW_OPERATOR CAsyncIODataCallback<CServiceAsyncTCPServer>(this, &CServiceAsyncTCPServer::accept_callback, server.GetData()));
		}
		catch (CBaseException* ex)
		{
			CEventLogger::WriteLog(CEventLogger::Error, ex->GetExceptionMessage());
			io_manager.Stop();
		}
		io_manager.WaitForComplete();
		io_manager.Close();
	}

	void CancelAll()
	{
		CScopedLock lock;
		CAsyncTCPClientList::Iterator it = clientlist.Begin();

		while (it)
		{
			(*it)->Cancel();
			(*it)->Close();
			(*it)->reset_database();
			++it;
		}
		if (_env)
		{
			_env->Close();
			_env->release();
			_env = nullptr;
		}
		io_manager.Stop();
	}

protected:
	CAsyncIOManager io_manager;
	CAsyncTCPServer server;
	CAsyncTCPClientList clientlist;
	Ptr(CSqLite3Environment) _env;
	bool _verbose;
};

class CMain1Thread : public CThread
{
public:
	CMain1Thread() {}
	~CMain1Thread() {}

	virtual dword Run()
	{
		_server.Run();
		return 0;
	}

	void CancelAll() 
	{
		_server.CancelAll();
	}

protected:
	CServiceAsyncTCPServer _server;
};

static VOID WINAPI ServiceMain1(DWORD dwArgc, LPTSTR *lpszArgv)
{
	if (!gSCManager)
		return;

	Ptr(CService) pService = nullptr;
	Ptr(CMain1Thread) pMainThread = nullptr;

	try
	{
		gSCManager->Open(SERVICENAME1, &pService);
	}
	catch (CServiceException* ex)
	{
		CEventLogger::WriteLog(CEventLogger::Error, ex->GetExceptionMessage());
		return;
	}
	try
	{
		pService->Register();
		pMainThread = OK_NEW_OPERATOR CMain1Thread;
		pMainThread->Start();
		pService->SendStatus(CService::eServiceRunning, NO_ERROR, 0);
		CEventLogger::WriteFormattedLog(CEventLogger::Information, _T("Service %s is running."), pService->get_ServiceName().GetString());
		pService->GetStopSignal();
		pMainThread->CancelAll();
		pMainThread->Join();
		pService->SendStatus(CService::eServiceStopped, NO_ERROR, 0);
		CEventLogger::WriteFormattedLog(CEventLogger::Information, _T("Service %s is stopped."), pService->get_ServiceName().GetString());
	}
	catch (CServiceException* ex)
	{
		pService->SendStatus(CService::eServiceStopped, GetLastError(), 0);
		CEventLogger::WriteLog(CEventLogger::Error, ex->GetExceptionMessage());
	}
}

DWORD WINAPI ServiceMain1CtrlHandler(
	_In_  DWORD dwControl,
	_In_  DWORD dwEventType,
	_In_  LPVOID lpEventData,
	_In_  LPVOID lpContext
	)
{
	Ptr(CService) pService = CastAnyPtr(CService, lpContext);

	try
	{
		return pService->DoCtrlHandler(dwControl);
	}
	catch (CServiceException* ex)
	{
		CEventLogger::WriteLog(CEventLogger::Error, ex->GetExceptionMessage());
		pService->SendStopSignal();
	}

	return ERROR_CALL_NOT_IMPLEMENTED;
}

class CMain2Thread : public CThread
{
public:
	CMain2Thread() {}
	~CMain2Thread() {}

	virtual dword Run()
	{
		CStringBuffer value;
		CStringBuffer valueRequestQueueName;
		CAbstractConfiguration::Values values(__FILE__LINE__ 16, 16);
		CAbstractConfiguration::Values::Iterator it;
		CFilePath path;
		bool boolValue;

		valueRequestQueueName = theApp->config()->GetUserValue(_T("HttpServerService.RequestQueueName"), _T("XService"), _T("okreis"));
		_server.CreateRequestQueue(valueRequestQueueName);

		theApp->config()->GetUserValues(_T("HttpServerService.AddUrls"), _T("XService"), values);
		it = values.Begin();
		while (it)
		{
			_server.AddUrl(*it, 0);
			++it;
		}

		value = theApp->config()->GetUserValue(_T("HttpServerService.WorkerExe"), _T("XService"), _T("XHttpWorker.exe"));
		path.set_Path(value);

		boolValue = theApp->config()->GetUserBoolValue(_T("HttpServerService.Logging"), _T("XService"), false);

		value.FormatString(__FILE__LINE__ _T("-n %s%s"), valueRequestQueueName.GetString(), boolValue?_T(" -l"):_T(""));
		_server.RegisterWorker(path, value);

		if (boolValue)
			_server.SetupLogging(_T("HttpServerService"), _T("XService"));
		_server.RunServer();
		return 0;
	}

	void StopAll()
	{
		_server.StopServer();
	}

protected:
	CHttpServer _server;
};

static VOID WINAPI ServiceMain2(DWORD dwArgc, LPTSTR *lpszArgv)
{
	if (!gSCManager)
		return;

	Ptr(CService) pService = nullptr;
	Ptr(CMain2Thread) pMainThread = nullptr;

	try
	{
		gSCManager->Open(SERVICENAME2, &pService);
	}
	catch (CServiceException* ex)
	{
		CEventLogger::WriteLog(CEventLogger::Error, ex->GetExceptionMessage());
		return;
	}
	try
	{
		pService->Register();
		pMainThread = OK_NEW_OPERATOR CMain2Thread;
		pMainThread->Start();
		pService->SendStatus(CService::eServiceRunning, NO_ERROR, 0);
		CEventLogger::WriteFormattedLog(CEventLogger::Information, _T("Service %s is running."), pService->get_ServiceName().GetString());
		while (!(pService->GetStopSignal(250)))
		{
			DWORD ret = WaitForSingleObject(pMainThread->GetHandle(), 50);

			if (ret == WAIT_OBJECT_0)
			{
				pService->SendStatus(CService::eServiceStopped, ERROR_SERVICE_NO_THREAD, 0);
				CEventLogger::WriteFormattedLog(CEventLogger::Error, _T("Service %s is stopped abnormaly."), pService->get_ServiceName().GetString());
				return;
			}
			if (ret != WAIT_TIMEOUT)
			{
				pService->SendStatus(CService::eServiceStopped, GetLastError(), 0);
				CEventLogger::WriteFormattedLog(CEventLogger::Error, _T("Service %s is stopped abnormaly."), pService->get_ServiceName().GetString());
				return;
			}
		}
		pMainThread->StopAll();
		pMainThread->Join();
		pService->SendStatus(CService::eServiceStopped, NO_ERROR, 0);
		CEventLogger::WriteFormattedLog(CEventLogger::Information, _T("Service %s is stopped."), pService->get_ServiceName().GetString());
	}
	catch (CServiceException* ex)
	{
		pService->SendStatus(CService::eServiceStopped, GetLastError(), 0);
		CEventLogger::WriteLog(CEventLogger::Error, ex->GetExceptionMessage());
	}
}

DWORD WINAPI ServiceMain2CtrlHandler(
	_In_  DWORD dwControl,
	_In_  DWORD dwEventType,
	_In_  LPVOID lpEventData,
	_In_  LPVOID lpContext
	)
{
	Ptr(CService) pService = CastAnyPtr(CService, lpContext);

	try
	{
		return pService->DoCtrlHandler(dwControl);
	}
	catch (CServiceException* ex)
	{
		CEventLogger::WriteLog(CEventLogger::Error, ex->GetExceptionMessage());
		pService->SendStopSignal();
	}

	return ERROR_CALL_NOT_IMPLEMENTED;
}

CServiceInfoTable DispatchTable[] =
{
	{ SERVICENAME1, OK_NEW_OPERATOR CServiceInfo(SERVICENAME1, SERVICEDISPLAYNAME1, SERVICEDESCRIPTION1,
	CServiceInfo::eMultiServices, CServiceInfo::eManualStartType, CServiceInfo::eErrorNormal,
	nullptr, nullptr, ServiceMain1, ServiceMain1CtrlHandler) },
	{ SERVICENAME2, OK_NEW_OPERATOR CServiceInfo(SERVICENAME2, SERVICEDISPLAYNAME2, SERVICEDESCRIPTION2,
	CServiceInfo::eMultiServices, CServiceInfo::eManualStartType, CServiceInfo::eErrorNormal,
	nullptr, nullptr, ServiceMain2, ServiceMain2CtrlHandler) },
	{ nullptr, nullptr }
};


void okInstallServices(void)
{
	if (!gSCManager)
		return;
	try
	{
		gSCManager->StartUp();
		gSCManager->Install(DispatchTable);
		gSCManager->CleanUp();
		CEventLogger::CleanUp();
	}
	catch (CServiceException* ex)
	{
		CEventLogger::WriteLog(CEventLogger::Error, ex->GetExceptionMessage());
	}
}

void okStartServices(ConstRef(TMBCharList) services)
{
	if (!gSCManager)
		return;
	try
	{
		gSCManager->StartUp();
		gSCManager->Load(DispatchTable);
		gSCManager->Run(services);
		gSCManager->CleanUp();
		CEventLogger::CleanUp();
	}
	catch (CServiceException* ex)
	{
		CEventLogger::WriteLog(CEventLogger::Error, ex->GetExceptionMessage());
	}
}
