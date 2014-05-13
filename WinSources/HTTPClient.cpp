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
#include "HTTPClient.h"
#include "HttpDate.h"

#define MAX_BUFFER		512

CHttpClient::CHttpClient(void):
	_backupReason(0),
	_defaultURL(false),
	_requestData(250),
    _responseData(250),
    _totalsize(0),
	_open(false),
	_errcnt(0),
	_canlog(false),
	_log(__FILE__LINE__ 16, 16)
{
}

CHttpClient::CHttpClient(ConstRef(CUrl) url):
	_backupReason(0),
	_defaultURL(false),
	_requestData(250),
    _responseData(250),
    _totalsize(0),
	_open(false),
	_errcnt(0),
	_canlog(false),
	_log(__FILE__LINE__ 16, 16)
{
	InitRequest(url);
}

CHttpClient::~CHttpClient(void)
{
}

void CHttpClient::Log(ConstRef(CStringBuffer) text)
{
	if ( _canlog )
		_log.Append(text);
}

void CHttpClient::ResetLog()
{
	_log.Close();
	_log.Open(__FILE__LINE__ 16, 16);
}

void CHttpClient::ClearAll()
{
	_serverName.Clear();
	_resourceString.Clear();
	_resourceBackupString.Clear();
	_defaultURL = false;
	_backupReason = 0;
	_requestData.clear();
	_getBuffer.set_BufferSize(__FILE__LINE__ 0);
	_responseVersion.Clear();
	_responseTypeNum.Clear();
	_responseTypeText.Clear();
	_responseData.clear();
	_responseContent.Clear();
    _totalsize = 0;
}

void CHttpClient::ClearResponse()
{
	_responseVersion.Clear();
	_responseTypeNum.Clear();
	_responseTypeText.Clear();
	_responseData.clear();
	_responseContent.Clear();
    _totalsize = 0;
}

bool CHttpClient::InitRequest(ConstRef(CStringBuffer) serverName, ConstRef(CStringBuffer) resourceString)
{
	if ( serverName.IsEmpty() )
		return false;
	if ( resourceString.IsEmpty() )
		return false;
	_serverName = serverName;
	_resourceString = resourceString;

	_InitRequest();
	return true;
}

bool CHttpClient::InitRequest(ConstRef(CUrl) url)
{
	CStringBuffer serverName = url.get_Server();
	CStringBuffer resourceString = url.get_Resource();

	if ( serverName.IsEmpty() )
		return false;
	if ( resourceString.IsEmpty() )
		return false;
	_serverName = serverName;
	_resourceString = resourceString;

	_InitRequest();
	return true;
}

void CHttpClient::_InitRequest()
{
	_defaultURL = (_resourceString == CStringLiteral(_T("/")));

	CStringBuffer key;
	CStringBuffer value;

	key.SetString(__FILE__LINE__ _T("Host"));
	value.SetString(__FILE__LINE__ _serverName);
	set_RequestData(key, value);

	CHttpDate getDate;

	key.SetString(__FILE__LINE__ _T("Date"));
	value.SetString(__FILE__LINE__ getDate.GetDate());
	set_RequestData(key, value);

	key.SetString(__FILE__LINE__ _T("Accept-Charset"));
	value.SetString(__FILE__LINE__ _T("UTF-8"));
	set_RequestData(key, value);

	key.SetString(__FILE__LINE__ _T("Connection"));
	value.SetString(__FILE__LINE__ _T("close")); // Keep-Alive
	set_RequestData(key, value);

	key.SetString(__FILE__LINE__ _T("User-Agent"));
	value.SetString(__FILE__LINE__ _T("Mozilla/5.0 (Windows NT 6.3; WOW64; rv:28.0) Gecko/20100101 Firefox/28.0"));
	set_RequestData(key, value);
}

void CHttpClient::MakeCommand()
{
	CStringBuffer getString;
	ResponseDataList::iterator getIt(_requestData.begin());

	switch ( _command )
	{
	case GET_COMMAND:
		getString.FormatString(__FILE__LINE__ _T("GET %s HTTP/1.1"), _resourceString.GetString());
		break;
	case POST_COMMAND:
		getString.FormatString(__FILE__LINE__ _T("POST %s HTTP/1.1"), _resourceString.GetString());
		break;
	case HEAD_COMMAND:
		getString.FormatString(__FILE__LINE__ _T("HEAD %s HTTP/1.1"), _resourceString.GetString());
		break;
	}
	getString.AppendString(_T("\r\n"));
	while ( *getIt )
	{
		getString.AppendString((*getIt)->item.Key);
		getString.AppendString(_T(": "));
		getString.AppendString((*getIt)->item.Value);
		getString.AppendString(_T("\r\n"));
		++getIt;
	}
	getString.AppendString(_T("\r\n"));
	Log(getString);
	getString.convertToByteBuffer(_getBuffer);
}

bool CHttpClient::Load(LoadOptions options, LoadCommand command)
{
	if ( _serverName.IsEmpty() || _resourceString.IsEmpty() )
		return false;

	dword numDataSend;
	dword numDataReceived;
	int state;
	int errstate;

	if ( (OPEN_PERSISTENT == options) || (STAY_PERSISTENT == options) )
	{
		CStringBuffer name(__FILE__LINE__ _T("Connection"));

		remove_RequestData(name);
		set_RequestData(name, CStringBuffer(__FILE__LINE__ _T("Keep-Alive")));
	}

	_command = command;
	MakeCommand();

	CByteLinkedBuffer::Iterator pos(_responseContent.Begin());

	for ( int cnt = 10 ; cnt > 0; --cnt )
	{
		try
		{
			errstate = 0;
			if ( (OPEN_TRANSIENT == options) || (OPEN_PERSISTENT == options) || (!_open) )
			{
				_tcpClient.OpenConnection(_serverName.GetString(), _T("http"));
				_open = true;
			}
			errstate = 1;
			_tcpClient.SendData(_getBuffer.get_Buffer(), _getBuffer.get_BufferSize(), &numDataSend);

			state = 0;
			_totalsize = 0;
			errstate = 2;
			for ( ;; )
			{
				_tcpClient.ReceiveData(_responseContent.AddBufferItem(MAX_BUFFER), MAX_BUFFER, &numDataReceived, 3000);
				if ( numDataReceived == 0 )
				{
					_responseContent.RemoveBufferItem(_responseContent.GetBufferItemCount() - 1);
					if ( _totalsize == 0 )
						_responseContent.SetBegin(pos);
					break;
				}
				if ( numDataReceived != MAX_BUFFER )
					_responseContent.SetBufferItemSize(_responseContent.GetBufferItemCount() - 1, numDataReceived);
				if ( Parse(pos, &state) )
					break;
			} 
			if ( (OPEN_TRANSIENT == options) || (CLOSE_PERSISTENT == options) )
			{
				_tcpClient.CloseConnection();
				_open = false;
			}
			else
			{
				CStringBuffer sBuf = get_ResponseData(CStringBuffer(__FILE__LINE__ _T("Connection")));

				if ( 0 == (sBuf.Compare(CStringLiteral(_T("Close")), 0, CStringLiteral::cIgnoreCase)) )
				{
					_tcpClient.CloseConnection();
					_open = false;
				}
			}
			return true;
		}
		catch ( CTcpClientException* ex )
		{
			_tcpClient.CloseConnection();
			_open = false;

			++_errcnt;
			_log.Append(ex->GetExceptionMessage());

			switch ( errstate )
			{
			case 0:
			case 1:
				break;
			case 2:
				ClearResponse();
				pos = _responseContent.Begin();
				break;
			}
		}
	}
	return false;
}

bool CHttpClient::Parse(Ref(CByteLinkedBuffer::Iterator) pos, int *state)
{
	CByteBuffer buf;
	CStringBuffer sBuf;

	for ( ;; )
	{
		switch ( *state )
		{
		case 0:
			{
				CPointer sBufSplit[3];
				dword sBufSplitN;
				CByteLinkedBuffer::Iterator find(pos);

				find.Find((BPointer)("\r\n"), 2);
				if ( !find )
					return false;
				buf.set_BufferSize(__FILE__LINE__ find - pos);
				_responseContent.GetSubBuffer(pos, buf);
				sBuf.convertFromByteBuffer(buf);
				Log(sBuf);
				sBuf.Split(_T(" "), sBufSplit, 3, &sBufSplitN);
				if ( sBufSplitN == 3 )
				{
					_responseVersion.SetString(__FILE__LINE__ sBufSplit[0]);
					_responseTypeNum.SetString(__FILE__LINE__ sBufSplit[1]);
					_responseTypeText.SetString(__FILE__LINE__ sBufSplit[2]);
				}
				else
					return true;
				pos = find;
				++(*state);
			}
			break;
		case 1:
			{
				CByteLinkedBuffer::Iterator find(pos);
				CStringConstIterator it;
				CStringBuffer name;
				CStringBuffer key;
				int delta;

				find += 2;
				find.Find((BPointer)("\r\n"), 2);
				if ( !find )
					return false;
				pos += 2;
				delta = find - pos;
				if ( delta == 0  )
				{
					pos += 2;
					++(*state);
					break;
				}
				buf.set_BufferSize(__FILE__LINE__ delta);
				_responseContent.GetSubBuffer(pos, buf);
				sBuf.convertFromByteBuffer(buf);
				Log(sBuf);
				it = sBuf.GetString();
				it.Find(_T(":"));
				if ( it.IsEnd() )
					break;
				sBuf.SubString(0, Castdword(it.GetCurrent() - it.GetOrigin()), name);
				sBuf.SubString(Castdword(it.GetCurrent() - it.GetOrigin() + 1), it.GetLength() - 1, key);
				name.Trim();
				key.Trim();

				ResponseDataItem value(name, key);

				_responseData.insert(value);
				pos = find;
			}
			break;
		case 2:
			{
				dword dvalue = 0;

				if ( _responseTypeNum.IsEmpty() )
					return true;
				if ( _responseTypeNum.ScanString(_T("%u"), &dvalue) <= 0 )
					return true;
				switch ( dvalue )
				{
				case 100:
				case 101:
				case 204:
				case 304:
					_responseContent.SetBegin(pos);
					return true;
				case 200:
				default:
					if ( _command == HEAD_COMMAND )
					{
						_responseContent.SetBegin(pos);
						return true;
					}
					break;
				}
				++(*state);
			}
            break;
		case 3:
			{
				dword dvalue = 0;

				sBuf = _responseData.search(CStringBuffer(__FILE__LINE__ _T("Content-Length"))).Value;
				if ( sBuf.IsEmpty() )
                {
                    ++(*state);
					break;
                }
				if ( sBuf.ScanString(_T("%u"), &dvalue) <= 0 )
					return false;
				if ( dvalue > (_responseContent.GetTotalLength() - pos.GetTotalLength()) )
					return false;
                _responseContent.SetBegin(pos);
				return true;
			}
			break;
        case 4:
            {
				  sBuf = _responseData.search(CStringBuffer(__FILE__LINE__ _T("Transfer-Encoding"))).Value;
                _responseContent.SetBegin(pos);
                pos = _responseContent.Begin();
				if ( sBuf.IsEmpty() || (sBuf != CStringLiteral(_T("chunked"))) )
                    return true;
                ++(*state);
                return false;
            }
            break;
        case 5:
            {
                CByteLinkedBuffer::Iterator find(pos);
                CByteLinkedBuffer::Iterator find1(pos);
				CByteBuffer rBuf;
				dword dvalue;
                
				find.Find((BPointer)("\r\n"), 2);
				if ( !find )
					return false;
				find1.Find((BPointer)(";"), 1);
                if ( (!find1) || (find1 > find) )
                    find1 = find;
				buf.set_BufferSize(__FILE__LINE__ find1 - pos);
				_responseContent.GetSubBuffer(pos, buf);
				_responseContent.ReplaceSubBuffer(pos, find - pos + 2, rBuf);
				sBuf.convertFromByteBuffer(buf);
                sBuf.ScanString(_T("%x"), &dvalue);
				if ( dvalue == 0 )
					return true;
				_totalsize += dvalue + 2;
				++(*state);
				++(*state);
                return false;
            }
            break;
        case 6:
            {
                CByteLinkedBuffer::Iterator find(pos);
                CByteLinkedBuffer::Iterator find1(pos);
				CByteBuffer rBuf;
				dword dvalue;
                
				find.Find((BPointer)("\r\n"), 2);
				if ( !find )
					return false;
				find1.Find((BPointer)(";"), 1);
                if ( (!find1) || (find1 > find) )
                    find1 = find;
				buf.set_BufferSize(__FILE__LINE__ find1 - pos);
				_responseContent.GetSubBuffer(pos, buf);
                pos -= 2;
				_responseContent.ReplaceSubBuffer(pos, find - pos + 2, rBuf);
				sBuf.convertFromByteBuffer(buf);
                sBuf.ScanString(_T("%x"), &dvalue);
				if ( dvalue == 0 )
					return true;
				_totalsize += dvalue;
				++(*state);
                return false;
            }
            break;
		case 7:
			{
				if ( _responseContent.GetTotalLength() < _totalsize )
					return false;
				pos = _responseContent.Begin();
				pos += _totalsize;
				--(*state);
			}
			break;
        default:
            break;
		}
	}
	return true;
}
