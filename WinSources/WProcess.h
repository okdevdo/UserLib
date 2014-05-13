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

#include "File.h"
#include "DataVector.h"
#include "WinSources.h"
#include "WinException.h"

class WINSOURCES_API CProcess: public CFile
{
public:
	CProcess(void);
	virtual ~CProcess(void);

	virtual void Create(ConstRef(CFilePath) _path);
	void Create(ConstRef(CFilePath) _cmd, ConstRef(CStringBuffer) _args, ConstRef(CDataVectorT<CStringBuffer>) _env, ConstRef(CFilePath) _startDir = CFilePath());
	virtual void Close();

	virtual TFileSize GetSize();
	virtual void SetSize(TFileSize);
	virtual TFileOffset GetFilePos();
	virtual void SetFilePos(TFileOffset);

	virtual void Read(Ref(CByteBuffer) _buffer);
	virtual void Read(Ref(CByteLinkedBuffer) _buffer);
	virtual void Write(ConstRef(CByteBuffer) _buffer);
	virtual void Write(ConstRef(CByteLinkedBuffer) _buffer);

private:
#ifdef OK_SYS_WINDOWS
	HANDLE m_hChildStd_IN_Rd;
	HANDLE m_hChildStd_IN_Wr;
	HANDLE m_hChildStd_OUT_Rd;
	HANDLE m_hChildStd_OUT_Wr;
#endif
#ifdef OK_SYS_UNIX
	int m_stdin[2];
	int m_stdout[2];
#endif
};

DECL_WINEXCEPTION(WINSOURCES_API, CProcessException, CWinException)
