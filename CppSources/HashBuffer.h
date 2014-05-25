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
/**
 *  \file HashBuffer.h
 *  \brief Digest algorithms.
 */
#pragma once

#include "CppSources.h"
#include "DataVector.h"

class CFile;
class CPPSOURCES_API CHashBuffer
{
public:
	CHashBuffer();
	CHashBuffer(word algo, Ptr(CFile) _file);
	CHashBuffer(ConstRef(CHashBuffer) _copy);
	~CHashBuffer();

	ConstRef(CHashBuffer) operator = (ConstRef(CHashBuffer) _copy);

	void Init(word algo = 0);
	void Append(BConstPointer buf, dword buflen);
	void Append(ConstRef(CByteBuffer) buf);
	void Append(Ptr(CFile) _file);
	void Finish();

	word get_DigestSize() const;
	word get_BlockSize() const;

	__inline ConstRef(CStringBuffer) GetHexBuffer() const { return _hexBuffer; }
	__inline void SetHexBuffer(ConstRef(CStringBuffer) _hexData) { _hexBuffer = _hexData; }

	ConstRef(CStringBuffer) get_Algorithm() const;

	static ConstRef(CDataVectorT<CStringBuffer>) get_Algorithms();
	static sword find_Algorithm(ConstRef(CStringBuffer) key);

protected:
	word _algo;
	static CDataVectorT<CStringBuffer> _algos;

private:
	CStringBuffer _hexBuffer;
	Pointer _hashData;

};

__inline bool operator == (ConstRef(CHashBuffer) a, ConstRef(CHashBuffer) b)
{
	return (a.GetHexBuffer().Compare(b.GetHexBuffer(), 0, CStringLiteral::cIgnoreCase) == 0);
}

__inline bool operator == (ConstRef(CStringBuffer) a, ConstRef(CHashBuffer) b)
{
	return (a.Compare(b.GetHexBuffer(), 0, CStringLiteral::cIgnoreCase) == 0);
}

__inline bool operator == (ConstRef(CHashBuffer) a, ConstRef(CStringBuffer) b)
{
	return (a.GetHexBuffer().Compare(b, 0, CStringLiteral::cIgnoreCase) == 0);
}

__inline bool operator != (ConstRef(CHashBuffer) a, ConstRef(CHashBuffer) b)
{
	return (a.GetHexBuffer().Compare(b.GetHexBuffer(), 0, CStringLiteral::cIgnoreCase) != 0);
}

__inline bool operator != (ConstRef(CStringBuffer) a, ConstRef(CHashBuffer) b)
{
	return (a.Compare(b.GetHexBuffer(), 0, CStringLiteral::cIgnoreCase) != 0);
}

__inline bool operator != (ConstRef(CHashBuffer) a, ConstRef(CStringBuffer) b)
{
	return (a.GetHexBuffer().Compare(b, 0, CStringLiteral::cIgnoreCase) != 0);
}
