/******************************************************************************
    
	This file is part of XTest, which is part of UserLib.

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
#include "Program.h"
#ifdef OK_SYS_WINDOWS
#include "HTTPClient.h"
#include "HTTPCache.h"
#include "SyncTCPSocketClasses.h"
#include "File.h"

void TestHTTP1Internal(CStringLiteral url)
{
	CUrl url1(url.GetString());
	CHttpClient client(url1);

	if ( !(client.Load()) )
		return;
	CStringBuffer urlpath = url1.get_Url();
	if ( !(urlpath.IsEmpty()) )
		COUT << _T("URL = ") << urlpath << endl;
	CStringBuffer version = client.get_ResponseVersion();
	if ( !(version.IsEmpty()) )
		COUT << _T("version = ") << version << endl;
	CStringBuffer typeNum = client.get_ResponseTypeNum();
	if ( !(typeNum.IsEmpty()) )
		COUT << _T("typeNum = ") << typeNum << endl;
	CStringBuffer typeText = client.get_ResponseTypeText();
	if ( !(typeText.IsEmpty()) )
		COUT << _T("typeText = ") << typeText << endl;

	CHttpClient::TResponseDataItems::Iterator it = client.get_ResponseDataBegin();

	while ( *it )
	{
		CStringBuffer key = (*it)->Key;
		CStringBuffer value = (*it)->Value;

		if ( (!(key.IsEmpty())) && (!(value.IsEmpty())) )
			COUT << key << _T(" = ") << value << endl;

		++it;
	}

	ConstRef(CByteLinkedBuffer) buffer = client.get_ResponseContent();
	dword buflen = buffer.GetTotalLength();

	COUT << _T("Buflen = ") << buflen << endl;

	CByteBuffer buf;
	CStringBuffer sbuf;

	buf.set_BufferSize(__FILE__LINE__ 300);
	buffer.GetSubBuffer(buffer.Begin(), buf);
	sbuf.convertFromByteBuffer(buf);

	COUT << sbuf << endl;

	CDiskFile file;
	CFilePath path(__FILE__LINE__ _T("HttpClientTest.html"));

	try
	{
		file.Create(path);
		file.Write(buffer);
		file.Close();
	}
	catch ( CFileException* ex )
	{
		file.Close();
		COUT << (CConstPointer)(ex->GetExceptionMessage()) << endl;
	}
}

//-thttp1=http://linux.rz.ruhr-uni-bochum.de/download/cygwin/;ftp://ftp.cygwinports.org/pub/cygwinports/
void TestHTTP1(ConstRef(TMBCharList) urlList)
{
	try
	{
		TMBCharList::Iterator it = urlList.Begin();

		while ( it )
		{
			TestHTTP1Internal(*it);

		//	CUrl url(*it);
		//	CStringBuffer tmp(url.get_Encoded(CUrl::EncodeTypePart));

		//	COUT << url.get_Url() << endl;
		//	COUT << tmp << endl;
		//	url.set_Encoded(tmp);
		//	COUT << url.get_Url() << endl;
			++it;
		}
	}
	catch ( CTcpClientException* ex )
	{
		COUT << ex->GetExceptionMessage() << endl;
	}
}

void TestHTTP2Internal(CStringLiteral importfile)
{
	CTcpClient client;
	CFilePath path(__FILE__LINE__ _T("c:\\WebCache\\"));
	CHttpCache cache(path);
	CFilePath path2(__FILE__LINE__ importfile);

	cache.Import(path2);
}

//--TestHttp2 C:\Users\Oliver\Documents\HttpList.txt
void TestHTTP2(CStringLiteral importfile)
{
	try
	{
		TestHTTP2Internal(importfile);
	}
	catch ( CTcpClientException* ex )
	{
		COUT << ex->GetExceptionMessage() << endl;
	}
}
#endif