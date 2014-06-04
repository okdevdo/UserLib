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
#include "MD5Buffer.h"
#include "MD5.H"

CMD5Buffer::CMD5Buffer(void):
    _hexBuffer(), _md5Data(nullptr)
{
}

CMD5Buffer::CMD5Buffer(ConstRef(CStringBuffer) _hexData):
    _hexBuffer(_hexData), _md5Data(nullptr)
{
}

CMD5Buffer::CMD5Buffer(Ptr(CFile) _file):
    _hexBuffer(), _md5Data(nullptr)
{
	CByteBuffer buf(__FILE__LINE__ 16384);

	Init();
	_file->Read(buf);
	while ( buf.get_BufferSize() > 0 )
	{
		Append(buf);
		_file->Read(buf);
	}
	Finish();
}

CMD5Buffer::CMD5Buffer(ConstRef(CMD5Buffer) _copy):
    _hexBuffer(_copy._hexBuffer), _md5Data(nullptr)
{
}

CMD5Buffer::~CMD5Buffer(void)
{
	if ( NotPtrCheck(_md5Data) )
		TFfree(_md5Data);
	_md5Data = nullptr;
}

void CMD5Buffer::operator = (ConstRef(CStringBuffer) _copy)
{
	if ( NotPtrCheck(_md5Data) )
		TFfree(_md5Data);
	_md5Data = nullptr;
	_hexBuffer = _copy;
}

void CMD5Buffer::operator = (ConstRef(CMD5Buffer) _copy)
{
	if ( NotPtrCheck(_md5Data) )
		TFfree(_md5Data);
	_md5Data = nullptr;
	_hexBuffer = _copy._hexBuffer;
}

void CMD5Buffer::Init()
{
	if ( NotPtrCheck(_md5Data) )
		TFfree(_md5Data);
	if ( PtrCheck(_md5Data = TFalloc(sizeof(md5_state_t))) )
		return;
	md5_init(CastAnyPtr(md5_state_t, _md5Data));
}

void CMD5Buffer::Append(ConstRef(CByteBuffer) buf)
{
	if ( PtrCheck(_md5Data) )
		return;
	md5_append(CastAnyPtr(md5_state_t, _md5Data), buf.get_Buffer(), buf.get_BufferSize());
}

void CMD5Buffer::Finish()
{
	if ( PtrCheck(_md5Data) )
		return;

	CByteBuffer digest(__FILE__LINE__ MD5_DIGEST_SIZE);

	md5_finish(CastAnyPtr(md5_state_t, _md5Data), digest.get_Buffer());
	_hexBuffer.convertToHex(digest);

	TFfree(_md5Data);
	_md5Data = nullptr;
}
