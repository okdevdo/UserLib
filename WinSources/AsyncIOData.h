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
#include "Thread.h"

class WINSOURCES_API CAsyncIOData: public CCppObject
{
public:
	enum IOOperation {
		IORead,
		IOWrite
	};

	CAsyncIOData(void);
	virtual ~CAsyncIOData(void);

	__inline LPOVERLAPPED get_overlapped() { return &m_overlapped; }
	__inline HANDLE get_file() const { return m_file; }
	__inline void set_file(HANDLE h) { m_file = h; }
	__inline IOOperation get_ioop() const { return m_ioop; }
	__inline void set_ioop(IOOperation i) { m_ioop = i; }
	__inline DWORD get_bytestransferred() const { return m_bytestransferred; }
	__inline void set_bytestransferred(DWORD v) { m_bytestransferred = v; }
	__inline ConstRef(CByteBuffer) get_buffer() const { return m_buffer; }
	__inline void set_buffer(ConstRef(CByteBuffer) b) { m_buffer = b; }
	__inline DWORD get_errorcode() const { return m_errorcode; }
	__inline void set_errorcode(DWORD v) { m_errorcode = v; }
	__inline Ptr(CAbstractThreadCallback) get_callback() const { return m_callback; }
	__inline void set_callback(Ptr(CAbstractThreadCallback) p) { m_callback = p; }

protected:
	OVERLAPPED m_overlapped;
	HANDLE m_file;
	IOOperation m_ioop;
	DWORD m_bytestransferred;
	CByteBuffer m_buffer;
	DWORD m_errorcode;
	Ptr(CAbstractThreadCallback) m_callback;
};

template <class C>
class CAsyncIODataCallback: public CAbstractThreadCallback
{
public:
	typedef dword (C::*Callback)(Ptr(CAsyncIOData) pData);

	CAsyncIODataCallback(C* pObject, Callback method, Ptr(CAsyncIOData) pData):
		CAbstractThreadCallback(),
		_pObject(pObject),
		_method(method),
		_data(pData)
	{
	}
	
	CAsyncIODataCallback(const CAsyncIODataCallback& cb):
		CAbstractThreadCallback(cb),
		_pObject(cb._pObject),
		_method(cb._method),
		_data(cb._data)
	{
	}
	
	virtual ~CAsyncIODataCallback()
	{
	}
	
	CAsyncIODataCallback& operator = (const CAsyncIODataCallback& cb)
	{
		if (&cb != this)
		{
			this->_pObject = cb._pObject;
			this->_method  = cb._method;
			this->_data    = cb._data;
		}
		return *this;
	}
	
	virtual dword invoke()
	{
		return (_pObject->*_method)(_data);
	}
	
private:
	CAsyncIODataCallback();
	
	C* _pObject;
	Callback _method;
	Ptr(CAsyncIOData) _data;
};


