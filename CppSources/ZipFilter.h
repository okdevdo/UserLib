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
#include "ZLibFilter.h"

class CPPSOURCES_API CZipCompressFilter :
	public CZLibCompressFilter
{
public:
	CZipCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	virtual ~CZipCompressFilter(void);

	virtual void open();

	virtual void read(Ref(CByteBuffer) inputbuf);
	virtual void write(Ref(CByteBuffer) outputbuf);

	__inline dword get_crc32() const { return m_crc32; }
	__inline dword get_compressedSize() const { return m_compressedSize; }

private:
	CZipCompressFilter(void);
	dword m_crc32;
	dword m_compressedSize;
};

class CPPSOURCES_API CZipDeCompressFilter :
	public CZLibDeCompressFilter
{
public:
	CZipDeCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	virtual ~CZipDeCompressFilter(void);

	virtual void open();

	virtual void write(Ref(CByteBuffer) outputbuf);

	__inline dword get_crc32() const { return m_crc32; }
	__inline dword get_unCompressedSize() const { return m_unCompressedSize; }

private:
	CZipDeCompressFilter(void);
	dword m_crc32;
	dword m_unCompressedSize;
};

