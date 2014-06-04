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
#include "AsyncIOManager.h"

class WINSOURCES_API CAsyncIOBuffer: public CCppObject
{
public:
	CAsyncIOBuffer(Ptr(CAsyncIOData) pData); // for searching
	CAsyncIOBuffer(Ptr(CAsyncIOManager) pManager);
	virtual ~CAsyncIOBuffer(void);

	__inline Ptr(CAsyncIOData) GetData() const { return m_pData; }

	virtual void Read(Ref(CByteBuffer) buf, Ptr(CAbstractThreadCallback) pHandler);
	virtual void Write(ConstRef(CByteBuffer) buf, Ptr(CAbstractThreadCallback) pHandler);

protected:
	Ptr(CAsyncIOManager) m_pManager;
	CCppObjectPtr<CAsyncIOData> m_pData;
	bool m_bDetach;
private:
	CAsyncIOBuffer();
};

