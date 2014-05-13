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
#include "WinSources.h"
#include "WinException.h"
#include "okDateTime.h"

#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
    #include <sys/types.h>
#endif
#ifdef OK_SYS_UNIX
    #include <sys/types.h>
    #define HANDLE int
	#undef INVALID_HANDLE_VALUE
    #define INVALID_HANDLE_VALUE (-1)
#endif
#endif
#ifdef OK_COMP_MSC
	typedef dword mode_t;
#endif

class WINSOURCES_API CSecurityFile: public CFile
{
public:
	CSecurityFile(void);
	CSecurityFile(ConstRef(CFilePath) _path, WBool _readOnly = true, WBool _textMode = true, TEncoding _encoding = ScanFor_Encoding);
	CSecurityFile(ConstRef(CSecurityFile) _file);
	virtual ~CSecurityFile(void);

	void operator = (ConstRef(CSecurityFile) _file);

	virtual void Open(ConstRef(CFilePath) _path, WBool _readOnly = true, WBool _textMode = true, TEncoding _encoding = ScanFor_Encoding);
	virtual void Create(ConstRef(CFilePath) _path, WBool _textMode = true, TEncoding _encoding = ISO_8859_1_Encoding, mode_t mode = 0644);
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
	HANDLE m_fileHandle;
};

DECL_WINEXCEPTION(WINSOURCES_API, CSecurityFileException, CWinException)
