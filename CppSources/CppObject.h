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

#ifndef OK_MODERN_CPLUSPLUS
template <typename cppobj>
class CCppObjectPtr
{
public:

	typedef cppobj type;

	CCppObjectPtr(Ptr(type) p) : _p(p)
	{
		assert(_p);
	}
	CCppObjectPtr(ConstRef(CCppObjectPtr) _copy) : _p(_copy._p) 
	{
		addRef(); 
	}
	~CCppObjectPtr() 
	{
		if (_p)
			release(); 
	}

	Ref(CCppObjectPtr) operator=(ConstRef(CCppObjectPtr) _copy)
	{
		if (this != &_copy)
		{
			_p = _copy._p;
			addRef();
		}
		return *this;
	}

	Ptr(type) get() const
	{
		return _p;
	}

	Ptr(type) operator->() const
	{ 
		assert(_p);
		return _p; 
	}

	Ref(type) operator*() const
	{ 
		assert(_p);
		return *_p; 
	}

	sdword refCount()
	{
		assert(_p);
		return _p->refCount();
	}

	sdword release()
	{
		sdword _ret;

		assert(_p);
		_ret = _p->release();
		_p = NULL;
		return _ret;
	}

private:
	sdword addRef()
	{
		assert(_p);
		return _p->addRef();
	}

	Ptr(type) _p;
};

template <typename T>
class CCppObjectReleaseFunctor
{
public:
	Ref(CCppObjectReleaseFunctor) operator()(Ptr(T) p)
	{
		if (p)
			p->release();
		return *this;
	}
};

template <typename T>
class CCppObjectNullFunctor
{
public:
	Ref(CCppObjectNullFunctor) operator()(Ptr(T) p)
	{
		return *this;
	}
};

template <typename C, typename D>
void __stdcall TCppObjectReleaseFunc(ConstPointer data, Pointer context)
{
	Ptr(C) pC = CastAnyPtr(C, CastMutable(Pointer, data));
	Ptr(D) pD = CastAnyPtr(D, context);

	(*pD)(pC);
}

template <typename T>
class CCppObjectLessFunctor
{
public:
	bool operator()(ConstPtr(T) r1, ConstPtr(T) r2) const
	{
		return r1 < r2;
	}
};

template <typename C, typename D>
sword __stdcall TCppObjectSearchAndSortUserFunc(ConstPointer ArrayItem, ConstPointer DataItem, Pointer Context)
{
	ConstPtr(C) pCA = CastAnyConstPtr(C, ArrayItem);
	ConstPtr(C) pCD = CastAnyConstPtr(C, DataItem);
	Ptr(D) pD = CastAnyPtr(D, Context);

	if ((*pD)(pCA, pCD))
		return -1;
	if ((*pD)(pCD, pCA))
		return 1;
	return 0;
}

template <typename T>
class CCppObjectForEachFunctor
{
public:
	bool operator()(Ptr(T) r1)
	{
		return true;
	}
};

template <typename C, typename D>
bool __stdcall TCppObjectForEachFunc(ConstPointer data, Pointer context)
{
	Ptr(C) pC = CastAnyPtr(C, CastMutable(Pointer, data));
	Ptr(D) pD = CastAnyPtr(D, context);

	return (*pD)(pC);
}
#endif