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
#include "Filter.h"

class CPPSOURCES_API CBZip2CompressFilter :
	public CFilter
{
public:
	CBZip2CompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	virtual ~CBZip2CompressFilter(void);

	virtual void open();
	virtual dword do_filter();
	virtual void close();

private:
	Pointer m_stream;

	CBZip2CompressFilter(void);
};

class CPPSOURCES_API CBZip2DeCompressFilter :
	public CFilter
{
public:
	CBZip2DeCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	virtual ~CBZip2DeCompressFilter(void);

	virtual void open();
	virtual dword do_filter();
	virtual void close();

private:
	Pointer m_stream;

	CBZip2DeCompressFilter(void);
};

