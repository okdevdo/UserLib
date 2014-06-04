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
#include "LZMAFilter.h"

#define LZMA_API_STATIC
#include "lzma.h"

static void* LZMA_API_CALL gAllocFunc(void *opaque, size_t nmemb, size_t size)
{
	return TFalloc(Cast(dword, nmemb * size));
}

static void LZMA_API_CALL gFreeFunc(void *opaque, void *ptr)
{
	if (NotPtrCheck(ptr))
		TFfree(ptr);
}

static lzma_allocator gAlloc = { gAllocFunc, gFreeFunc, nullptr };

CLZMACompressFilter::CLZMACompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
    CLZMALibCompressFilter(input, output)
{
}

CLZMACompressFilter::~CLZMACompressFilter(void)
{
}

void CLZMACompressFilter::open()
{
	CLZMALibCompressFilter::open();

	m_stream = TFalloc(sizeof(lzma_stream));
	if (PtrCheck(m_stream))
		throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("TFalloc"));

	lzma_stream tmp = LZMA_STREAM_INIT;
	DerefAnyPtr(lzma_stream, m_stream) = tmp;
	CastAnyPtr(lzma_stream, m_stream)->allocator = &gAlloc;

	lzma_options_lzma* options = CastAnyPtr(lzma_options_lzma, TFalloc(sizeof(lzma_options_lzma)));

	lzma_lzma_preset(options, 6);

	lzma_ret ret = lzma_alone_encoder(CastAnyPtr(lzma_stream, m_stream), options);

	TFfree(options);
	if (ret != LZMA_OK)
	{
		switch (ret)
		{
		case LZMA_MEM_ERROR:
			throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("LZMA_MEM_ERROR"));
			break;
		case LZMA_OPTIONS_ERROR:
			throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("LZMA_OPTIONS_ERROR"));
			break;
		case LZMA_PROG_ERROR:
			throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("LZMA_PROG_ERROR"));
			break;
		default:
			throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("LZMA_BUG"));
			break;
		}
	}
}

dword CLZMACompressFilter::do_filter()
{
	return CLZMALibCompressFilter::do_filter();
}

CLZMADeCompressFilter::CLZMADeCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
	CLZMALibDeCompressFilter(input, output)
{
}

CLZMADeCompressFilter::~CLZMADeCompressFilter(void)
{
}

void CLZMADeCompressFilter::open()
{
	CLZMALibDeCompressFilter::open();

	m_stream = TFalloc(sizeof(lzma_stream));
	if (PtrCheck(m_stream))
		throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("TFalloc"));

	lzma_stream tmp = LZMA_STREAM_INIT;
	DerefAnyPtr(lzma_stream, m_stream) = tmp;
	CastAnyPtr(lzma_stream, m_stream)->allocator = &gAlloc;

	lzma_ret ret = lzma_alone_decoder(CastAnyPtr(lzma_stream, m_stream), UINT64_MAX);

	if (ret != LZMA_OK)
	{
		switch (ret)
		{
		case LZMA_MEM_ERROR:
			throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("LZMA_MEM_ERROR"));
			break;
		case LZMA_PROG_ERROR:
			throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("LZMA_PROG_ERROR"));
			break;
		default:
			throw OK_NEW_OPERATOR CFilterException(__FILE__LINE__ _T("LZMA_BUG"));
			break;
		}
	}
}

dword CLZMADeCompressFilter::do_filter()
{
	return CLZMALibDeCompressFilter::do_filter();
}

