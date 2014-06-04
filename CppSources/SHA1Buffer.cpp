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
#include "SHA1Buffer.h"
#include "SHA1.H"

CSHA1Buffer::CSHA1Buffer(void):
    _hexBuffer(), _sha1Data(nullptr)
{
}

CSHA1Buffer::CSHA1Buffer(ConstRef(CStringBuffer) _hexData):
    _hexBuffer(_hexData), _sha1Data(nullptr)
{
}

CSHA1Buffer::CSHA1Buffer(Ptr(CFile) _file):
    _hexBuffer(), _sha1Data(nullptr)
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

CSHA1Buffer::CSHA1Buffer(ConstRef(CSHA1Buffer) _copy):
    _hexBuffer(_copy._hexBuffer), _sha1Data(nullptr)
{
}

CSHA1Buffer::~CSHA1Buffer(void)
{
	if ( NotPtrCheck(_sha1Data) )
		TFfree(_sha1Data);
	_sha1Data = nullptr;
}

void CSHA1Buffer::operator = (ConstRef(CStringBuffer) _copy)
{
	if ( NotPtrCheck(_sha1Data) )
		TFfree(_sha1Data);
	_sha1Data = nullptr;
	_hexBuffer = _copy;
}

void CSHA1Buffer::operator = (ConstRef(CSHA1Buffer) _copy)
{
	if ( NotPtrCheck(_sha1Data) )
		TFfree(_sha1Data);
	_sha1Data = nullptr;
	_hexBuffer = _copy._hexBuffer;
}

void CSHA1Buffer::Init()
{
	if ( NotPtrCheck(_sha1Data) )
		TFfree(_sha1Data);
	if (PtrCheck(_sha1Data = TFalloc(sizeof(SHA1_CTX))))
		return;
	sha1_init(CastAnyPtr(SHA1_CTX, _sha1Data));
}

void CSHA1Buffer::Append(ConstRef(CByteBuffer) buf)
{
	if ( PtrCheck(_sha1Data) )
		return;
	sha1_update(CastAnyPtr(SHA1_CTX, _sha1Data), buf.get_Buffer(), buf.get_BufferSize());
}

void CSHA1Buffer::Finish()
{
	if ( PtrCheck(_sha1Data) )
		return;

	CByteBuffer digest(__FILE__LINE__ SHA1_DIGEST_SIZE);

	sha1_finish(CastAnyPtr(SHA1_CTX, _sha1Data), digest.get_Buffer());
	_hexBuffer.convertToHex(digest);

	TFfree(_sha1Data);
	_sha1Data = nullptr;
}
