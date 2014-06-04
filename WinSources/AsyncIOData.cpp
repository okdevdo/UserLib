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
#include "AsyncIOData.h"

CAsyncIOData::CAsyncIOData(void):
	m_file(INVALID_HANDLE_VALUE),
	m_ioop(IORead),
	m_bytestransferred(0),
	m_buffer(),
	m_errorcode(0),
	m_callback(nullptr)
{
	s_memset(&m_overlapped, 0, sizeof(OVERLAPPED));
}

CAsyncIOData::~CAsyncIOData(void)
{
}
