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
#include "GZipFilter.h"

#define Z_SOLO
#include "zlib.h"

static void *zalloc(void *opaque, unsigned int size, unsigned int num)
{
	return TFalloc(size * num);
}

static void zfree(void *opaque, void *p)
{
	if (p)
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

CGZipCompressFilter::CGZipCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
CZLibCompressFilter(input, output), m_head(nullptr)
{
}

CGZipCompressFilter::~CGZipCompressFilter(void)
{
}

void CGZipCompressFilter::open()
{
	CFilter::open();

	m_stream = TFalloc(sizeof(z_stream));
	if ( PtrCheck(m_stream) )
		throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("CFilterException in %s"), _T("CGZipCompressFilter::open"), ENOMEM);

	CastAnyPtr(z_stream,m_stream)->zalloc = &zalloc;
	CastAnyPtr(z_stream,m_stream)->zfree = &zfree;

	int res = deflateInit2(CastAnyPtr(z_stream, m_stream), Z_DEFAULT_COMPRESSION, Z_DEFLATED, 16 + MAX_WBITS, 8, Z_DEFAULT_STRATEGY);

	if ( res != Z_OK )
		ThrowDefaultException(__FILE__LINE__ _T("deflateInit2"), CastAnyPtr(z_stream,m_stream)->msg);

	m_head = TFalloc(sizeof(gz_header));

	CastAnyPtr(gz_header, m_head)->comment = (Bytef*)"Olivers deflate c++ wrapper.";
	CastAnyPtr(gz_header, m_head)->name = (Bytef*)"Olivers deflate c++ wrapper.";
	CastAnyPtr(gz_header, m_head)->hcrc = 0;
	CastAnyPtr(gz_header, m_head)->os = 11;

	res = deflateSetHeader(CastAnyPtr(z_stream, m_stream), CastAnyPtr(gz_header, m_head));

	if ( res != Z_OK )
		ThrowDefaultException(__FILE__LINE__ _T("deflateSetHeader"), CastAnyPtr(z_stream,m_stream)->msg);
}

void CGZipCompressFilter::close()
{
	if (m_head)
		TFfree(m_head);
	CZLibCompressFilter::close();
}

CGZipDeCompressFilter::CGZipDeCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
    CZLibDeCompressFilter(input, output)
{
}

CGZipDeCompressFilter::~CGZipDeCompressFilter(void)
{
}

void CGZipDeCompressFilter::open()
{
	CFilter::open();

	m_stream = TFalloc(sizeof(z_stream));
	if ( PtrCheck(m_stream) )
		throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("CFilterException in %s"), _T("CGZipDeCompressFilter::open"), ENOMEM);
	CastAnyPtr(z_stream,m_stream)->zalloc = &zalloc;
	CastAnyPtr(z_stream,m_stream)->zfree = &zfree;

	int res = inflateInit2(CastAnyPtr(z_stream, m_stream), 16 + MAX_WBITS);

	if ( res != Z_OK )
		ThrowDefaultException(__FILE__LINE__ _T("inflateInit2"), CastAnyPtr(z_stream,m_stream)->msg);
}
