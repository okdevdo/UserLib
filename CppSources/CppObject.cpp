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
#include "CppObject.h"

CCppObject::CCppObject(void)
{
}

CCppObject::~CCppObject(void)
{
}

void *CCppObject::operator new(size_t size)
{
	return TFalloc(Castdword(size)); // refCnt == 1
}

#ifdef __DEBUG__
void *CCppObject::operator new(size_t size, const char* file, int line)
{
	return TFallocDbg(Castdword(size), file, line); // refCnt == 1
}
void CCppObject::operator delete(void *p, const char* file, int line)
{
	TFfree(p);
}

#endif

void CCppObject::operator delete(void *p)
{
	TFfree(p);
}

sdword CCppObject::addRef()
{
	if ( TFisalloc(this) )
		return TFincrefcnt(this);
	return -1;
}

sdword CCppObject::refCount()
{
	if ( TFisalloc(this) )
		return TFrefcnt(this);
	return -1;
}

sdword CCppObject::release()
{
	if ( !(TFisalloc(this)) )
		return -1;
		
	dword cnt = TFdecrefcnt(this);

	if (cnt == 0)
		delete this;
	return cnt;
}
