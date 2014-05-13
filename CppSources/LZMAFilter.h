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
#include "LZMALibFilter.h"

class CPPSOURCES_API CLZMACompressFilter :
	public CLZMALibCompressFilter
{
public:
	CLZMACompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	virtual ~CLZMACompressFilter(void);

	virtual void open();
	virtual dword do_filter();

private:
	CLZMACompressFilter(void);
};

class CPPSOURCES_API CLZMADeCompressFilter :
	public CLZMALibDeCompressFilter
{
public:
	CLZMADeCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	virtual ~CLZMADeCompressFilter(void);

	virtual void open();
	virtual dword do_filter();

private:
	CLZMADeCompressFilter(void);
};

