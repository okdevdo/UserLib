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
#include "ZipFilter.h"

#define Z_SOLO
#include "zlib.h"

static void *zalloc(void *opaque, unsigned int size, unsigned int num)
{
	return TFalloc(size * num);
}

static void zfree(void *opaque, void *p)
{
	TFfree(p);
}

static void ThrowDefaultException(DECL_FILE_LINE CConstPointer format, const char* arg1)
{
	CStringBuffer tmp(__FILE__LINE__ format);
	CByteBuffer buf(__FILE__LINE__ CastAny(BConstPointer,arg1), s_strlen(CastMutable(char*,arg1), INT_MAX));
	CStringBuffer tmp1;

	tmp1.convertFromByteBuffer(buf);
	tmp.AppendString(_T(" (%s)"));
	throw OK_NEW_OPERATOR CFilterException(ARGS_FILE_LINE tmp, tmp1.GetString());
}

CZipCompressFilter::CZipCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
    CZLibCompressFilter(input, output), m_crc32(0), m_compressedSize(0)

{
}

CZipCompressFilter::~CZipCompressFilter(void)
{
}

void CZipCompressFilter::open()
{
	CFilter::open();

	m_stream = TFalloc(sizeof(z_stream));
	if ( PtrCheck(m_stream) )
		throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("CFilterException in %s"), _T("CZipCompressFilter::open"), ENOMEM);

	CastAnyPtr(z_stream,m_stream)->zalloc = &zalloc;
	CastAnyPtr(z_stream,m_stream)->zfree = &zfree;

	int res = deflateInit2(CastAnyPtr(z_stream,m_stream), Z_DEFAULT_COMPRESSION, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);

	if ( res != Z_OK )
		ThrowDefaultException(__FILE__LINE__ _T("deflateInit2"), CastAnyPtr(z_stream,m_stream)->msg);
	m_crc32 = crc32(0, Z_NULL, 0);
	m_compressedSize = 0;
}

void CZipCompressFilter::read(Ref(CByteBuffer) inputbuf)
{
	CZLibCompressFilter::read(inputbuf);
	if ( 0 < inputbuf.get_BufferSize() )
		m_crc32 = crc32(m_crc32, inputbuf.get_Buffer(), inputbuf.get_BufferSize());
}

void CZipCompressFilter::write(Ref(CByteBuffer) outputbuf)
{
	CZLibCompressFilter::write(outputbuf);
	m_compressedSize += outputbuf.get_BufferSize();
}

CZipDeCompressFilter::CZipDeCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
    CZLibDeCompressFilter(input, output), m_crc32(0), m_unCompressedSize(0)
{
}

CZipDeCompressFilter::~CZipDeCompressFilter(void)
{
}

void CZipDeCompressFilter::open()
{
	CFilter::open();

	m_stream = TFalloc(sizeof(z_stream));
	if ( PtrCheck(m_stream) )
		throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("CFilterException in %s"), _T("CZipDeCompressFilter::open"), ENOMEM);
	CastAnyPtr(z_stream,m_stream)->zalloc = &zalloc;
	CastAnyPtr(z_stream,m_stream)->zfree = &zfree;

	int res = inflateInit2(CastAnyPtr(z_stream,m_stream), -15);

	if ( res != Z_OK )
		ThrowDefaultException(__FILE__LINE__ _T("inflateInit2"), CastAnyPtr(z_stream,m_stream)->msg);
	m_crc32 = crc32(0, Z_NULL, 0);
	m_unCompressedSize = 0;
}

void CZipDeCompressFilter::write(Ref(CByteBuffer) outputbuf)
{
	CZLibDeCompressFilter::write(outputbuf);

	m_unCompressedSize += outputbuf.get_BufferSize();
	m_crc32 = crc32(m_crc32, outputbuf.get_Buffer(), outputbuf.get_BufferSize());
}
