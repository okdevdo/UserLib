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
#include "HttpServer.h"
#include "Application.h"
#include "Configuration.h"
#include "EventLogger.h"
#include "ScopedLock.h"
#include "UTLPTR.H"
#include <Http.h>
#include <aclapi.h>

IMPL_WINEXCEPTION(CHttpServerException, CWinException)

static void HandleError(ULONG cmd, DECL_FILE_LINE CConstPointer innerfunc, CConstPointer outerfunc)
{
	if (NOERROR != cmd)
		throw OK_NEW_OPERATOR CHttpServerException(ARGS_FILE_LINE _T("[%s] in %s failed"), innerfunc, outerfunc, CWinException::WinExtError, cmd);
}

static void HandleError2(BOOL cmd, DECL_FILE_LINE CConstPointer innerfunc, CConstPointer outerfunc)
{
	if (!cmd)
		throw OK_NEW_OPERATOR CHttpServerException(ARGS_FILE_LINE _T("[%s] in %s failed"), innerfunc, outerfunc, CWinException::WinExtError);
}

typedef struct __tagHttpServerData
{
	HTTPAPI_VERSION version;
	PSID pLocalSID;
	PACL pACL;
	PSECURITY_DESCRIPTOR pSD;
	SECURITY_ATTRIBUTES sa;
	HANDLE hQueue;
	HTTP_SERVER_SESSION_ID ssID;
	HTTP_URL_GROUP_ID ugID;
	CFilePath workerExe;
	CStringBuffer ExeArgs;
	bool StopSignal;
	OVERLAPPED ov;
	PROCESS_INFORMATION pi[MAXIMUM_WAIT_OBJECTS];
	WORD pim;
	HANDLE pw[MAXIMUM_WAIT_OBJECTS];
	WORD pwm;
} THttpServerData;

#define SDATA(c) CastAnyPtr(THttpServerData, _data)->c

typedef struct __tagHttpWorkerData
{
	HTTPAPI_VERSION version;
	HANDLE hQueue;
	PHTTP_REQUEST hrq;
	CHttpServer::StatusCodeList* statuscodelist;
} THttpWorkerData;

#define WDATA(c) CastAnyPtr(THttpWorkerData, _data)->c

static const char* HttpRequestHeader[HttpHeaderRequestMaximum] = {
	"CacheControl",
	"Connection",
	"Date",
	"KeepAlive",
	"Pragma",
	"Trailer",
	"TransferEncoding",
	"Upgrade",
	"Via",
	"Warning",
	"Allow",
	"ContentLength",
	"ContentType",
	"ContentEncoding",
	"ContentLanguage",
	"ContentLocation",
	"ContentMd5",
	"ContentRange",
	"Expires",
	"LastModified",
	"Accept",
	"AcceptCharset",
	"AcceptEncoding",
	"AcceptLanguage",
	"Authorization",
	"Cookie",
	"Expect",
	"From",
	"Host",
	"IfMatch",
	"IfModifiedSince",
	"IfNoneMatch",
	"IfRange",
	"IfUnmodifiedSince",
	"MaxForwards",
	"ProxyAuthorization",
	"Referer",
	"Range",
	"Te",
	"Translate",
	"UserAgent"
};

static struct {
	dword sz;
	const char* pS;
} HttpResponseHeader[HttpHeaderResponseMaximum] = {
	{ 12, "CacheControl" },
	{ 10, "Connection" },
	{ 4, "Date" },
	{ 9, "KeepAlive" },
	{ 6, "Pragma" },
	{ 7, "Trailer" },
	{ 16, "TransferEncoding" },
	{ 7, "Upgrade" },
	{ 3, "Via" },
	{ 7, "Warning" },
	{ 5, "Allow" },
	{ 13, "ContentLength" },
	{ 11, "ContentType" },
	{ 15, "ContentEncoding" },
	{ 15, "ContentLanguage" },
	{ 15, "ContentLocation" },
	{ 10, "ContentMd5" },
	{ 12, "ContentRange" },
	{ 7, "Expires" },
	{ 12, "LastModified" },
	{ 12, "AcceptRanges" },
	{ 3, "Age" },
	{ 4, "Etag" },
	{ 8, "Location" },
	{ 17, "ProxyAuthenticate" },
	{ 10, "RetryAfter" },
	{ 6, "Server" },
	{ 9, "SetCookie" },
	{ 4, "Vary" },
	{ 15, "WwwAuthenticate" }
};

static int GetHttpResponseHeaderNum(ConstRef(CByteBuffer) buf)
{
	for (int i = 0; i < HttpHeaderResponseMaximum; ++i)
		if ((HttpResponseHeader[i].sz == buf.get_BufferSize()) && (strnicmp(HttpResponseHeader[i].pS, CastAnyPtr(char, buf.get_Buffer()), buf.get_BufferSize()) == 0))
			return i;
	return -1;
}

static void FillStatusCodeList(Ptr(CHttpServer::StatusCodeList) list)
{
	list->insert(CHttpServer::StatusCode(100, _T("Continue")));
	list->insert(CHttpServer::StatusCode(101, _T("Switching Protocols")));
	list->insert(CHttpServer::StatusCode(102, _T("Processing")));
	list->insert(CHttpServer::StatusCode(200, _T("OK")));
	list->insert(CHttpServer::StatusCode(201, _T("Created")));
	list->insert(CHttpServer::StatusCode(202, _T("Accepted")));
	list->insert(CHttpServer::StatusCode(203, _T("Non-Authoritative Information")));
	list->insert(CHttpServer::StatusCode(204, _T("No Content")));
	list->insert(CHttpServer::StatusCode(205, _T("Reset Content")));
	list->insert(CHttpServer::StatusCode(206, _T("Partial Content")));
	list->insert(CHttpServer::StatusCode(207, _T("Multi-Status")));
	list->insert(CHttpServer::StatusCode(208, _T("Already Reported")));
	list->insert(CHttpServer::StatusCode(226, _T("IM Used")));
	list->insert(CHttpServer::StatusCode(300, _T("Multiple Choices")));
	list->insert(CHttpServer::StatusCode(301, _T("Moved Permanently")));
	list->insert(CHttpServer::StatusCode(302, _T("Found")));
	list->insert(CHttpServer::StatusCode(303, _T("See Other")));
	list->insert(CHttpServer::StatusCode(304, _T("Not Modified")));
	list->insert(CHttpServer::StatusCode(305, _T("Use Proxy")));
	list->insert(CHttpServer::StatusCode(307, _T("Temporary Redirect")));
	list->insert(CHttpServer::StatusCode(308, _T("Permanent Redirect")));
	list->insert(CHttpServer::StatusCode(400, _T("Bad Request")));
	list->insert(CHttpServer::StatusCode(401, _T("Unauthorized")));
	list->insert(CHttpServer::StatusCode(402, _T("Payment Required")));
	list->insert(CHttpServer::StatusCode(403, _T("Forbidden")));
	list->insert(CHttpServer::StatusCode(404, _T("Not Found")));
	list->insert(CHttpServer::StatusCode(405, _T("Method Not Allowed")));
	list->insert(CHttpServer::StatusCode(406, _T("Not Acceptable")));
	list->insert(CHttpServer::StatusCode(407, _T("Proxy Authentication Required")));
	list->insert(CHttpServer::StatusCode(408, _T("Request Time-out")));
	list->insert(CHttpServer::StatusCode(409, _T("Conflict")));
	list->insert(CHttpServer::StatusCode(410, _T("Gone")));
	list->insert(CHttpServer::StatusCode(411, _T("Length Required")));
	list->insert(CHttpServer::StatusCode(412, _T("Precondition Failed")));
	list->insert(CHttpServer::StatusCode(413, _T("Request Entity Too Large")));
	list->insert(CHttpServer::StatusCode(414, _T("Request-URL Too Long")));
	list->insert(CHttpServer::StatusCode(415, _T("Unsupported Media Type")));
	list->insert(CHttpServer::StatusCode(416, _T("Requested range not satisfiable")));
	list->insert(CHttpServer::StatusCode(417, _T("Expectation Failed")));
	list->insert(CHttpServer::StatusCode(418, _T("I’m a teapot")));
	list->insert(CHttpServer::StatusCode(420, _T("Policy Not Fulfilled")));
	list->insert(CHttpServer::StatusCode(421, _T("There are too many connections from your internet address")));
	list->insert(CHttpServer::StatusCode(422, _T("Unprocessable Entity")));
	list->insert(CHttpServer::StatusCode(423, _T("Locked")));
	list->insert(CHttpServer::StatusCode(424, _T("Failed Dependency")));
	list->insert(CHttpServer::StatusCode(425, _T("Unordered Collection")));
	list->insert(CHttpServer::StatusCode(426, _T("Upgrade Required")));
	list->insert(CHttpServer::StatusCode(428, _T("Precondition Required")));
	list->insert(CHttpServer::StatusCode(429, _T("Too Many Requests")));
	list->insert(CHttpServer::StatusCode(431, _T("Request Header Fields Too Large")));
	list->insert(CHttpServer::StatusCode(500, _T("Internal Server Error")));
	list->insert(CHttpServer::StatusCode(501, _T("Not Implemented")));
	list->insert(CHttpServer::StatusCode(502, _T("Bad Gateway")));
	list->insert(CHttpServer::StatusCode(503, _T("Service Unavailable")));
	list->insert(CHttpServer::StatusCode(504, _T("Gateway Time-out")));
	list->insert(CHttpServer::StatusCode(505, _T("HTTP Version not supported")));
	list->insert(CHttpServer::StatusCode(506, _T("Variant Also Negotiates")));
	list->insert(CHttpServer::StatusCode(507, _T("Insufficient Storage")));
	list->insert(CHttpServer::StatusCode(508, _T("Loop Detected")));
	list->insert(CHttpServer::StatusCode(509, _T("Bandwidth Limit Exceeded")));
	list->insert(CHttpServer::StatusCode(510, _T("Not Extended")));
}

CHttpServer::CHttpServer(bool bWorker):
_worker(bWorker), _data(nullptr)
{
	Initialize(bWorker);
}

CHttpServer::~CHttpServer(void)
{
	Uninitialize();
}

void CHttpServer::Initialize(bool bWorker)
{
	HTTPAPI_VERSION v = HTTPAPI_VERSION_2;

	if (bWorker)
	{
		_data = TFalloc(sizeof(THttpWorkerData));
		WDATA(version) = v;
		WDATA(statuscodelist) = new CHttpServer::StatusCodeList(150);
		FillStatusCodeList(WDATA(statuscodelist));
		HandleError(HttpInitialize(WDATA(version), HTTP_INITIALIZE_SERVER, nullptr), __FILE__LINE__ _T("HttpInitialize"), _T("CHttpServer::Initialize"));
	}
	else
	{
		_data = TFalloc(sizeof(THttpServerData));
		SDATA(version) = v;
		HandleError(HttpInitialize(SDATA(version), HTTP_INITIALIZE_SERVER, nullptr), __FILE__LINE__ _T("HttpInitialize"), _T("CHttpServer::Initialize"));
	}
	_worker = bWorker;
}

void CHttpServer::Uninitialize()
{
	CloseRequestQueue();
	if (_worker)
	{
		if (WDATA(hrq))
			TFfree(WDATA(hrq));
		if (WDATA(statuscodelist))
			delete WDATA(statuscodelist);
		if (SDATA(ov).hEvent)
			CloseHandle(SDATA(ov).hEvent);
	}
	else
	{
		SDATA(workerExe) = CFilePath();
		SDATA(ExeArgs).Clear();
		if (SDATA(pLocalSID))
			LocalFree(SDATA(pLocalSID));
		if (SDATA(pACL))
			LocalFree(SDATA(pACL));
		if (SDATA(pSD))
			LocalFree(SDATA(pSD));
		if (SDATA(ugID))
			HandleError(HttpCloseUrlGroup(SDATA(ugID)), __FILE__LINE__ _T("HttpCloseUrlGroup"), _T("CHttpServer::Uninitialize"));
		if (SDATA(ssID))
			HandleError(HttpCloseServerSession(SDATA(ssID)), __FILE__LINE__ _T("HttpCloseServerSession"), _T("CHttpServer::Uninitialize"));
	}
	HandleError(HttpTerminate(HTTP_INITIALIZE_SERVER, nullptr), __FILE__LINE__ _T("HttpTerminate"), _T("CHttpServer::Uninitialize"));
}

void CHttpServer::ShutdownRequestQueue()
{
	if (_worker)
	{
		if (WDATA(hQueue))
			HandleError(HttpShutdownRequestQueue(WDATA(hQueue)), __FILE__LINE__ _T("HttpShutdownRequestQueue"), _T("CHttpServer::ShutdownRequestQueue"));
	}
	else
	{
		if (SDATA(hQueue))
			HandleError(HttpShutdownRequestQueue(SDATA(hQueue)), __FILE__LINE__ _T("HttpShutdownRequestQueue"), _T("CHttpServer::ShutdownRequestQueue"));
	}
}

void CHttpServer::CloseRequestQueue()
{
	if (_worker)
	{
		if (WDATA(hQueue))
		{
			HandleError(HttpCloseRequestQueue(WDATA(hQueue)), __FILE__LINE__ _T("HttpCloseRequestQueue"), _T("CHttpServer::ShutdownRequestQueue"));
			WDATA(hQueue) = nullptr;
		}
	}
	else
	{
		if (SDATA(hQueue))
		{
			HandleError(HttpCloseRequestQueue(SDATA(hQueue)), __FILE__LINE__ _T("HttpCloseRequestQueue"), _T("CHttpServer::ShutdownRequestQueue"));
			SDATA(hQueue) = nullptr;
		}
	}
}

void CHttpServer::CreateRequestQueue(CConstPointer name)
{
	if (_worker)
		throw OK_NEW_OPERATOR CHttpServerException(__FILE__LINE__ _T("Program sequence error in %s"), _T("CHttpServer::CreateRequestQueue"), CWinException::WinExtError, ERROR_REQUEST_OUT_OF_SEQUENCE);
	if (PtrCheck(name))
		throw OK_NEW_OPERATOR CHttpServerException(__FILE__LINE__ _T("Parameter error in %s"), _T("CHttpServer::CreateRequestQueue"), CWinException::WinExtError, ERROR_INVALID_PARAMETER);

	EXPLICIT_ACCESS sea;
	DWORD dwLength;

	dwLength = SECURITY_MAX_SID_SIZE;
	SDATA(pLocalSID) = LocalAlloc(LMEM_FIXED, dwLength);
	HandleError2(!!SDATA(pLocalSID), __FILE__LINE__ _T("LocalAlloc"), _T("CHttpServer::CreateRequestQueue"));
	HandleError2(CreateWellKnownSid(WinWorldSid, nullptr, SDATA(pLocalSID), &dwLength), __FILE__LINE__ _T("CreateWellKnownSid"), _T("CHttpServer::CreateRequestQueue"));

	ZeroMemory(&sea, sizeof(EXPLICIT_ACCESS));
	sea.grfAccessPermissions = STANDARD_RIGHTS_ALL | SPECIFIC_RIGHTS_ALL;
	sea.grfAccessMode = SET_ACCESS;
	sea.grfInheritance = NO_INHERITANCE;
	sea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
	sea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	sea.Trustee.ptstrName = (LPTSTR)(SDATA(pLocalSID));

	HandleError(SetEntriesInAcl(1, &sea, nullptr, &(SDATA(pACL))), __FILE__LINE__ _T("SetEntriesInAcl"), _T("CHttpServer::CreateRequestQueue"));

	SDATA(pSD) = (PSECURITY_DESCRIPTOR)LocalAlloc(LMEM_FIXED, SECURITY_DESCRIPTOR_MIN_LENGTH);
	HandleError2(!!SDATA(pSD), __FILE__LINE__ _T("LocalAlloc"), _T("CHttpServer::CreateRequestQueue"));
	HandleError2(InitializeSecurityDescriptor(SDATA(pSD), SECURITY_DESCRIPTOR_REVISION), __FILE__LINE__ _T("InitializeSecurityDescriptor"), _T("CHttpServer::CreateRequestQueue"));
	HandleError2(SetSecurityDescriptorDacl(SDATA(pSD), TRUE, SDATA(pACL), FALSE), __FILE__LINE__ _T("SetSecurityDescriptorDacl"), _T("CHttpServer::CreateRequestQueue"));

	SDATA(sa).nLength = sizeof (SECURITY_ATTRIBUTES);
	SDATA(sa).lpSecurityDescriptor = SDATA(pSD);
	SDATA(sa).bInheritHandle = FALSE;

	HandleError(HttpCreateRequestQueue(SDATA(version), name, &(SDATA(sa)), HTTP_CREATE_REQUEST_QUEUE_FLAG_CONTROLLER, &(SDATA(hQueue))), __FILE__LINE__ _T("HttpCreateRequestQueue"), _T("CHttpServer::CreateRequestQueue"));

	HTTP_503_RESPONSE_VERBOSITY v = Http503ResponseVerbosityLimited;

	HandleError(HttpSetRequestQueueProperty(SDATA(hQueue), HttpServer503VerbosityProperty, &v, sizeof(HTTP_503_RESPONSE_VERBOSITY), 0, nullptr), __FILE__LINE__ _T("HttpSetRequestQueueProperty"), _T("CHttpServer::CreateRequestQueue"));

	ULONG n = 16;

	HandleError(HttpSetRequestQueueProperty(SDATA(hQueue), HttpServerQueueLengthProperty, &n, sizeof(ULONG), 0, nullptr), __FILE__LINE__ _T("HttpSetRequestQueueProperty"), _T("CHttpServer::CreateRequestQueue"));
}

void CHttpServer::AddUrl(ConstRef(CStringBuffer) url, qword context)
{
	if (_worker)
		throw OK_NEW_OPERATOR CHttpServerException(__FILE__LINE__ _T("Program sequence error in %s"), _T("CHttpServer::AddUrl"), CWinException::WinExtError, ERROR_REQUEST_OUT_OF_SEQUENCE);
	if (url.IsEmpty())
		throw OK_NEW_OPERATOR CHttpServerException(__FILE__LINE__ _T("Parameter error in %s"), _T("CHttpServer::AddUrl"), CWinException::WinExtError, ERROR_INVALID_PARAMETER);

	if (!(SDATA(ssID)))
		HandleError(HttpCreateServerSession(SDATA(version), &(SDATA(ssID)), 0), __FILE__LINE__ _T("HttpCreateServerSession"), _T("CHttpServer::AddUrl"));
	if (!(SDATA(ugID)))
		HandleError(HttpCreateUrlGroup(SDATA(ssID), &(SDATA(ugID)), 0), __FILE__LINE__ _T("HttpCreateUrlGroup"), _T("CHttpServer::AddUrl"));
	HandleError(HttpAddUrlToUrlGroup(SDATA(ugID), url.GetString(), context, 0), __FILE__LINE__ _T("HttpAddUrlToUrlGroup"), _T("CHttpServer::AddUrl"));
}

void CHttpServer::RegisterWorker(ConstRef(CFilePath) path, CConstPointer args)
{
	if (_worker)
		throw OK_NEW_OPERATOR CHttpServerException(__FILE__LINE__ _T("Program sequence error in %s"), _T("CHttpServer::RegisterWorker"), CWinException::WinExtError, ERROR_REQUEST_OUT_OF_SEQUENCE);
	if (path.IsEmpty())
		throw OK_NEW_OPERATOR CHttpServerException(__FILE__LINE__ _T("Parameter error in %s"), _T("CHttpServer::RegisterWorker"), CWinException::WinExtError, ERROR_INVALID_PARAMETER);
	SDATA(workerExe) = path;
	SDATA(ExeArgs).SetString(__FILE__LINE__ args);
}

void CHttpServer::SetupLogging(CConstPointer logConfigPathPrefix, CConstPointer _defaultappname)
{
	if (_worker || (!(SDATA(ssID))))
		throw OK_NEW_OPERATOR CHttpServerException(__FILE__LINE__ _T("Program sequence error in %s"), _T("CHttpServer::SetLogging"), CWinException::WinExtError, ERROR_REQUEST_OUT_OF_SEQUENCE);
	if (PtrCheck(logConfigPathPrefix))
		throw OK_NEW_OPERATOR CHttpServerException(__FILE__LINE__ _T("Parameter error in %s"), _T("CHttpServer::SetLogging"), CWinException::WinExtError, ERROR_INVALID_PARAMETER);

	HTTP_LOGGING_INFO li;
	CStringBuffer configName;
	CStringBuffer value;
	CAbstractConfiguration::Values values(__FILE__LINE__ 16, 16);
	CAbstractConfiguration::Values::Iterator it;

	ZeroMemory(&li, sizeof(HTTP_LOGGING_INFO));
	li.Flags.Present = 1;
	li.LoggingFlags = HTTP_LOGGING_FLAG_USE_UTF8_CONVERSION;
	li.Format = HttpLoggingTypeW3C;

	configName.FormatString(__FILE__LINE__ _T("%s.LoggingDirectory"), logConfigPathPrefix);
	value = theApp->config()->GetUserValue(configName, _defaultappname);

	if (value.IsEmpty())
		value = theApp->config()->GetValue(_T("System.Environment.USERPROFILE"));
	if (value.IsEmpty())
		throw OK_NEW_OPERATOR CHttpServerException(__FILE__LINE__ _T("Cannot find logging directory in %s"), _T("CHttpServer::SetLogging"), CWinException::WinExtError, ERROR_INVALID_DATA);

	li.DirectoryName = value.GetString();
	li.DirectoryNameLength = Castword(value.GetLength() * szchar);

	configName.FormatString(__FILE__LINE__ _T("%s.LoggingFields"), logConfigPathPrefix);
	theApp->config()->GetUserValues(configName, _defaultappname, values);

	it = values.Begin();
	li.Fields = 0;
	while (it)
	{
		value = *it;

		if (value.Compare(_T("DATE"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_DATE;
		else if (value.Compare(_T("TIME"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_TIME;
		else if (value.Compare(_T("CLIENT_IP"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_CLIENT_IP;
		else if (value.Compare(_T("USER_NAME"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_USER_NAME;
		else if (value.Compare(_T("SITE_NAME"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_SITE_NAME;
		else if (value.Compare(_T("COMPUTER_NAME"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_COMPUTER_NAME;
		else if (value.Compare(_T("SERVER_IP"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_SERVER_IP;
		else if (value.Compare(_T("METHOD"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_METHOD;
		else if (value.Compare(_T("URI_STEM"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_URI_STEM;
		else if (value.Compare(_T("URI_QUERY"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_URI_QUERY;
		else if (value.Compare(_T("STATUS"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_STATUS;
		else if (value.Compare(_T("WIN32_STATUS"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_WIN32_STATUS;
		else if (value.Compare(_T("BYTES_SENT"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_BYTES_SENT;
		else if (value.Compare(_T("BYTES_RECV"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_BYTES_RECV;
		else if (value.Compare(_T("TIME_TAKEN"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_TIME_TAKEN;
		else if (value.Compare(_T("SERVER_PORT"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_SERVER_PORT;
		else if (value.Compare(_T("USER_AGENT"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_USER_AGENT;
		else if (value.Compare(_T("COOKIE"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_COOKIE;
		else if (value.Compare(_T("REFERER"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_REFERER;
		else if (value.Compare(_T("VERSION"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_VERSION;
		else if (value.Compare(_T("HOST"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_HOST;
		else if (value.Compare(_T("SUB_STATUS"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_SUB_STATUS;
		else if (value.Compare(_T("CLIENT_PORT"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_CLIENT_PORT;
		else if (value.Compare(_T("URI"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_URI;
		else if (value.Compare(_T("SITE_ID"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_SITE_ID;
		else if (value.Compare(_T("REASON"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_REASON;
		else if (value.Compare(_T("QUEUE_NAME"), 0, CStringLiteral::cIgnoreCase) == 0)
			li.Fields |= HTTP_LOG_FIELD_QUEUE_NAME;
		++it;
	}

	configName.FormatString(__FILE__LINE__ _T("%s.LoggingRollOverType"), logConfigPathPrefix);
	value = theApp->config()->GetUserValue(configName, _defaultappname);

	if (value.IsEmpty())
		li.RolloverType = HttpLoggingRolloverDaily;
	else if (value.Compare(_T("Size"), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		li.RolloverType = HttpLoggingRolloverSize;
		configName.FormatString(__FILE__LINE__ _T("%s.LoggingRollOverSize"), logConfigPathPrefix);
		value = theApp->config()->GetUserValue(configName, _defaultappname);
		if (value.IsEmpty())
			li.RolloverSize = HTTP_MIN_ALLOWED_LOG_FILE_ROLLOVER_SIZE;
		else
		{
			if (value.Compare(_T("INFINITE"), 0, CStringLiteral::cIgnoreCase) == 0)
				li.RolloverSize = HTTP_LIMIT_INFINITE;
			else
			{
				ULONG limit = 0;

				if (value.ScanString(_T("%lu"), &limit) < 0)
					li.RolloverSize = HTTP_MIN_ALLOWED_LOG_FILE_ROLLOVER_SIZE;
				else
				{
					li.RolloverSize = limit;
					if (li.RolloverSize < HTTP_MIN_ALLOWED_LOG_FILE_ROLLOVER_SIZE)
						li.RolloverSize = HTTP_MIN_ALLOWED_LOG_FILE_ROLLOVER_SIZE;
				}
			}
		}
	}
	else if (value.Compare(_T("Daily"), 0, CStringLiteral::cIgnoreCase) == 0)
		li.RolloverType = HttpLoggingRolloverDaily;
	else if (value.Compare(_T("Weekly"), 0, CStringLiteral::cIgnoreCase) == 0)
		li.RolloverType = HttpLoggingRolloverWeekly;
	else if (value.Compare(_T("Monthly"), 0, CStringLiteral::cIgnoreCase) == 0)
		li.RolloverType = HttpLoggingRolloverMonthly;
	else if (value.Compare(_T("Hourly"), 0, CStringLiteral::cIgnoreCase) == 0)
		li.RolloverType = HttpLoggingRolloverHourly;
	else
		li.RolloverType = HttpLoggingRolloverDaily;
	
	HandleError(HttpSetServerSessionProperty(SDATA(ssID), HttpServerLoggingProperty, &li, sizeof(HTTP_LOGGING_INFO)), __FILE__LINE__ _T("HttpSetServerSessionProperty"), _T("CHttpServer::SetLogging"));
}

void CHttpServer::RunServer()
{
	if (_worker || (!(SDATA(ugID))) || (!(SDATA(hQueue))) || (SDATA(workerExe).IsEmpty()))
		throw OK_NEW_OPERATOR CHttpServerException(__FILE__LINE__ _T("Program sequence error in %s"), _T("CHttpServer::RunServer"), CWinException::WinExtError, ERROR_REQUEST_OUT_OF_SEQUENCE);

	HTTP_BINDING_INFO bi;
	bool bWaitForDemandStart = true;
	ULONG ret;

	bi.Flags.Present = 1;
	bi.RequestQueueHandle = SDATA(hQueue);
	HandleError(HttpSetUrlGroupProperty(SDATA(ugID), HttpServerBindingProperty, &bi, sizeof(HTTP_BINDING_INFO)), __FILE__LINE__ _T("HttpSetUrlGroupProperty"), _T("CHttpServer::RunServer"));

	ZeroMemory(&(SDATA(ov)), sizeof(OVERLAPPED));
	SDATA(ov).hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	HandleError2(!!(SDATA(ov).hEvent), __FILE__LINE__ _T("CreateEvent"), _T("CHttpServer::RunServer"));
	SDATA(pw)[0] = SDATA(ov).hEvent;
	SDATA(pwm) = 1;
	while (true)
	{
		if (bWaitForDemandStart)
		{
			ret = HttpWaitForDemandStart(SDATA(hQueue), &(SDATA(ov)));
			if (ret != ERROR_IO_PENDING)
				HandleError(ret, __FILE__LINE__ _T("HttpWaitForDemandStart"), _T("CHttpServer::RunServer"));
		}
		ret = WaitForMultipleObjects(SDATA(pwm), SDATA(pw), FALSE, INFINITE);
		if (ret == WAIT_FAILED)
			HandleError2(FALSE, __FILE__LINE__ _T("WaitForMultipleObjects"), _T("CHttpServer::RunServer"));

		CScopedLock lock;

		if (SDATA(StopSignal))
			break;
		lock.unlock();

		bWaitForDemandStart = false;
		if (ret == WAIT_OBJECT_0)
		{
			STARTUPINFO si;
			PROCESS_INFORMATION pi;

			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
			if (SDATA(ExeArgs).IsEmpty())
				HandleError2(CreateProcess(SDATA(workerExe).GetString(), nullptr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi), __FILE__LINE__ _T("CreateProcess"), _T("CHttpServer::RunServer"));
			else
			{
				CStringBuffer cmd;

				cmd.FormatString(__FILE__LINE__ _T("\"%s\" %s"), SDATA(workerExe).GetString(), SDATA(ExeArgs).GetString());
				HandleError2(CreateProcess(nullptr, CastMutable(CPointer, cmd.GetString()), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi), __FILE__LINE__ _T("CreateProcess"), _T("CHttpServer::RunServer"));
			}
			if (SDATA(pim) < (MAXIMUM_WAIT_OBJECTS - 1))
			{
				SDATA(pi)[SDATA(pim)] = pi;
				++(SDATA(pim));
				SDATA(pw)[SDATA(pwm)] = pi.hProcess;
				++(SDATA(pwm));
			}
			else
			{
				WaitForSingleObject(pi.hProcess, INFINITE);

				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}
			bWaitForDemandStart = true;
		}
		else if ((ret > WAIT_OBJECT_0) && (ret < (WAIT_OBJECT_0 + SDATA(pwm))))
		{
			DWORD i = ret - WAIT_OBJECT_0;

			CloseHandle(SDATA(pi)[i - 1].hProcess);
			CloseHandle(SDATA(pi)[i - 1].hThread);
			for (; i < Castdword((SDATA(pwm) - 1)); ++i)
			{
				SDATA(pw)[i] = SDATA(pw)[i + 1];
				SDATA(pi)[i - 1] = SDATA(pi)[i];
			}
			--(SDATA(pwm));
			--(SDATA(pim));
		}
	}
	if (SDATA(pwm) > 1)
	{
		for (DWORD i = 1; i < SDATA(pwm); ++i)
			TerminateProcess(SDATA(pw)[i], -4);
		WaitForMultipleObjects(SDATA(pwm) - 1, &(SDATA(pw)[1]), TRUE, INFINITE);
		for (DWORD i = 0; i < SDATA(pim); ++i)
		{
			CloseHandle(SDATA(pi)[i].hProcess);
			CloseHandle(SDATA(pi)[i].hThread);
		}
	}
}

void CHttpServer::StopServer()
{
	if (_worker || PtrCheck(SDATA(ov).hEvent))
		throw OK_NEW_OPERATOR CHttpServerException(__FILE__LINE__ _T("Program sequence error in %s"), _T("CHttpServer::RunServer"), CWinException::WinExtError, ERROR_REQUEST_OUT_OF_SEQUENCE);

	CScopedLock lock;

	SDATA(StopSignal) = true;
	SetEvent(SDATA(ov).hEvent);
}

void CHttpServer::OpenRequestQueue(CConstPointer name)
{
	if (!_worker)
		throw OK_NEW_OPERATOR CHttpServerException(__FILE__LINE__ _T("Program sequence error in %s"), _T("CHttpServer::OpenRequestQueue"), CWinException::WinExtError, ERROR_REQUEST_OUT_OF_SEQUENCE);

	HandleError(HttpCreateRequestQueue(WDATA(version), name, nullptr, HTTP_CREATE_REQUEST_QUEUE_FLAG_OPEN_EXISTING, &(WDATA(hQueue))), __FILE__LINE__ _T("HttpCreateRequestQueue"), _T("CHttpServer::OpenRequestQueue"));
}

void CHttpServer::ReceiveRequestHeader(Ref(RequestDataList) header, DWORD timeout)
{
	if ((!_worker) || (!(WDATA(hQueue))))
		throw OK_NEW_OPERATOR CHttpServerException(__FILE__LINE__ _T("Program sequence error in %s"), _T("CHttpServer::OpenRequestQueue"), CWinException::WinExtError, ERROR_REQUEST_OUT_OF_SEQUENCE);

	ULONG res;
	ULONG ret;

	res = 4096;
	WDATA(hrq) = CastAnyPtr(HTTP_REQUEST, TFalloc(res));
	if (timeout != INFINITE)
	{
		OVERLAPPED ov;

		ZeroMemory(&ov, sizeof(OVERLAPPED));
		ov.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
		if (PtrCheck(ov.hEvent))
			HandleError2(FALSE, __FILE__LINE__ _T("CreateEvent"), _T("CHttpServer::OpenRequestQueue"));
		ret = HttpReceiveHttpRequest(WDATA(hQueue), HTTP_NULL_ID, 0, WDATA(hrq), res, &res, &ov);
		if (ret != ERROR_IO_PENDING)
			HandleError(ret, __FILE__LINE__ _T("HttpReceiveHttpRequest"), _T("CHttpServer::OpenRequestQueue"));
		ret = WaitForSingleObject(ov.hEvent, timeout);		
		CloseHandle(ov.hEvent);
		switch (ret)
		{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			return;
		default:
			HandleError2(FALSE, __FILE__LINE__ _T("WaitForSingleObject"), _T("CHttpServer::OpenRequestQueue"));
			return;
		}
	}
	else
	{
		ret = HttpReceiveHttpRequest(WDATA(hQueue), HTTP_NULL_ID, 0, WDATA(hrq), res, &res, nullptr);
		if (ret != ERROR_MORE_DATA)
			HandleError(ret, __FILE__LINE__ _T("HttpReceiveHttpRequest"), _T("CHttpServer::OpenRequestQueue"));
	}
	for (int i = 0; i < HttpHeaderRequestMaximum; ++i)
	{
		if (WDATA(hrq)->Headers.KnownHeaders[i].RawValueLength > 0)
		{
			CStringBuffer key;
			CStringBuffer value;
			CByteBuffer buf;
			const char* h = HttpRequestHeader[i];

			buf.set_Buffer(__FILE__LINE__ CastAny(BConstPointer, h), Castdword(strlen(h)));
			key.convertFromByteBuffer(buf);
			buf.set_Buffer(__FILE__LINE__ CastAny(BConstPointer, WDATA(hrq)->Headers.KnownHeaders[i].pRawValue), WDATA(hrq)->Headers.KnownHeaders[i].RawValueLength);
			value.convertFromUTF8(buf, false);

			RequestDataItem item(key, value);

			header.insert(item);
		}
	}
	for (int i = 0; i < WDATA(hrq)->Headers.UnknownHeaderCount; ++i)
	{
		CStringBuffer key;
		CStringBuffer value;
		CByteBuffer buf;

		buf.set_Buffer(__FILE__LINE__ CastAny(BConstPointer, WDATA(hrq)->Headers.pUnknownHeaders[i].pName), WDATA(hrq)->Headers.pUnknownHeaders[i].NameLength);
		key.convertFromByteBuffer(buf);
		buf.set_Buffer(__FILE__LINE__ CastAny(BConstPointer, WDATA(hrq)->Headers.pUnknownHeaders[i].pRawValue), WDATA(hrq)->Headers.pUnknownHeaders[i].RawValueLength);
		value.convertFromByteBuffer(buf);

		RequestDataItem item(key, value);

		header.insert(item);
	}
	if (WDATA(hrq)->Address.pLocalAddress->sa_family == AF_INET)
	{
		PSOCKADDR_IN p = (PSOCKADDR_IN)(WDATA(hrq)->Address.pLocalAddress);
		CStringBuffer key(__FILE__LINE__ _T("$$LocalAddress"));
		CStringBuffer value;

		value.FormatString(__FILE__LINE__ _T("%02x.%02x.%02x.%02x:%d"), (DWORD)(p->sin_addr.s_net), (DWORD)(p->sin_addr.s_host), (DWORD)(p->sin_addr.s_lh), (DWORD)(p->sin_addr.s_impno), (DWORD)(p->sin_port));

		RequestDataItem item(key, value);

		header.insert(item);

		p = (PSOCKADDR_IN)(WDATA(hrq)->Address.pRemoteAddress);
		key.SetString(__FILE__LINE__ _T("$$RemoteAddress"));
		value.FormatString(__FILE__LINE__ _T("%02x.%02x.%02x.%02x:%d"), (DWORD)(p->sin_addr.s_net), (DWORD)(p->sin_addr.s_host), (DWORD)(p->sin_addr.s_lh), (DWORD)(p->sin_addr.s_impno), (DWORD)(p->sin_port));

		RequestDataItem item1(key, value);

		header.insert(item1);
	}
	else if (WDATA(hrq)->Address.pLocalAddress->sa_family == AF_INET6)
	{
		PSOCKADDR_IN6 p = (PSOCKADDR_IN6)(WDATA(hrq)->Address.pLocalAddress);
		CStringBuffer key(__FILE__LINE__ _T("$$LocalAddress6"));
		CStringBuffer value;

		value.FormatString(__FILE__LINE__ _T("%04x.%04x.%04x.%04x.%04x.%04x.%04x.%04x:%d"), (DWORD)(p->sin6_addr.u.Word[0]), (DWORD)(p->sin6_addr.u.Word[1]), (DWORD)(p->sin6_addr.u.Word[2]), (DWORD)(p->sin6_addr.u.Word[3]), (DWORD)(p->sin6_addr.u.Word[4]), (DWORD)(p->sin6_addr.u.Word[5]), (DWORD)(p->sin6_addr.u.Word[6]), (DWORD)(p->sin6_addr.u.Word[7]), (DWORD)(p->sin6_port));

		RequestDataItem item(key, value);

		header.insert(item);

		p = (PSOCKADDR_IN6)(WDATA(hrq)->Address.pRemoteAddress);
		key.SetString(__FILE__LINE__ _T("$$RemoteAddress6"));
		value.FormatString(__FILE__LINE__ _T("%04x.%04x.%04x.%04x.%04x.%04x.%04x.%04x:%d"), (DWORD)(p->sin6_addr.u.Word[0]), (DWORD)(p->sin6_addr.u.Word[1]), (DWORD)(p->sin6_addr.u.Word[2]), (DWORD)(p->sin6_addr.u.Word[3]), (DWORD)(p->sin6_addr.u.Word[4]), (DWORD)(p->sin6_addr.u.Word[5]), (DWORD)(p->sin6_addr.u.Word[6]), (DWORD)(p->sin6_addr.u.Word[7]), (DWORD)(p->sin6_port));

		RequestDataItem item1(key, value);

		header.insert(item1);
	}
	if (WDATA(hrq)->CookedUrl.FullUrlLength)
	{
		CStringBuffer key(__FILE__LINE__ _T("$$CookedUrl"));
		CStringBuffer value;

		value.FormatString(__FILE__LINE__ _T("%.*s"), Castdword(WDATA(hrq)->CookedUrl.FullUrlLength / sizeof(wchar_t)), WDATA(hrq)->CookedUrl.pFullUrl);

		RequestDataItem item(key, value);

		header.insert(item);
	}
	if (WDATA(hrq)->EntityChunkCount)
	{
		CStringBuffer key(__FILE__LINE__ _T("$$Data"));
		CStringBuffer value;
		CByteLinkedBuffer bufL;
		CByteBuffer bufB;

		for (int i = 0; i < WDATA(hrq)->EntityChunkCount; ++i)
		{
			CByteBuffer buf;

			switch (WDATA(hrq)->pEntityChunks[i].DataChunkType)
			{
			case HttpDataChunkFromMemory:
				buf.set_Buffer(__FILE__LINE__ CastAny(BConstPointer, WDATA(hrq)->pEntityChunks[i].FromMemory.pBuffer), WDATA(hrq)->pEntityChunks[i].FromMemory.BufferLength);
				break;
			case HttpDataChunkFromFileHandle:
				break;
			case HttpDataChunkFromFragmentCache:
				break;
			case HttpDataChunkFromFragmentCacheEx:
				break;
			}
			bufL.AddBufferItem(buf);
		}
		bufL.GetBuffer(bufB);
		value.convertToBase64(bufB);

		RequestDataItem item(key, value);

		header.insert(item);
	}
	if (WDATA(hrq)->RawUrlLength)
	{
		CStringBuffer key(__FILE__LINE__ _T("$$RawUrl"));
		CStringBuffer value;

		value.FormatString(__FILE__LINE__ _T("%.*hs"), Castdword(WDATA(hrq)->RawUrlLength), WDATA(hrq)->pRawUrl);

		RequestDataItem item(key, value);

		header.insert(item);
	}
	if (WDATA(hrq)->RequestInfoCount)
	{
		for (int i = 0; i < WDATA(hrq)->RequestInfoCount; ++i)
		{
			if (WDATA(hrq)->pRequestInfo[i].InfoType == HttpRequestInfoTypeAuth)
			{
				PHTTP_REQUEST_AUTH_INFO p = (PHTTP_REQUEST_AUTH_INFO)(WDATA(hrq)->pRequestInfo[i].pInfo);
				CStringBuffer key;
				CStringBuffer value;

				key.FormatString(__FILE__LINE__ _T("$$AuthInfo%d"), i);
				value.SetString(__FILE__LINE__ _T("AuthType="));

				switch (p->AuthType)
				{
				case HttpRequestAuthTypeNone:
					value.AppendString(_T("None"));
					break;
				case HttpRequestAuthTypeBasic:
					value.AppendString(_T("Basic"));
					break;
				case HttpRequestAuthTypeDigest:
					value.AppendString(_T("Digest"));
					break;
				case HttpRequestAuthTypeNTLM:
					value.AppendString(_T("NTLM"));
					break;
				case HttpRequestAuthTypeNegotiate:
					value.AppendString(_T("Negotiate"));
					break;
				case HttpRequestAuthTypeKerberos:
					value.AppendString(_T("Kerberos"));
					break;
				}
				value.AppendString(_T(", AuthStatus="));
				switch (p->AuthStatus)
				{
				case HttpAuthStatusSuccess:
					value.AppendString(_T("Success"));
					break;
				case HttpAuthStatusNotAuthenticated:
					value.AppendString(_T("NotAuthenticated"));
					break;
				case HttpAuthStatusFailure:
					{
						CStringBuffer v1;

						v1.FormatString(__FILE__LINE__ _T("Failure, SecStatus=%d"), p->SecStatus);
						value.AppendString(v1);
					}
					break;
				}

				RequestDataItem item(key, value);

				header.insert(item);
			}
		}
	}
	{
		CStringBuffer key(__FILE__LINE__ _T("$$Verb"));
		CStringBuffer value;

		switch (WDATA(hrq)->Verb)
		{
		case HttpVerbUnparsed:
			value.AppendString(_T("Unparsed"));
			break;
		case HttpVerbUnknown:
			value.FormatString(__FILE__LINE__ _T("Unknown, Verb=%.*hs"), WDATA(hrq)->UnknownVerbLength, WDATA(hrq)->pUnknownVerb);
			break;
		case HttpVerbInvalid:
			value.AppendString(_T("Invalid"));
			break;
		case HttpVerbOPTIONS:
			value.AppendString(_T("OPTIONS"));
			break;
		case HttpVerbGET:
			value.AppendString(_T("GET"));
			break;
		case HttpVerbHEAD:
			value.AppendString(_T("HEAD"));
			break;
		case HttpVerbPOST:
			value.AppendString(_T("POST"));
			break;
		case HttpVerbPUT:
			value.AppendString(_T("PUT"));
			break;
		case HttpVerbDELETE:
			value.AppendString(_T("DELETE"));
			break;
		case HttpVerbTRACE:
			value.AppendString(_T("TRACE"));
			break;
		case HttpVerbCONNECT:
			value.AppendString(_T("CONNECT"));
			break;
		case HttpVerbTRACK:
			value.AppendString(_T("TRACK"));
			break;
		case HttpVerbMOVE:
			value.AppendString(_T("MOVE"));
			break;
		case HttpVerbCOPY:
			value.AppendString(_T("COPY"));
			break;
		case HttpVerbPROPFIND:
			value.AppendString(_T("PROPFIND"));
			break;
		case HttpVerbPROPPATCH:
			value.AppendString(_T("PROPPATCH"));
			break;
		case HttpVerbMKCOL:
			value.AppendString(_T("MKCOL"));
			break;
		case HttpVerbLOCK:
			value.AppendString(_T("LOCK"));
			break;
		case HttpVerbUNLOCK:
			value.AppendString(_T("UNLOCK"));
			break;
		case HttpVerbSEARCH:
			value.AppendString(_T("SEARCH"));
			break;
		}

		RequestDataItem item(key, value);

		header.insert(item);
	}
	if (WDATA(hrq)->pSslInfo)
	{
	}
	{
		CStringBuffer key(__FILE__LINE__ _T("$$Version"));
		CStringBuffer value;

		value.FormatString(__FILE__LINE__ _T("%d.%d"), (DWORD)(WDATA(hrq)->Version.MajorVersion), (DWORD)(WDATA(hrq)->Version.MinorVersion));

		RequestDataItem item(key, value);

		header.insert(item);
	}
}

void CHttpServer::ReceiveRequestBody(Ref(CByteLinkedBuffer) body)
{
	if ((!_worker) || (!(WDATA(hQueue))) || (!(WDATA(hrq))))
		throw OK_NEW_OPERATOR CHttpServerException(__FILE__LINE__ _T("Program sequence error in %s"), _T("CHttpServer::ReceiveRequestBody"), CWinException::WinExtError, ERROR_REQUEST_OUT_OF_SEQUENCE);
	
	CByteBuffer buf;
	ULONG res;
	ULONG ret;

	do
	{
		res = 4096;
		buf.set_BufferSize(__FILE__LINE__ res);
		ret = HttpReceiveRequestEntityBody(WDATA(hQueue), WDATA(hrq)->RequestId, 0, CastAnyPtr(void, buf.get_Buffer()), res, &res, nullptr);
		if (res)
		{
			buf.set_BufferSize(__FILE__LINE__ res);
			body.AddBufferItem(buf);
		}
		else
			break;
	} while (ret == ERROR_SUCCESS);
	if (ret != ERROR_HANDLE_EOF)
		HandleError(ret, __FILE__LINE__ _T("HttpReceiveRequestEntityBody"), _T("CHttpServer::ReceiveRequestBody"));
}

void CHttpServer::SendResponse(ConstRef(StatusCode) statuscode, ConstRef(ResponseDataList) header, ConstRef(CByteLinkedBuffer) body, bool bLogging)
{
	if ((!_worker) || (!(WDATA(hQueue))) || (!(WDATA(hrq))))
		throw OK_NEW_OPERATOR CHttpServerException(__FILE__LINE__ _T("Program sequence error in %s"), _T("CHttpServer::SendResponse"), CWinException::WinExtError, ERROR_REQUEST_OUT_OF_SEQUENCE);

	PHTTP_RESPONSE hrp;
	ResponseDataList::iterator it(header.begin());
	DWORD sz = 0;
	DWORD res = 0;
	DWORD szUnknownHeader = 0;
	PCHAR p;
	PCHAR p1;
	PCHAR pUnknownHeader;
	
	while (*it)
	{
		ResponseDataItem* v(&((*it)->item));
		CByteBuffer buf;
		int i;

		v->Key.convertToByteBuffer(buf);
		i = GetHttpResponseHeaderNum(buf);
		if (i < 0)
		{
			++szUnknownHeader;
			sz += v->Key.GetLength();
		}
		sz += v->Value.GetLength();
		++it;
	}
	sz += sizeof(HTTP_RESPONSE);
	sz += body.GetTotalLength();
	sz += body.GetBufferItemCount() * sizeof(HTTP_DATA_CHUNK);
	sz += szUnknownHeader * sizeof(HTTP_UNKNOWN_HEADER);
	sz += 512;

	hrp = CastAnyPtr(HTTP_RESPONSE, TFalloc(sz));
	p = CastAny(PCHAR, _l_ptradd(hrp, sizeof(HTTP_RESPONSE)));
	hrp->EntityChunkCount = Castword(body.GetBufferItemCount());
	hrp->pEntityChunks = (hrp->EntityChunkCount > 0)?CastAnyPtr(HTTP_DATA_CHUNK, p):nullptr;
	p1 = CastAny(PCHAR, _l_ptradd(p, hrp->EntityChunkCount * sizeof(HTTP_DATA_CHUNK)));
	for (int i = 0; i < hrp->EntityChunkCount; ++i)
	{
		DerefAnyPtr(HTTP_DATA_CHUNK, p).DataChunkType = HttpDataChunkFromMemory;
		DerefAnyPtr(HTTP_DATA_CHUNK, p).FromMemory.BufferLength = body.GetBufferItemSize(i);
		DerefAnyPtr(HTTP_DATA_CHUNK, p).FromMemory.pBuffer = p1;
		s_memcpy(p1, body.GetBufferItem(i), body.GetBufferItemSize(i));
		p1 = CastAny(PCHAR, _l_ptradd(p1, body.GetBufferItemSize(i)));
		p = CastAny(PCHAR, _l_ptradd(p, sizeof(HTTP_DATA_CHUNK)));
	}
	p = p1;
	if (szUnknownHeader > 0)
	{
		hrp->Headers.pUnknownHeaders = CastAnyPtr(HTTP_UNKNOWN_HEADER, p);
		pUnknownHeader = p;
		p = CastAny(PCHAR, _l_ptradd(p, szUnknownHeader * sizeof(HTTP_UNKNOWN_HEADER)));
	}
	it = header.begin();
	while (*it)
	{
		ResponseDataItem* v(&((*it)->item));
		CByteBuffer bufKey;
		CByteBuffer bufValue;
		int i;

		v->Key.convertToByteBuffer(bufKey);
		v->Value.convertToByteBuffer(bufValue);
		i = GetHttpResponseHeaderNum(bufKey);
		if (i >= 0)
		{
			hrp->Headers.KnownHeaders[i].RawValueLength = Castword(bufValue.get_BufferSize());
			hrp->Headers.KnownHeaders[i].pRawValue = p;
			s_memcpy(p, bufValue.get_Buffer(), bufValue.get_BufferSize());
			p = CastAny(PCHAR, _l_ptradd(p, bufValue.get_BufferSize()));
		}
		else
		{
			DerefAnyPtr(HTTP_UNKNOWN_HEADER, pUnknownHeader).NameLength = Castword(bufKey.get_BufferSize());
			DerefAnyPtr(HTTP_UNKNOWN_HEADER, pUnknownHeader).pName = p;
			s_memcpy(p, bufKey.get_Buffer(), bufKey.get_BufferSize());
			p = CastAny(PCHAR, _l_ptradd(p, bufKey.get_BufferSize()));
			DerefAnyPtr(HTTP_UNKNOWN_HEADER, pUnknownHeader).RawValueLength = Castword(bufValue.get_BufferSize());
			DerefAnyPtr(HTTP_UNKNOWN_HEADER, pUnknownHeader).pRawValue = p;
			s_memcpy(p, bufValue.get_Buffer(), bufValue.get_BufferSize());
			p = CastAny(PCHAR, _l_ptradd(p, bufValue.get_BufferSize()));
			pUnknownHeader = CastAny(PCHAR, _l_ptradd(pUnknownHeader, sizeof(HTTP_UNKNOWN_HEADER)));
		}
		++it;
	}
	{
		CByteBuffer buf;

		hrp->Version.MajorVersion = 1;
		hrp->Version.MinorVersion = 1;
		hrp->StatusCode = Castword(statuscode.Status);
		statuscode.Reason.convertToByteBuffer(buf);
		hrp->ReasonLength = Castword(buf.get_BufferSize());
		hrp->pReason = p;
		s_memcpy(p, buf.get_Buffer(), buf.get_BufferSize());
		p = CastAny(PCHAR, _l_ptradd(p, buf.get_BufferSize()));
	}
	if (bLogging)
	{
		HTTP_LOG_FIELDS_DATA lfd;
		DWORD maxalloc = 1024;
		DWORD cp;
		Pointer pLog = TFalloc(maxalloc);
		CStringBuffer sBuf;
		CByteBuffer bBuf;
		CByteBuffer bComputerName;
		CByteBuffer bUserName;

		ZeroMemory(&lfd, sizeof(HTTP_LOG_FIELDS_DATA));
		p1 = CastAny(PCHAR, pLog);
		lfd.Base.Type = HttpLogDataTypeFields;
		if (WDATA(hrq)->Address.pLocalAddress->sa_family == AF_INET)
		{
			PSOCKADDR_IN p = (PSOCKADDR_IN)(WDATA(hrq)->Address.pLocalAddress);

			sBuf.FormatString(__FILE__LINE__ _T("%02x.%02x.%02x.%02x"), (DWORD)(p->sin_addr.s_net), (DWORD)(p->sin_addr.s_host), (DWORD)(p->sin_addr.s_lh), (DWORD)(p->sin_addr.s_impno));
			sBuf.convertToByteBuffer(bBuf);
			s_memcpy(p1, bBuf.get_Buffer(), bBuf.get_BufferSize());
			lfd.ServerIp = p1;
			lfd.ServerIpLength = Castword(bBuf.get_BufferSize());
			lfd.ServerPort = p->sin_port;
			p1 = CastAny(PCHAR, _l_ptradd(p1, bBuf.get_BufferSize()));
			maxalloc -= bBuf.get_BufferSize();

			p = (PSOCKADDR_IN)(WDATA(hrq)->Address.pRemoteAddress);
			sBuf.FormatString(__FILE__LINE__ _T("%02x.%02x.%02x.%02x"), (DWORD)(p->sin_addr.s_net), (DWORD)(p->sin_addr.s_host), (DWORD)(p->sin_addr.s_lh), (DWORD)(p->sin_addr.s_impno));
			sBuf.convertToByteBuffer(bBuf);
			s_memcpy(p1, bBuf.get_Buffer(), bBuf.get_BufferSize());
			lfd.ClientIp = p1;
			lfd.ClientIpLength = Castword(bBuf.get_BufferSize());
			p1 = CastAny(PCHAR, _l_ptradd(p1, bBuf.get_BufferSize()));
			maxalloc -= bBuf.get_BufferSize();
		}
		else if (WDATA(hrq)->Address.pLocalAddress->sa_family == AF_INET6)
		{
			PSOCKADDR_IN6 p = (PSOCKADDR_IN6)(WDATA(hrq)->Address.pLocalAddress);

			sBuf.FormatString(__FILE__LINE__ _T("%04x.%04x.%04x.%04x.%04x.%04x.%04x.%04x"), (DWORD)(p->sin6_addr.u.Word[0]), (DWORD)(p->sin6_addr.u.Word[1]), (DWORD)(p->sin6_addr.u.Word[2]), (DWORD)(p->sin6_addr.u.Word[3]), (DWORD)(p->sin6_addr.u.Word[4]), (DWORD)(p->sin6_addr.u.Word[5]), (DWORD)(p->sin6_addr.u.Word[6]), (DWORD)(p->sin6_addr.u.Word[7]));
			sBuf.convertToByteBuffer(bBuf);
			s_memcpy(p1, bBuf.get_Buffer(), bBuf.get_BufferSize());
			lfd.ServerIp = p1;
			lfd.ServerIpLength = Castword(bBuf.get_BufferSize());
			lfd.ServerPort = p->sin6_port;
			p1 = CastAny(PCHAR, _l_ptradd(p1, bBuf.get_BufferSize()));
			maxalloc -= bBuf.get_BufferSize();

			p = (PSOCKADDR_IN6)(WDATA(hrq)->Address.pRemoteAddress);
			sBuf.FormatString(__FILE__LINE__ _T("%04x.%04x.%04x.%04x.%04x.%04x.%04x.%04x"), (DWORD)(p->sin6_addr.u.Word[0]), (DWORD)(p->sin6_addr.u.Word[1]), (DWORD)(p->sin6_addr.u.Word[2]), (DWORD)(p->sin6_addr.u.Word[3]), (DWORD)(p->sin6_addr.u.Word[4]), (DWORD)(p->sin6_addr.u.Word[5]), (DWORD)(p->sin6_addr.u.Word[6]), (DWORD)(p->sin6_addr.u.Word[7]));
			sBuf.convertToByteBuffer(bBuf);
			s_memcpy(p1, bBuf.get_Buffer(), bBuf.get_BufferSize());
			lfd.ClientIp = p1;
			lfd.ClientIpLength = Castword(bBuf.get_BufferSize());
			p1 = CastAny(PCHAR, _l_ptradd(p1, bBuf.get_BufferSize()));
			maxalloc -= bBuf.get_BufferSize();
		}
		if (WDATA(hrq)->Headers.KnownHeaders[HttpHeaderUserAgent].RawValueLength > 0)
		{
			lfd.UserAgent = CastAny(PCHAR, CastMutable(PSTR, WDATA(hrq)->Headers.KnownHeaders[HttpHeaderUserAgent].pRawValue));
			lfd.UserAgentLength = WDATA(hrq)->Headers.KnownHeaders[HttpHeaderUserAgent].RawValueLength;
		}
		if (WDATA(hrq)->Headers.KnownHeaders[HttpHeaderHost].RawValueLength > 0)
		{
			lfd.Host = CastAny(PCHAR, CastMutable(PSTR, WDATA(hrq)->Headers.KnownHeaders[HttpHeaderHost].pRawValue));
			lfd.HostLength = WDATA(hrq)->Headers.KnownHeaders[HttpHeaderHost].RawValueLength;
		}
		if (WDATA(hrq)->Headers.KnownHeaders[HttpHeaderReferer].RawValueLength > 0)
		{
			lfd.Referrer = CastAny(PCHAR, CastMutable(PSTR, WDATA(hrq)->Headers.KnownHeaders[HttpHeaderReferer].pRawValue));
			lfd.ReferrerLength = WDATA(hrq)->Headers.KnownHeaders[HttpHeaderReferer].RawValueLength;
		}
		lfd.MethodNum = WDATA(hrq)->Verb;
		sBuf.Clear();
		switch (WDATA(hrq)->Verb)
		{
		case HttpVerbUnparsed:
			sBuf.AppendString(_T("Unparsed"));
			break;
		case HttpVerbUnknown:
			sBuf.FormatString(__FILE__LINE__ _T("Unknown, Verb=%.*hs"), WDATA(hrq)->UnknownVerbLength, WDATA(hrq)->pUnknownVerb);
			break;
		case HttpVerbInvalid:
			sBuf.AppendString(_T("Invalid"));
			break;
		case HttpVerbOPTIONS:
			sBuf.AppendString(_T("OPTIONS"));
			break;
		case HttpVerbGET:
			sBuf.AppendString(_T("GET"));
			break;
		case HttpVerbHEAD:
			sBuf.AppendString(_T("HEAD"));
			break;
		case HttpVerbPOST:
			sBuf.AppendString(_T("POST"));
			break;
		case HttpVerbPUT:
			sBuf.AppendString(_T("PUT"));
			break;
		case HttpVerbDELETE:
			sBuf.AppendString(_T("DELETE"));
			break;
		case HttpVerbTRACE:
			sBuf.AppendString(_T("TRACE"));
			break;
		case HttpVerbCONNECT:
			sBuf.AppendString(_T("CONNECT"));
			break;
		case HttpVerbTRACK:
			sBuf.AppendString(_T("TRACK"));
			break;
		case HttpVerbMOVE:
			sBuf.AppendString(_T("MOVE"));
			break;
		case HttpVerbCOPY:
			sBuf.AppendString(_T("COPY"));
			break;
		case HttpVerbPROPFIND:
			sBuf.AppendString(_T("PROPFIND"));
			break;
		case HttpVerbPROPPATCH:
			sBuf.AppendString(_T("PROPPATCH"));
			break;
		case HttpVerbMKCOL:
			sBuf.AppendString(_T("MKCOL"));
			break;
		case HttpVerbLOCK:
			sBuf.AppendString(_T("LOCK"));
			break;
		case HttpVerbUNLOCK:
			sBuf.AppendString(_T("UNLOCK"));
			break;
		case HttpVerbSEARCH:
			sBuf.AppendString(_T("SEARCH"));
			break;
		}
		if (sBuf.GetLength() > 0)
		{
			sBuf.convertToByteBuffer(bBuf);
			lfd.Method = p1;
			lfd.MethodLength = Castword(bBuf.get_BufferSize());
			s_memcpy(p1, bBuf.get_Buffer(), bBuf.get_BufferSize());
			p1 = CastAny(PCHAR, _l_ptradd(p1, bBuf.get_BufferSize()));
			maxalloc -= bBuf.get_BufferSize();
		}
		if (WDATA(hrq)->RawUrlLength > 0)
		{
			lfd.UriQuery = CastMutablePtr(char, strchr(WDATA(hrq)->pRawUrl, '?'));
			if (NotPtrCheck(lfd.UriQuery))
			{
				++(lfd.UriQuery);
				lfd.UriQueryLength = WDATA(hrq)->RawUrlLength - Castword(lfd.UriQuery - (WDATA(hrq)->pRawUrl));
			}
		}
		if (WDATA(hrq)->CookedUrl.FullUrlLength > 0)
		{
			lfd.UriStem = CastMutable(PWCHAR, WDATA(hrq)->CookedUrl.pFullUrl);
			if (WDATA(hrq)->CookedUrl.QueryStringLength > 0)
				lfd.UriStemLength = Castword(WDATA(hrq)->CookedUrl.FullUrlLength - WDATA(hrq)->CookedUrl.QueryStringLength);
			else
				lfd.UriStemLength = Castword(WDATA(hrq)->CookedUrl.FullUrlLength);
		}
		bUserName.set_BufferSize(__FILE__LINE__ 1024);
		cp = 1024;
		GetUserName(CastAny(LPWSTR, bUserName.get_Buffer()), &cp);
		if (cp > 0)
			--cp;
		cp = cp * szchar;
		bUserName.set_BufferSize(__FILE__LINE__ cp);
		lfd.UserName = CastAny(PWCHAR, bUserName.get_Buffer());
		lfd.UserNameLength = Castword(cp);
		//CEventLogger::WriteFormattedLog(CEventLogger::Information, _T("GetUserName=%.*s, cp=%d"), Cast(int, lfd.UserNameLength) / szchar, lfd.UserName, cp);

		sBuf.SetSize(__FILE__LINE__ 1024);
		cp = 1024;
		GetComputerName(CastAny(LPWSTR, CastMutable(CPointer, sBuf.GetString())), &cp);
		if (cp > 0)
			--cp;
		cp = cp * szchar;
		sBuf.convertToByteBuffer(bComputerName);
		lfd.ServerName = CastAny(PCHAR, bComputerName.get_Buffer());
		lfd.ServerNameLength = Castword(bComputerName.get_BufferSize());

		lfd.ServiceName = "HttpServerService";
		lfd.ServiceNameLength = Castword(strlen(lfd.ServiceName));

		HandleError(HttpSendHttpResponse(WDATA(hQueue), WDATA(hrq)->RequestId, 0, hrp, nullptr, &res, nullptr, 0, nullptr, CastAnyPtr(HTTP_LOG_DATA, &lfd)), __FILE__LINE__ _T("HttpSendHttpResponse"), _T("CHttpServer::SendResponse"));
		TFfree(pLog);
	}
	else
		HandleError(HttpSendHttpResponse(WDATA(hQueue), WDATA(hrq)->RequestId, 0, hrp, nullptr, &res, nullptr, 0, nullptr, nullptr), __FILE__LINE__ _T("HttpSendHttpResponse"), _T("CHttpServer::SendResponse"));

}

ConstRef(CHttpServer::StatusCode) CHttpServer::get_StatusCode(long statuscode) const
{
	return WDATA(statuscodelist)->search(statuscode);
}
