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
#include "ZLibFilter.h"

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

CZLibCompressFilter::CZLibCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
    CFilter(input, output), m_stream(NULL)
{
}

CZLibCompressFilter::~CZLibCompressFilter(void)
{
}

void CZLibCompressFilter::open()
{
	CFilter::open();

	m_stream = TFalloc(sizeof(z_stream));
	if ( PtrCheck(m_stream) )
		throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("CFilterException in %s"), _T("CZLibCompressFilter::open"), ENOMEM);
	CastAnyPtr(z_stream,m_stream)->zalloc = &zalloc;
	CastAnyPtr(z_stream,m_stream)->zfree = &zfree;

	int res = deflateInit(CastAnyPtr(z_stream,m_stream), Z_DEFAULT_COMPRESSION);

	if ( res != Z_OK )
		ThrowDefaultException(__FILE__LINE__ _T("deflateInit"), CastAnyPtr(z_stream,m_stream)->msg);
}

dword CZLibCompressFilter::do_filter()
{
	CByteBuffer inputbuf(__FILE__LINE__ 16384);
	CByteBuffer outputbuf(__FILE__LINE__ 16384);
	int res = Z_OK;

	CastAnyPtr(z_stream, m_stream)->next_out = outputbuf.get_Buffer();
	CastAnyPtr(z_stream, m_stream)->avail_out = outputbuf.get_BufferSize();
	while ( true )
	{
		inputbuf.set_BufferSize(__FILE__LINE__ 16384);
		read(inputbuf);
		if ( inputbuf.get_BufferSize() == 0 )
			break;

		CastAnyPtr(z_stream, m_stream)->next_in = inputbuf.get_Buffer();
		CastAnyPtr(z_stream, m_stream)->avail_in = inputbuf.get_BufferSize();

		do
		{
			res = deflate(CastAnyPtr(z_stream,m_stream), Z_NO_FLUSH);
			if ( res != Z_OK )
				ThrowDefaultException(__FILE__LINE__ _T("deflate"), CastAnyPtr(z_stream,m_stream)->msg);
			if ( CastAnyPtr(z_stream, m_stream)->avail_out != outputbuf.get_BufferSize() )
			{
				outputbuf.set_BufferSize(__FILE__LINE__ outputbuf.get_BufferSize() - CastAnyPtr(z_stream, m_stream)->avail_out);
				write(outputbuf);
				outputbuf.set_BufferSize(__FILE__LINE__ 16384);
				CastAnyPtr(z_stream, m_stream)->next_out = outputbuf.get_Buffer();
				CastAnyPtr(z_stream, m_stream)->avail_out = outputbuf.get_BufferSize();
			}
		} while ( CastAnyPtr(z_stream, m_stream)->avail_in > 0 );
	}

	CastAnyPtr(z_stream, m_stream)->next_in = NULL;
	CastAnyPtr(z_stream, m_stream)->avail_in = 0;

	while ( res != Z_STREAM_END )
	{
		res = deflate(CastAnyPtr(z_stream,m_stream), Z_FINISH);
		if ( (res != Z_OK) && (res != Z_STREAM_END) )
			ThrowDefaultException(__FILE__LINE__ _T("deflate"), CastAnyPtr(z_stream,m_stream)->msg);
		if ( CastAnyPtr(z_stream, m_stream)->avail_out != outputbuf.get_BufferSize() )
		{
			outputbuf.set_BufferSize(__FILE__LINE__ outputbuf.get_BufferSize() - CastAnyPtr(z_stream, m_stream)->avail_out);
			write(outputbuf);
			outputbuf.set_BufferSize(__FILE__LINE__ 16384);
			CastAnyPtr(z_stream, m_stream)->next_out = outputbuf.get_Buffer();
			CastAnyPtr(z_stream, m_stream)->avail_out = outputbuf.get_BufferSize();
		}
	}
	return 0;
}

void CZLibCompressFilter::close()
{
	int res = deflateEnd(CastAnyPtr(z_stream,m_stream));

	if ( res != Z_OK )
		ThrowDefaultException(__FILE__LINE__ _T("deflateEnd"), CastAnyPtr(z_stream,m_stream)->msg);
	if ( NotPtrCheck(m_stream) )
		TFfree(m_stream);

	CFilter::close();
}

CZLibDeCompressFilter::CZLibDeCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
    CFilter(input, output), m_stream(NULL)
{
}

CZLibDeCompressFilter::~CZLibDeCompressFilter(void)
{
}

void CZLibDeCompressFilter::open()
{
	CFilter::open();

	m_stream = TFalloc(sizeof(z_stream));
	if ( PtrCheck(m_stream) )
		throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("CFilterException in %s"), _T("CZLibDeCompressFilter::open"), ENOMEM);
	CastAnyPtr(z_stream,m_stream)->zalloc = &zalloc;
	CastAnyPtr(z_stream,m_stream)->zfree = &zfree;

	int res = inflateInit(CastAnyPtr(z_stream,m_stream));

	if ( res != Z_OK )
		ThrowDefaultException(__FILE__LINE__ _T("inflateInit"), CastAnyPtr(z_stream,m_stream)->msg);
}

dword CZLibDeCompressFilter::do_filter()
{
	CByteBuffer inputbuf(__FILE__LINE__ 16384);
	CByteBuffer outputbuf(__FILE__LINE__ 16384);
	int res = Z_OK;

	CastAnyPtr(z_stream, m_stream)->next_out = outputbuf.get_Buffer();
	CastAnyPtr(z_stream, m_stream)->avail_out = outputbuf.get_BufferSize();
	while ( res != Z_STREAM_END )
	{
		inputbuf.set_BufferSize(__FILE__LINE__ 16384);
		read(inputbuf);
		if ( inputbuf.get_BufferSize() == 0 )
			break;

		CastAnyPtr(z_stream, m_stream)->next_in = inputbuf.get_Buffer();
		CastAnyPtr(z_stream, m_stream)->avail_in = inputbuf.get_BufferSize();

		do 
		{
			res = inflate(CastAnyPtr(z_stream,m_stream), Z_NO_FLUSH);
			if ( (res != Z_OK) && (res != Z_STREAM_END) )
				ThrowDefaultException(__FILE__LINE__ _T("inflate"), CastAnyPtr(z_stream,m_stream)->msg);
			if ( CastAnyPtr(z_stream, m_stream)->avail_out != outputbuf.get_BufferSize() )
			{
				outputbuf.set_BufferSize(__FILE__LINE__ outputbuf.get_BufferSize() - CastAnyPtr(z_stream, m_stream)->avail_out);
				write(outputbuf);
				outputbuf.set_BufferSize(__FILE__LINE__ 16384);
				CastAnyPtr(z_stream, m_stream)->next_out = outputbuf.get_Buffer();
				CastAnyPtr(z_stream, m_stream)->avail_out = outputbuf.get_BufferSize();
			}
		} while ( (res != Z_STREAM_END) && (CastAnyPtr(z_stream, m_stream)->avail_in > 0) );
	}
	if ( res != Z_STREAM_END )
	{
		CastAnyPtr(z_stream, m_stream)->next_in = NULL;
		CastAnyPtr(z_stream, m_stream)->avail_in = 0;

		do 
		{
			res = inflate(CastAnyPtr(z_stream,m_stream), Z_FINISH);
			if ( (res != Z_OK) && (res != Z_STREAM_END) )
				ThrowDefaultException(__FILE__LINE__ _T("inflate"), CastAnyPtr(z_stream,m_stream)->msg);

			if ( CastAnyPtr(z_stream, m_stream)->avail_out != outputbuf.get_BufferSize() )
			{
				outputbuf.set_BufferSize(__FILE__LINE__ outputbuf.get_BufferSize() - CastAnyPtr(z_stream, m_stream)->avail_out);
				write(outputbuf);
				outputbuf.set_BufferSize(__FILE__LINE__ 16384);
				CastAnyPtr(z_stream, m_stream)->next_out = outputbuf.get_Buffer();
				CastAnyPtr(z_stream, m_stream)->avail_out = outputbuf.get_BufferSize();
			}
		} while ( res != Z_STREAM_END );
	}
	return 0;
}

void CZLibDeCompressFilter::close()
{
	int res = inflateEnd(CastAnyPtr(z_stream,m_stream));

	if ( res != Z_OK )
		ThrowDefaultException(__FILE__LINE__ _T("inflateEnd"), CastAnyPtr(z_stream,m_stream)->msg);
	if ( NotPtrCheck(m_stream) )
		TFfree(m_stream);

	CFilter::close();
}

