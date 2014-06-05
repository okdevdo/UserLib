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
#include "HashBuffer.h"
#include "File.h"
#include "hash.h"

CDataVectorT<CStringBuffer> CHashBuffer::_algos(__FILE__LINE__ 16, 16);

CHashBuffer::CHashBuffer():
_algo(0),
_hexBuffer(),
_hashData(nullptr)
{
}

CHashBuffer::CHashBuffer(word algo, Ptr(CFile) _file):
_algo(0),
_hexBuffer(),
_hashData(nullptr)
{
	Init(algo);
	Append(_file);
	Finish();
}

CHashBuffer::CHashBuffer(ConstRef(CHashBuffer) _copy):
_algo(_copy._algo),
_hexBuffer(_copy._hexBuffer),
_hashData(_copy._hashData)
{
}

CHashBuffer::~CHashBuffer()
{
	if (_hashData)
		TFfree(_hashData);
}

ConstRef(CHashBuffer) CHashBuffer::operator = (ConstRef(CHashBuffer) _copy)
{
	_algo = _copy._algo;
	_hexBuffer = _copy._hexBuffer;
	_hashData = _copy._hashData;
	return *this;
}

void CHashBuffer::Init(word algo)
{
	_algo = algo;
	if (_hashData)
		TFfree(_hashData);
	_hashData = nullptr;
	switch (_algo)
	{
	case 0:
		_hashData = TFalloc(sizeof(MD2_CTX));
		MD2Init(CastAnyPtr(MD2_CTX, _hashData));
		break;
	case 1:
		_hashData = TFalloc(sizeof(MD4_CTX));
		MD4Init(CastAnyPtr(MD4_CTX, _hashData));
		break;
	case 2:
		_hashData = TFalloc(sizeof(MD5_CTX));
		MD5Init(CastAnyPtr(MD5_CTX, _hashData));
		break;
	case 3:
		_hashData = TFalloc(sizeof(SHA1_CTX));
		SHA1Init(CastAnyPtr(SHA1_CTX, _hashData));
		break;
	case 4:
		_hashData = TFalloc(sizeof(SHA224_CTX));
		SHA224Init(CastAnyPtr(SHA224_CTX, _hashData));
		break;
	case 5:
		_hashData = TFalloc(sizeof(SHA256_CTX));
		SHA256Init(CastAnyPtr(SHA256_CTX, _hashData));
		break;
	case 6:
		_hashData = TFalloc(sizeof(SHA384_CTX));
		SHA384Init(CastAnyPtr(SHA384_CTX, _hashData));
		break;
	case 7:
		_hashData = TFalloc(sizeof(SHA512_CTX));
		SHA512Init(CastAnyPtr(SHA512_CTX, _hashData));
		break;
	default:
		break;
	}
}

void CHashBuffer::Append(BConstPointer buf, dword buflen)
{
	switch (_algo)
	{
	case 0:
		MD2Update(CastAnyPtr(MD2_CTX, _hashData), buf, buflen);
		break;
	case 1:
		MD4Update(CastAnyPtr(MD4_CTX, _hashData), buf, buflen);
		break;
	case 2:
		MD5Update(CastAnyPtr(MD5_CTX, _hashData), buf, buflen);
		break;
	case 3:
		SHA1Update(CastAnyPtr(SHA1_CTX, _hashData), buf, buflen);
		break;
	case 4:
		SHA224Update(CastAnyPtr(SHA224_CTX, _hashData), buf, buflen);
		break;
	case 5:
		SHA256Update(CastAnyPtr(SHA256_CTX, _hashData), buf, buflen);
		break;
	case 6:
		SHA384Update(CastAnyPtr(SHA384_CTX, _hashData), buf, buflen);
		break;
	case 7:
		SHA512Update(CastAnyPtr(SHA512_CTX, _hashData), buf, buflen);
		break;
	default:
		break;
	}
}

void CHashBuffer::Append(ConstRef(CByteBuffer) buf)
{
	switch (_algo)
	{
	case 0:
		MD2Update(CastAnyPtr(MD2_CTX, _hashData), buf.get_Buffer(), buf.get_BufferSize());
		break;
	case 1:
		MD4Update(CastAnyPtr(MD4_CTX, _hashData), buf.get_Buffer(), buf.get_BufferSize());
		break;
	case 2:
		MD5Update(CastAnyPtr(MD5_CTX, _hashData), buf.get_Buffer(), buf.get_BufferSize());
		break;
	case 3:
		SHA1Update(CastAnyPtr(SHA1_CTX, _hashData), buf.get_Buffer(), buf.get_BufferSize());
		break;
	case 4:
		SHA224Update(CastAnyPtr(SHA224_CTX, _hashData), buf.get_Buffer(), buf.get_BufferSize());
		break;
	case 5:
		SHA256Update(CastAnyPtr(SHA256_CTX, _hashData), buf.get_Buffer(), buf.get_BufferSize());
		break;
	case 6:
		SHA384Update(CastAnyPtr(SHA384_CTX, _hashData), buf.get_Buffer(), buf.get_BufferSize());
		break;
	case 7:
		SHA512Update(CastAnyPtr(SHA512_CTX, _hashData), buf.get_Buffer(), buf.get_BufferSize());
		break;
	default:
		break;
	}
}

void CHashBuffer::Append(Ptr(CFile) _file)
{
	CByteBuffer buf(__FILE__LINE__ 16384);

	_file->Read(buf);
	while (buf.get_BufferSize() > 0)
	{
		Append(buf);
		_file->Read(buf);
	}
}

void CHashBuffer::Finish()
{
	CByteBuffer buf(__FILE__LINE__ get_DigestSize());

	switch (_algo)
	{
	case 0:
		MD2Finish(CastAnyPtr(MD2_CTX, _hashData), buf.get_Buffer());
		break;
	case 1:
		MD4Finish(CastAnyPtr(MD4_CTX, _hashData), buf.get_Buffer());
		break;
	case 2:
		MD5Finish(CastAnyPtr(MD5_CTX, _hashData), buf.get_Buffer());
		break;
	case 3:
		SHA1Finish(CastAnyPtr(SHA1_CTX, _hashData), buf.get_Buffer());
		break;
	case 4:
		SHA224Finish(CastAnyPtr(SHA224_CTX, _hashData), buf.get_Buffer());
		break;
	case 5:
		SHA256Finish(CastAnyPtr(SHA256_CTX, _hashData), buf.get_Buffer());
		break;
	case 6:
		SHA384Finish(CastAnyPtr(SHA384_CTX, _hashData), buf.get_Buffer());
		break;
	case 7:
		SHA512Finish(CastAnyPtr(SHA512_CTX, _hashData), buf.get_Buffer());
		break;
	default:
		break;
	}
	_hexBuffer.convertToHex(buf);
	TFfree(_hashData);
	_hashData = nullptr;
}

word CHashBuffer::get_DigestSize() const
{
	static word _digestsize[] = { 16, 16, 16, 20, 28, 32, 48, 64 };

	assert(_algo < 8);
	return _digestsize[_algo];
}

word CHashBuffer::get_BlockSize() const
{
	static word _buffersize[] = { 16, 64, 64, 64, 64, 64, 128, 128 };

	assert(_algo < 8);
	return _buffersize[_algo];
}

ConstRef(CStringBuffer) CHashBuffer::get_Algorithm() const
{
	return CStringBuffer::null();
}

ConstRef(CDataVectorT<CStringBuffer>) CHashBuffer::get_Algorithms()
{
	return _algos;
}

sword CHashBuffer::find_Algorithm(ConstRef(CStringBuffer) key)
{
	return -1;
}

