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
 *  \file SHA1Buffer.h
 *  \brief Special class calculating the SHA1 digest.
 */
#pragma once

#include "CppSources.h"
#include "File.h"

class CPPSOURCES_API CSHA1Buffer
{
public:
	CSHA1Buffer(void);
	CSHA1Buffer(ConstRef(CStringBuffer) _hexData);
	CSHA1Buffer(Ptr(CFile) _file);
	CSHA1Buffer(ConstRef(CSHA1Buffer) _copy);
	~CSHA1Buffer(void);

	void operator = (ConstRef(CStringBuffer) _copy);
	void operator = (ConstRef(CSHA1Buffer) _copy);

	void Init();
	void Append(ConstRef(CByteBuffer) buf);
	void Finish();

	__inline ConstRef(CStringBuffer) GetDigest() const { return _hexBuffer; }
	__inline void SetDigest(ConstRef(CStringBuffer) _hexData) { _hexBuffer = _hexData; }

private:
	CStringBuffer _hexBuffer;
	Pointer _sha1Data;
};

__inline bool operator == (ConstRef(CSHA1Buffer) a, ConstRef(CSHA1Buffer) b)
{
	return (a.GetDigest().Compare(b.GetDigest(), 0, CStringLiteral::cIgnoreCase) == 0);
}

__inline bool operator != (ConstRef(CSHA1Buffer) a, ConstRef(CSHA1Buffer) b)
{
	return (a.GetDigest().Compare(b.GetDigest(), 0, CStringLiteral::cIgnoreCase) != 0);
}
