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

#include "DataVector.h"
#include "WinSources.h"

#ifdef OK_SYS_UNIX
#include <unistd.h>
#define Sleep(t) sleep(t / 1000)
#endif

class CDebugServerPrivate;
class WINSOURCES_API CDebugServer
{
public:
	CDebugServer(void);
	~CDebugServer(void);

	void RunServer(void);

private:
	Ptr(CDebugServerPrivate) _d;
};

class CThread;
class CTcpClient;
class WINSOURCES_API CDebugClient: public CCppObject
{
public:
	CDebugClient(void);
	~CDebugClient(void);

	static void CreateInstance();
	static void FreeInstance();
	static Ptr(CDebugClient) Instance();

	void SendDebugMessage(ConstRef(CStringBuffer) msg);

	dword ThreadProc(void);
	void Start(void);
	void Stop(void);

private:
	Ptr(CThread) _thread;
	Ptr(CTcpClient) _tcpClient;
	CDataVectorT<CStringBuffer> _msgs;

	static Ptr(CDebugClient) _instance;
};

class WINSOURCES_API CDebug
{
public:
	CDebug(void);
	~CDebug(void);

	Ref(CDebug) Append(byte);
	Ref(CDebug) Append(sbyte);
#ifdef _UNICODE
	Ref(CDebug) Append(mbchar);
#endif
	Ref(CDebug) Append(word);
	Ref(CDebug) Append(sword);
	Ref(CDebug) Append(dword);
	Ref(CDebug) Append(sdword);
	Ref(CDebug) Append(qword);
	Ref(CDebug) Append(sqword);
	Ref(CDebug) Append(CConstPointer);
	Ref(CDebug) Append(ConstRef(CStringLiteral));
	Ref(CDebug) Append(ConstRef(CStringBuffer));

	void SendDebugMessage();

	__inline Ref(CDebug) operator << (byte val)
	{
		return Append(val);
	}

	__inline Ref(CDebug) operator << (sbyte val)
	{
		return Append(val);
	}

#ifdef _UNICODE
	__inline Ref(CDebug) operator << (mbchar val)
	{
		return Append(val);
	}
#endif

	__inline Ref(CDebug) operator << (word val)
	{
		return Append(val);
	}

	__inline Ref(CDebug) operator << (sword val)
	{
		return Append(val);
	}

	__inline Ref(CDebug) operator << (dword val)
	{
		return Append(val);
	}

	__inline Ref(CDebug) operator << (sdword val)
	{
		return Append(val);
	}

	__inline Ref(CDebug) operator << (qword val)
	{
		return Append(val);
	}

	__inline Ref(CDebug) operator << (sqword val)
	{
		return Append(val);
	}

	__inline Ref(CDebug) operator << (CConstPointer val)
	{
		return Append(val);
	}

	__inline Ref(CDebug) operator << (ConstRef(CStringLiteral) val)
	{
		return Append(val);
	}

	__inline Ref(CDebug) operator << (ConstRef(CStringBuffer) val)
	{
		return Append(val);
	}

	__inline Ref(CDebug) operator << (Ref(CDebug) (__cdecl *_Pfn)(Ref(CDebug)))
	{
		return ((*_Pfn)(*this));
	}

private:
	CStringBuffer _buffer;
};

__inline Ref(CDebug) eodbg(Ref(CDebug) out)
{
	out.SendDebugMessage();
	return out;
}

#define __DBGPRINT__
