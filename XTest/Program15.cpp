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
#include "AsyncIOManager.h"
#include "AsyncFile.h"
#include "AsyncTCPClient.h"
#include "AsyncTCPServer.h"
#include "ScopedLock.h"

class CTestAsyncFile
{
public:
	CTestAsyncFile():
        io_manager(),
		buffer(__FILE__LINE__ 4096),
		input(&io_manager),
		output(&io_manager)
	{
	}

	~CTestAsyncFile()
	{
	}

	dword read_callback(Ptr(CAsyncIOData) pData)
	{
		buffer.set_BufferSize(__FILE__LINE__ pData->get_bytestransferred());
		if ( buffer.get_BufferSize() == 0 )
		{
			output.Close();
			input.Close();
			io_manager.Stop();
			return 1;
		}
		output.Write(buffer, OK_NEW_OPERATOR CAsyncIODataCallback<CTestAsyncFile>(this, &CTestAsyncFile::write_callback, output.GetData()));
		return 0;
	}

	dword write_callback(Ptr(CAsyncIOData) pData)
	{
		buffer.set_BufferSize(__FILE__LINE__ 4096);
		input.Read(buffer, OK_NEW_OPERATOR CAsyncIODataCallback<CTestAsyncFile>(this, &CTestAsyncFile::read_callback, input.GetData()));
		return 0;
	}

	void Run()
	{
		CFilePath finput(__FILE__LINE__ _T("UserLibTest.zip"));
		CFilePath foutput(__FILE__LINE__ _T("UserLibTest1.zip"));

		io_manager.Create();
		input.Open(finput, true);
		output.Create(foutput);
		input.Read(buffer, OK_NEW_OPERATOR CAsyncIODataCallback<CTestAsyncFile>(this, &CTestAsyncFile::read_callback, input.GetData()));
		io_manager.WaitForComplete();
		io_manager.Close();
	}

protected:
	CAsyncIOManager io_manager;
	CByteBuffer buffer;
	CAsyncFile input;
	CAsyncFile output;
};

void TestAsyncFile()
{
	CTestAsyncFile test;

	test.Run();
}

class AsyncTCPClientListEqualFunctor
{
public:
	bool operator()(ConstPtr(CAsyncTCPClient) r1, ConstPtr(CAsyncTCPClient) r2) const
	{
		return r1->GetData() == r2->GetData();
	}
};

typedef CDataDoubleLinkedListT<CAsyncTCPClient> CAsyncTCPClientList;

class CTestAsyncTCPClient
{
public:
	CTestAsyncTCPClient():
        io_manager(),
		clientlist(__FILE__LINE__0),
		count(0)
	{
	}

	~CTestAsyncTCPClient()
	{
	}

	dword read_callback(Ptr(CAsyncIOData) pData)
	{
		CScopedLock lock;
		CAsyncTCPClient client(pData);
		CAsyncTCPClientList::Iterator it = clientlist.Find<AsyncTCPClientListEqualFunctor>(&client);

		if ( it )
		{
			Ptr(CAsyncTCPClient) pClient = *it;

			//if ((pData->get_bytestransferred() == 0) || (pData->get_errorcode() != 0) || (count++ >= 5))
			if ((pData->get_bytestransferred() == 0) || (pData->get_errorcode() != 0))
			{
				pClient->Close();
				clientlist.Remove(it);
				if ( clientlist.Count() == 0 )
					io_manager.Stop();
				return 1;
			}
			//printf("%s\n", pData->get_buffer().get_Buffer());
			printf(".");

			CByteBuffer buffer(__FILE__LINE__ 8192);

			FillBuffer(buffer);
			pClient->Write(buffer, OK_NEW_OPERATOR CAsyncIODataCallback<CTestAsyncTCPClient>(this, &CTestAsyncTCPClient::write_callback, pClient->GetData()));
		}
		return 0;
	}

	dword write_callback(Ptr(CAsyncIOData) pData)
	{
		CScopedLock lock;
		CAsyncTCPClient client(pData);
		CAsyncTCPClientList::Iterator it = clientlist.Find<AsyncTCPClientListEqualFunctor>(&client);

		if ( it )
		{
			Ptr(CAsyncTCPClient) pClient = *it;

			if (pData->get_errorcode() != 0)
			{
				pClient->Close();
				clientlist.Remove(it);
				if ( clientlist.Count() == 0 )
					io_manager.Stop();
				return 1;
			}

			CByteBuffer buffer(__FILE__LINE__ 8192);

			pClient->Read(buffer, OK_NEW_OPERATOR CAsyncIODataCallback<CTestAsyncTCPClient>(this, &CTestAsyncTCPClient::read_callback, pClient->GetData()));
		}
		return 0;
	}

	void Run()
	{
		io_manager.Create();
		for ( dword i = 0; i < 32; ++i )
		{
			Ptr(CAsyncTCPClient) pClient = OK_NEW_OPERATOR CAsyncTCPClient(&io_manager);
			CByteBuffer buffer(__FILE__LINE__ 8192);

			FillBuffer(buffer);
			pClient->Open(_T("localhost"), _T("6001"));
			clientlist.Append(pClient);
			pClient->Write(buffer, OK_NEW_OPERATOR CAsyncIODataCallback<CTestAsyncTCPClient>(this, &CTestAsyncTCPClient::write_callback, pClient->GetData()));
		}
		io_manager.WaitForComplete();
		io_manager.Close();
	}

	void FillBuffer(Ref(CByteBuffer) buffer)
	{
		BPointer bp = buffer.get_Buffer();
		dword bcnt = buffer.get_BufferSize();

		srand((unsigned)time(nullptr));

		do
		{
			unsigned int number;
			errno_t err = rand_s(&number);
			char buf[20];
			dword len;

			_ASSERTE(err == 0);

			itoa(number, buf, 16);
			len = Castdword(strlen(buf));
			if (len <= bcnt)
			{
				s_memcpy_s(bp, bcnt, buf, len);
				bp += len;
				bcnt -= len;
			}
			else
			{
				s_memcpy_s(bp, bcnt, buf, bcnt);
				bcnt = 0;
			}
		} while (bcnt > 0);
	}

protected:
	CAsyncIOManager io_manager;
	CAsyncTCPClientList clientlist;
	dword count;
};

void TestAsyncTCPClient()
{
	CTestAsyncTCPClient test;

	test.Run();
}

class CTestAsyncTCPServer
{
public:
	CTestAsyncTCPServer():
        io_manager(),
		server(&io_manager),
		clientlist(__FILE__LINE__0)
	{
	}

	~CTestAsyncTCPServer()
	{
	}

	dword accept_callback(Ptr(CAsyncIOData) pData)
	{
		CScopedLock lock;
		CByteBuffer buffer(__FILE__LINE__ 8192);
		CByteBuffer buffer1(__FILE__LINE__ 8192);

		//printf("%s\n", pData->get_buffer().get_Buffer());
		
		Ptr(CAsyncTCPClient) pClient = OK_NEW_OPERATOR CAsyncTCPClient(&io_manager);

		server.CreateClientConnection(pClient);
		clientlist.Append(pClient);

		read_callback(pClient->GetData());
		try
		{
			server.Accept(buffer1, OK_NEW_OPERATOR CAsyncIODataCallback<CTestAsyncTCPServer>(this, &CTestAsyncTCPServer::accept_callback, server.GetData()));
		}
		catch ( CBaseException* )
		{
			io_manager.Stop();
			return 1;
		}
		return 0;
	}

	dword read_callback(Ptr(CAsyncIOData) pData)
	{
		CScopedLock lock;
		CAsyncTCPClient client(pData);
		CAsyncTCPClientList::Iterator it = clientlist.Find<AsyncTCPClientListEqualFunctor>(&client);

		if ( it )
		{
			Ptr(CAsyncTCPClient) pClient = *it;

			if ( (pData->get_bytestransferred() == 0) || (pData->get_errorcode() != 0) )
			{
				pClient->Close();
				clientlist.Remove(it);
				if ( clientlist.Count() == 0 )
					io_manager.Stop();
				return 1;
			}
			//printf("%s\n", pData->get_buffer().get_Buffer());

			CByteBuffer buffer(__FILE__LINE__ 8192);

			FillBuffer(buffer);
			try
			{
				pClient->Write(buffer, OK_NEW_OPERATOR CAsyncIODataCallback<CTestAsyncTCPServer>(this, &CTestAsyncTCPServer::write_callback, pClient->GetData()));
			}
			catch ( CBaseException* )
			{
				pClient->Close();
				clientlist.Remove(it);
				if ( clientlist.Count() == 0 )
					io_manager.Stop();
				return 1;
			}
		}
		return 0;
	}

	dword write_callback(Ptr(CAsyncIOData) pData)
	{
		CScopedLock lock;
		CAsyncTCPClient client(pData);
		CAsyncTCPClientList::Iterator it = clientlist.Find<AsyncTCPClientListEqualFunctor>(&client);

		if ( it )
		{
			Ptr(CAsyncTCPClient) pClient = *it;

			if ( pData->get_errorcode() != 0 )
			{
				pClient->Close();
				clientlist.Remove(it);
				if ( clientlist.Count() == 0 )
					io_manager.Stop();
				return 1;
			}

			CByteBuffer buffer(__FILE__LINE__ 8192);

			try
			{
				pClient->Read(buffer, OK_NEW_OPERATOR CAsyncIODataCallback<CTestAsyncTCPServer>(this, &CTestAsyncTCPServer::read_callback, pClient->GetData()));
			}
			catch ( CBaseException* )
			{
				pClient->Close();
				clientlist.Remove(it);
				if ( clientlist.Count() == 0 )
					io_manager.Stop();
				return 1;
			}
		}
		return 0;
	}

	void Run()
	{
		CByteBuffer buffer(__FILE__LINE__ 8192);

		FillBuffer(buffer);

		try
		{
			io_manager.Create();
		}
		catch ( CBaseException* )
		{
			return;
		}
		try
		{
			server.Open(_T("localhost"), _T("6001"));
			server.Accept(buffer, OK_NEW_OPERATOR CAsyncIODataCallback<CTestAsyncTCPServer>(this, &CTestAsyncTCPServer::accept_callback, server.GetData()));
		}
		catch ( CBaseException* )
		{
			io_manager.Stop();
		}
		io_manager.WaitForComplete();
		io_manager.Close();
	}

	void FillBuffer(Ref(CByteBuffer) buffer)
	{
		BPointer bp = buffer.get_Buffer();
		dword bcnt = buffer.get_BufferSize();

		srand((unsigned)time(nullptr));

		do
		{
			unsigned int number;
			errno_t err = rand_s(&number);
			char buf[20];
			dword len;

			_ASSERTE(err == 0);

			itoa(number, buf, 16);
			len = Castdword(strlen(buf));
			if (len <= bcnt)
			{
				s_memcpy_s(bp, bcnt, buf, len);
				bp += len;
				bcnt -= len;
			}
			else
			{
				s_memcpy_s(bp, bcnt, buf, bcnt);
				bcnt = 0;
			}
		} while (bcnt > 0);
	}

protected:
	CAsyncIOManager io_manager;
	CAsyncTCPServer server;
	CAsyncTCPClientList clientlist;
};

void TestAsyncTCPServer()
{
	CTestAsyncTCPServer test;

	test.Run();
}

