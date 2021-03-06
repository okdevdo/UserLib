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

#include "FilePath.h"
#include "WinSources.h"
#include "AsyncIOBuffer.h"

class WINSOURCES_API CAsyncFile: public CAsyncIOBuffer
{
public:
	CAsyncFile(Ptr(CAsyncIOManager) pManager = NULL);
	virtual ~CAsyncFile(void);

	void Create(ConstRef(CFilePath) _path);
	void Open(ConstRef(CFilePath) _path, bool _readOnly);
	void Close();

	virtual void Read(Ref(CByteBuffer) buf, Ptr(CAbstractThreadCallback) pHandler);
	virtual void Write(ConstRef(CByteBuffer) buf, Ptr(CAbstractThreadCallback) pHandler);
};

