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
#include "LZMALibFilter.h"

#define LZMA_API_STATIC
#include "lzma.h"

#undef BUFSIZE
#define BUFSIZE 16384

CLZMALibCompressFilter::CLZMALibCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
	CFilter(input, output), m_stream(nullptr) {}
CLZMALibCompressFilter::~CLZMALibCompressFilter(){}

dword CLZMALibCompressFilter::do_filter()
{
	lzma_action action = LZMA_RUN;
	CByteBuffer inbuf(__FILE__LINE__ BUFSIZE);
	CByteBuffer outbuf(__FILE__LINE__ BUFSIZE);

	CastAnyPtr(lzma_stream, m_stream)->next_in = nullptr;
	CastAnyPtr(lzma_stream, m_stream)->avail_in = 0;
	CastAnyPtr(lzma_stream, m_stream)->next_out = outbuf.get_Buffer();
	CastAnyPtr(lzma_stream, m_stream)->avail_out = outbuf.get_BufferSize();

	while (true)
	{
		if ((CastAnyPtr(lzma_stream, m_stream)->avail_in == 0) && (inbuf.get_BufferSize() != 0))
		{
			read(inbuf);
			CastAnyPtr(lzma_stream, m_stream)->next_in = inbuf.get_Buffer();
			CastAnyPtr(lzma_stream, m_stream)->avail_in = inbuf.get_BufferSize();
			if (CastAnyPtr(lzma_stream, m_stream)->avail_in == 0)
				action = LZMA_FINISH;
		}

		lzma_ret ret = lzma_code(CastAnyPtr(lzma_stream, m_stream), action);

		if ((CastAnyPtr(lzma_stream, m_stream)->avail_out == 0) || (ret == LZMA_STREAM_END))
		{
			size_t write_size = outbuf.get_BufferSize() - CastAnyPtr(lzma_stream, m_stream)->avail_out;

			outbuf.set_BufferSize(__FILE__LINE__ Cast(dword, write_size));
			write(outbuf);
			outbuf.set_BufferSize(__FILE__LINE__ BUFSIZE);
			CastAnyPtr(lzma_stream, m_stream)->next_out = outbuf.get_Buffer();
			CastAnyPtr(lzma_stream, m_stream)->avail_out = outbuf.get_BufferSize();
		}

		if (ret != LZMA_OK)
		{
			if (ret == LZMA_STREAM_END)
				return 0;

			switch (ret)
			{
			case LZMA_MEM_ERROR:
				throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("LZMA_MEM_ERROR"));
				break;
			case LZMA_DATA_ERROR:
				throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("LZMA_DATA_ERROR"));
				break;
			default:
				throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("LZMA_BUG"));
				break;
			}
			return 0;
		}
	}
	return 0;
}

void CLZMALibCompressFilter::close()
{
	lzma_end(CastAnyPtr(lzma_stream, m_stream));

	if (NotPtrCheck(m_stream))
		TFfree(m_stream);

	CFilter::close();
}

CLZMALibDeCompressFilter::CLZMALibDeCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
	CFilter(input, output), m_stream(nullptr) {}
CLZMALibDeCompressFilter::~CLZMALibDeCompressFilter() {}

dword CLZMALibDeCompressFilter::do_filter()
{
	lzma_action action = LZMA_RUN;
	CByteBuffer inbuf(__FILE__LINE__ BUFSIZE);
	CByteBuffer outbuf(__FILE__LINE__ BUFSIZE);

	CastAnyPtr(lzma_stream, m_stream)->next_in = nullptr;
	CastAnyPtr(lzma_stream, m_stream)->avail_in = 0;
	CastAnyPtr(lzma_stream, m_stream)->next_out = outbuf.get_Buffer();
	CastAnyPtr(lzma_stream, m_stream)->avail_out = outbuf.get_BufferSize();

	while (true)
	{
		if ((CastAnyPtr(lzma_stream, m_stream)->avail_in == 0) && (inbuf.get_BufferSize() != 0))
		{
			read(inbuf);
			CastAnyPtr(lzma_stream, m_stream)->next_in = inbuf.get_Buffer();
			CastAnyPtr(lzma_stream, m_stream)->avail_in = inbuf.get_BufferSize();

			if (CastAnyPtr(lzma_stream, m_stream)->avail_in == 0)
				action = LZMA_FINISH;
		}

		lzma_ret ret = lzma_code(CastAnyPtr(lzma_stream, m_stream), action);

		if ((CastAnyPtr(lzma_stream, m_stream)->avail_out == 0) || (ret == LZMA_STREAM_END))
		{
			size_t write_size = outbuf.get_BufferSize() - CastAnyPtr(lzma_stream, m_stream)->avail_out;

			outbuf.set_BufferSize(__FILE__LINE__ Cast(dword, write_size));
			write(outbuf);
			outbuf.set_BufferSize(__FILE__LINE__ BUFSIZE);
			CastAnyPtr(lzma_stream, m_stream)->next_out = outbuf.get_Buffer();
			CastAnyPtr(lzma_stream, m_stream)->avail_out = outbuf.get_BufferSize();
		}

		if (ret != LZMA_OK)
		{
			if (ret == LZMA_STREAM_END)
				return 0;

			switch (ret)
			{
			case LZMA_MEM_ERROR:
				throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("LZMA_MEM_ERROR"));
				break;
			case LZMA_FORMAT_ERROR:
				throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("LZMA_FORMAT_ERROR"));
				break;
			case LZMA_OPTIONS_ERROR:
				throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("LZMA_OPTIONS_ERROR"));
				break;
			case LZMA_DATA_ERROR:
				throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("LZMA_DATA_ERROR"));
				break;
			case LZMA_BUF_ERROR:
				throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("LZMA_BUF_ERROR"));
				break;
			default:
				throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("LZMA_BUG"));
				break;
			}
			return 0;
		}
	}
	return 0;
}

void CLZMALibDeCompressFilter::close()
{
	lzma_end(CastAnyPtr(lzma_stream, m_stream));

	if (NotPtrCheck(m_stream))
		TFfree(m_stream);

	CFilter::close();
}
