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
#include "AsyncIOBuffer.h"

CAsyncIOBuffer::CAsyncIOBuffer(Ptr(CAsyncIOManager) pManager):
    m_pManager(pManager),
	m_pData(OK_NEW_OPERATOR CAsyncIOData())
{
}

CAsyncIOBuffer::~CAsyncIOBuffer(void)
{
	m_pData->release();
}

void CAsyncIOBuffer::Read(Ref(CByteBuffer) buf, Ptr(CAbstractThreadCallback) pHandler)
{
}

void CAsyncIOBuffer::Write(ConstRef(CByteBuffer) buf, Ptr(CAbstractThreadCallback) pHandler)
{
}
