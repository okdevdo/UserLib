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
#include "CPPS_PCH.H"
#include "ByteBuffer.h"

CByteBuffer::CByteBuffer():
	_buffer(NULL), _bufferSize(0)
{
}

CByteBuffer::CByteBuffer(DECL_FILE_LINE dword initialSize):
	_buffer(NULL), _bufferSize(0)
{
	if ( initialSize )
	{
#ifdef __DEBUG__
		_buffer = (BPointer)(TFallocDbg(initialSize, file, line));
#else
		_buffer = (BPointer)(TFalloc(initialSize));
#endif
		_bufferSize = initialSize;
	}
}

CByteBuffer::CByteBuffer(DECL_FILE_LINE BConstPointer buffer, dword bufferSize) :
	_buffer(NULL), _bufferSize(0)
{
	if ( bufferSize )
	{
#ifdef __DEBUG__
		_buffer = (BPointer)(TFallocDbg(bufferSize, file, line));
#else
		_buffer = (BPointer)(TFalloc(bufferSize));
#endif
		if (buffer)
			s_memcpy(_buffer, CastMutable(BPointer, buffer), bufferSize);
		_bufferSize = bufferSize;
	}
}

CByteBuffer::CByteBuffer(const CByteBuffer& buffer):
	_buffer(NULL), _bufferSize(0)
{
	if ( buffer.get_BufferSize() )
	{
		_buffer = buffer.get_Buffer();
		TFincrefcnt(_buffer);
		_bufferSize = buffer.get_BufferSize();
	}
}

CByteBuffer::~CByteBuffer(void)
{
	if ( _buffer )
	{
		if ( TFdecrefcnt(_buffer) == 0 )
			TFfree(_buffer);
	}
}

void CByteBuffer::set_BufferSize(DECL_FILE_LINE dword bufferSize)
{
	if ( bufferSize == 0 )
	{
		if ( _buffer )
		{
			if ( TFdecrefcnt(_buffer) == 0 )
				TFfree(_buffer);
		}
		_buffer = NULL;
		_bufferSize = 0;
		return;
	}
	if ( _buffer && (TFrefcnt(_buffer) == 1) )
	{
		dword bufSize = Castdword(TFsize(_buffer));

		if ( bufferSize <= bufSize )
		{
			_bufferSize = bufferSize;
			return;
		}
	}

#ifdef __DEBUG__
	BPointer buffer = (BPointer)(TFallocDbg(bufferSize, file, line));
#else
	BPointer buffer = (BPointer)(TFalloc(bufferSize));
#endif

	if ( _buffer )
	{
		s_memcpy(buffer, _buffer, Min(bufferSize, _bufferSize));
		if ( TFdecrefcnt(_buffer) == 0 )
			TFfree(_buffer);
	}
	_buffer = buffer;
	_bufferSize = bufferSize;
}

void CByteBuffer::set_Buffer(DECL_FILE_LINE BConstPointer buffer, dword bufferSize)
{
	if ( bufferSize == 0 )
	{
		if ( _buffer )
		{
			if ( TFdecrefcnt(_buffer) == 0 )
				TFfree(_buffer);
		}
		_buffer = NULL;
		_bufferSize = 0;
		return;
	}
	if ( _buffer && (TFrefcnt(_buffer) == 1) )
	{
		dword bufSize = Castdword(TFsize(_buffer));

		if ( bufferSize <= bufSize )
		{
			s_memcpy(_buffer, CastMutable(BPointer, buffer), bufferSize);
			_bufferSize = bufferSize;
			return;
		}
	}

#ifdef __DEBUG__
	BPointer buffer1 = (BPointer)(TFallocDbg(bufferSize, file, line));
#else
	BPointer buffer1 = (BPointer)(TFalloc(bufferSize));
#endif

	s_memcpy(buffer1, CastMutable(BPointer, buffer), bufferSize);
	if ( _buffer )
	{
		if ( TFdecrefcnt(_buffer) == 0 )
			TFfree(_buffer);
	}
	_buffer = buffer1;
	_bufferSize = bufferSize;
}

void CByteBuffer::concat_Buffer(BConstPointer buffer, dword bufferSize)
{
	if ( bufferSize == 0 )
		return;
	if ( _buffer && (TFrefcnt(_buffer) == 1) )
	{
		dword bufSize = Castdword(TFsize(_buffer));

		if ( (bufferSize + _bufferSize) <= bufSize )
		{
			s_memcpy(_buffer + _bufferSize, CastMutable(BPointer, buffer), bufferSize);
			_bufferSize += bufferSize;
			return;
		}
	}

	BPointer buffer1 = (BPointer)(TFalloc(bufferSize + _bufferSize));

	if ( _buffer )
	{
		s_memcpy(buffer1, _buffer, _bufferSize);
		if ( TFdecrefcnt(_buffer) == 0 )
			TFfree(_buffer);
	}
	s_memcpy(buffer1 + _bufferSize, CastMutable(BPointer, buffer), bufferSize);
	_buffer = buffer1;
	_bufferSize += bufferSize;
}

void CByteBuffer::concat_Buffer(const CByteBuffer& other)
{
	concat_Buffer(other.get_Buffer(), other.get_BufferSize());
}

int CByteBuffer::find_Char(byte ch, dword startpos) const
{
	if ( !_buffer )
		return -1;

	if ( startpos >= _bufferSize )
		return -1;

	dword pos = startpos;
	BPointer p = _buffer + startpos;

	while ( pos < _bufferSize )
	{
		if ( *p == ch )
			break;
		++p; ++pos;
	}

	if ( pos == _bufferSize )
		return -1;
	return pos;
}

int CByteBuffer::find_Text(BPointer str, dword strLen, dword startpos) const
{
	if ( (!_buffer) || (!str) || (strLen == 0) )
		return -1;

	if ( startpos >= _bufferSize )
		return -1;

	int pos = find_Char(*str, startpos);

	while ( pos >= 0 )
	{
		BPointer p = _buffer + pos;
		BPointer sp = str;
		dword ix = (dword)pos;
		dword jx = strLen;

		while ( (ix < _bufferSize) && (jx > 0) )
		{
			if ( *p != *sp )
				break;
			++p; ++sp; ++ix; --jx;
		}
		if ( jx == 0 )
			return pos;
		pos = find_Char(*str, pos + 1);
	}
	return pos;
}

void CByteBuffer::get_SubBuffer(dword startpos, dword chLen, CByteBuffer& result) const
{
	if ( (!_buffer) || (chLen == 0) )
	{
		result.set_BufferSize(__FILE__LINE__ 0);
		return;
	}

	if ( startpos >= _bufferSize )
	{
		result.set_BufferSize(__FILE__LINE__ 0);
		return;
	}
	if ( (startpos + chLen) >= _bufferSize )
	{
		result.set_Buffer(__FILE__LINE__ _buffer + startpos, _bufferSize - startpos);
		return;
	}
	result.set_BufferSize(__FILE__LINE__ chLen);
	s_memcpy((void*)(result.get_Buffer()), (void*)(_buffer + startpos), chLen);
}

void CByteBuffer::operator=(const CByteBuffer& copy)
{
	if ( _buffer != copy.get_Buffer() )
	{
		if ( _buffer )
		{
			if ( TFdecrefcnt(_buffer) == 0 )
				TFfree(_buffer);
		}
		if ( PtrCheck(copy.get_Buffer()) )
		{
			_buffer = NULL;
			_bufferSize = 0;
			return;
		}
		_buffer = copy.get_Buffer();
		TFincrefcnt(_buffer);
		_bufferSize = copy.get_BufferSize();
	}
}
	
bool operator==(const CByteBuffer& buffer1, const CByteBuffer& buffer2)
{
	if ( buffer1.get_BufferSize() != buffer2.get_BufferSize() )
		return false;
	BPointer buffer11 = buffer1.get_Buffer();
	BPointer buffer22 = buffer2.get_Buffer();

	for ( dword ix = 0; ix < buffer1.get_BufferSize(); ++ix )
	{
		if ( *buffer11++ != *buffer22++ )
			return false;
	}
	return true;
}

bool operator!=(const CByteBuffer& buffer1, const CByteBuffer& buffer2)
{
	return !(operator==(buffer1, buffer2));
}
