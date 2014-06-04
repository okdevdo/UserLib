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
#include "BZip2Filter.h"

#include "bzlib.h"

static void *bzalloc(void *opaque, int size, int num)
{
	return TFalloc(size * num);
}

static void bzfree(void *opaque, void *p)
{
	TFfree(p);
}

CBZip2CompressFilter::CBZip2CompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
    CFilter(input, output), m_stream(nullptr)
{
}

CBZip2CompressFilter::~CBZip2CompressFilter(void)
{
}

void CBZip2CompressFilter::open()
{
	CFilter::open();

	m_stream = TFalloc(sizeof(bz_stream));
	if ( PtrCheck(m_stream) )
		throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("BZ2_bzCompressInit"));
	CastAnyPtr(bz_stream, m_stream)->bzalloc = &bzalloc;
	CastAnyPtr(bz_stream, m_stream)->bzfree = &bzfree;

	int res = BZ2_bzCompressInit(CastAnyPtr(bz_stream, m_stream), 9, 0, 0);

	if ( res != BZ_OK )
		throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("BZ2_bzCompressInit"));
}

dword CBZip2CompressFilter::do_filter()
{
	CByteBuffer inputbuf(__FILE__LINE__ 16384);
	CByteBuffer outputbuf(__FILE__LINE__ 16384);
	int res = BZ_RUN_OK;

	CastAnyPtr(bz_stream, m_stream)->next_out = CastAnyPtr(char, outputbuf.get_Buffer());
	CastAnyPtr(bz_stream, m_stream)->avail_out = outputbuf.get_BufferSize();
	while ( true )
	{
		inputbuf.set_BufferSize(__FILE__LINE__ 16384);
		read(inputbuf);
		if ( inputbuf.get_BufferSize() == 0 )
			break;

		CastAnyPtr(bz_stream, m_stream)->next_in = CastAnyPtr(char, inputbuf.get_Buffer());
		CastAnyPtr(bz_stream, m_stream)->avail_in = inputbuf.get_BufferSize();

		do
		{
			res = BZ2_bzCompress(CastAnyPtr(bz_stream, m_stream), BZ_RUN);
			if ( res != BZ_RUN_OK )
				throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("BZ2_bzCompress"));
			if ( CastAnyPtr(bz_stream, m_stream)->avail_out != outputbuf.get_BufferSize() )
			{
				outputbuf.set_BufferSize(__FILE__LINE__ outputbuf.get_BufferSize() - CastAnyPtr(bz_stream, m_stream)->avail_out);
				write(outputbuf);
				outputbuf.set_BufferSize(__FILE__LINE__ 16384);
				CastAnyPtr(bz_stream, m_stream)->next_out = CastAnyPtr(char, outputbuf.get_Buffer());
				CastAnyPtr(bz_stream, m_stream)->avail_out = outputbuf.get_BufferSize();
			}
		} while ( CastAnyPtr(bz_stream, m_stream)->avail_in > 0 );
	}

	CastAnyPtr(bz_stream, m_stream)->next_in = nullptr;
	CastAnyPtr(bz_stream, m_stream)->avail_in = 0;

	while ( res != BZ_STREAM_END )
	{
		res = BZ2_bzCompress(CastAnyPtr(bz_stream, m_stream), BZ_FINISH);
		if ( (res != BZ_FINISH_OK) && (res != BZ_STREAM_END) )
			throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("BZ2_bzCompress"));
		if ( CastAnyPtr(bz_stream, m_stream)->avail_out != outputbuf.get_BufferSize() )
		{
			outputbuf.set_BufferSize(__FILE__LINE__ outputbuf.get_BufferSize() - CastAnyPtr(bz_stream, m_stream)->avail_out);
			write(outputbuf);
			outputbuf.set_BufferSize(__FILE__LINE__ 16384);
			CastAnyPtr(bz_stream, m_stream)->next_out = CastAnyPtr(char, outputbuf.get_Buffer());
			CastAnyPtr(bz_stream, m_stream)->avail_out = outputbuf.get_BufferSize();
		}
	}
	return 0;
}

void CBZip2CompressFilter::close()
{
	int res = BZ2_bzCompressEnd(CastAnyPtr(bz_stream, m_stream));

	if ( res != BZ_OK )
		throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("BZ2_bzCompressEnd"));
	if ( NotPtrCheck(m_stream) )
		TFfree(m_stream);

	CFilter::close();
}

CBZip2DeCompressFilter::CBZip2DeCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
    CFilter(input, output), m_stream(nullptr)
{
}

CBZip2DeCompressFilter::~CBZip2DeCompressFilter(void)
{
}

void CBZip2DeCompressFilter::open()
{
	CFilter::open();

	m_stream = TFalloc(sizeof(bz_stream));
	if ( PtrCheck(m_stream) )
		throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("BZ2_bzDecompressInit"));
	CastAnyPtr(bz_stream, m_stream)->bzalloc = &bzalloc;
	CastAnyPtr(bz_stream, m_stream)->bzfree = &bzfree;

	int res = BZ2_bzDecompressInit(CastAnyPtr(bz_stream, m_stream), 0, 0);

	if ( res != BZ_OK )
		throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("BZ2_bzDecompressInit"));
}

dword CBZip2DeCompressFilter::do_filter()
{
	CByteBuffer inputbuf(__FILE__LINE__ 16384);
	CByteBuffer outputbuf(__FILE__LINE__ 16384);
	int res = BZ_OK;

	CastAnyPtr(bz_stream, m_stream)->next_out = CastAnyPtr(char, outputbuf.get_Buffer());
	CastAnyPtr(bz_stream, m_stream)->avail_out = outputbuf.get_BufferSize();
	while ( res != BZ_STREAM_END )
	{
		inputbuf.set_BufferSize(__FILE__LINE__ 16384);
		read(inputbuf);
		if ( inputbuf.get_BufferSize() == 0 )
			break;

		CastAnyPtr(bz_stream, m_stream)->next_in = CastAnyPtr(char, inputbuf.get_Buffer());
		CastAnyPtr(bz_stream, m_stream)->avail_in = inputbuf.get_BufferSize();

		do 
		{
			res = BZ2_bzDecompress(CastAnyPtr(bz_stream, m_stream));
			if ( (res != BZ_OK) && (res != BZ_STREAM_END) )
				throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("BZ2_bzDecompress"));

			if ( CastAnyPtr(bz_stream, m_stream)->avail_out != outputbuf.get_BufferSize() )
			{
				outputbuf.set_BufferSize(__FILE__LINE__ outputbuf.get_BufferSize() - CastAnyPtr(bz_stream, m_stream)->avail_out);
				write(outputbuf);
				outputbuf.set_BufferSize(__FILE__LINE__ 16384);
				CastAnyPtr(bz_stream, m_stream)->next_out = CastAnyPtr(char, outputbuf.get_Buffer());
				CastAnyPtr(bz_stream, m_stream)->avail_out = outputbuf.get_BufferSize();
			}
		} while ( (res != BZ_STREAM_END) && (CastAnyPtr(bz_stream, m_stream)->avail_in > 0) );
	}
	if ( res != BZ_STREAM_END )
	{
		CastAnyPtr(bz_stream, m_stream)->next_in = nullptr;
		CastAnyPtr(bz_stream, m_stream)->avail_in = 0;

		do 
		{
			res = BZ2_bzDecompress(CastAnyPtr(bz_stream, m_stream));
			if ( (res != BZ_OK) && (res != BZ_STREAM_END) )
				throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("BZ2_bzDecompress"));

			if ( CastAnyPtr(bz_stream, m_stream)->avail_out != outputbuf.get_BufferSize() )
			{
				outputbuf.set_BufferSize(__FILE__LINE__ outputbuf.get_BufferSize() - CastAnyPtr(bz_stream, m_stream)->avail_out);
				write(outputbuf);
				outputbuf.set_BufferSize(__FILE__LINE__ 16384);
				CastAnyPtr(bz_stream, m_stream)->next_out = CastAnyPtr(char, outputbuf.get_Buffer());
				CastAnyPtr(bz_stream, m_stream)->avail_out = outputbuf.get_BufferSize();
			}
		} while ( res != BZ_STREAM_END );
	}
	return 0;
}

void CBZip2DeCompressFilter::close()
{
	int res = BZ2_bzDecompressEnd(CastAnyPtr(bz_stream, m_stream));

	if ( res != BZ_OK )
		throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("BZ2_bzDecompressEnd"));
	if ( NotPtrCheck(m_stream) )
		TFfree(m_stream);

	CFilter::close();
}

