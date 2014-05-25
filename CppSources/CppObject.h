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
 *  \file CppObject.h
 *  \brief Base class of all objects allocated on the heap.
 */
#pragma once

#include "CppSources.h"

class CPPSOURCES_API CCppObject
{
public:
	CCppObject(void);
	virtual ~CCppObject(void);

	void *operator new(size_t size);
#ifdef __DEBUG__
	void *operator new(size_t size, const char*, int);
	void operator delete(void *p, const char*, int);
#define OK_NEW_OPERATOR new (__FILE__, __LINE__)
#else
#define OK_NEW_OPERATOR new
#endif
	void operator delete(void *p);

	sdword addRef();
	sdword refCount();
	sdword release();
	
};

