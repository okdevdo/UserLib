/******************************************************************************
    
	This file is part of CppSources, which is part of UserLib.

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

#include "CppSources.h"

class CPPSOURCES_API CByteBuffer
{
public:
	CByteBuffer();
	CByteBuffer(DECL_FILE_LINE dword initialSize);
	CByteBuffer(DECL_FILE_LINE BConstPointer buffer, dword bufferSize);
	CByteBuffer(const CByteBuffer& buffer);
	~CByteBuffer(void);

	__inline BPointer get_Buffer() const { return _buffer; }
	__inline dword get_BufferSize() const { return _bufferSize; }
	__inline dword get_AllocSize() const { return (PtrCheck(_buffer)?0:Castdword(TFsize(_buffer))); }
	void set_BufferSize(DECL_FILE_LINE dword bufferSize);
	void set_Buffer(DECL_FILE_LINE BConstPointer buffer, dword bufferSize);
	void concat_Buffer(BConstPointer buffer, dword bufferSize);
	void concat_Buffer(const CByteBuffer& other);

	int find_Char(byte ch, dword startpos = 0) const;
	int find_Text(BPointer str, dword strLen, dword startpos = 0) const;
	void get_SubBuffer(dword startpos, dword chLen, CByteBuffer& result) const;

	void operator=(const CByteBuffer& copy);
	
private:
	BPointer _buffer;
	dword _bufferSize;
};

CPPSOURCES_API bool operator==(const CByteBuffer& buffer1, const CByteBuffer& buffer2);
CPPSOURCES_API bool operator!=(const CByteBuffer& buffer1, const CByteBuffer& buffer2);


