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
#include <type_traits>

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

	virtual sdword addRef();
	virtual sdword refCount();
	virtual sdword release();
};

template <typename cppobj>
class CCppObjectPtr
{
public:

	typedef CCppObjectPtr this_type;
	typedef cppobj element_type;

	CCppObjectPtr(): _p(NULL) {}
	CCppObjectPtr(Ptr(element_type) p, bool _addref = false) : _p(nullptr)
	{
		attach(p);
		if (_addref)
			addRef();
	}
	CCppObjectPtr(ConstRef(this_type) _copy) : _p(nullptr)
	{
		attach(_copy._p);
		if (NotPtrCheck(_p))
			addRef();
	}
	template <typename other_type>
	CCppObjectPtr(ConstRef(CCppObjectPtr<other_type>) _copy) : _p(nullptr)
	{
		attach(_copy._p);
		if (NotPtrCheck(_p))
			addRef();
	}
	CCppObjectPtr(RefRef(this_type) _copy) : _p(nullptr)
	{
		attach(_copy._p);
		_copy.detach();
	}
	template <typename other_type>
	CCppObjectPtr(RefRef(CCppObjectPtr<other_type>) _copy) : _p(nullptr)
	{
		attach(_copy._p);
		_copy.detach();
	}
	~CCppObjectPtr()
	{
		if (NotPtrCheck(_p))
			release(); 
	}

	Ref(this_type) operator=(Ptr(element_type) _copy)
	{
		if (NotPtrCheck(_p))
			release();
		attach(_copy);
		return *this;
	}
	Ref(this_type) operator=(ConstRef(this_type) _copy)
	{
		if (this != &_copy)
		{
			if (NotPtrCheck(_p))
				release();
			attach(_copy._p);
			if (NotPtrCheck(_p))
				addRef();
		}
		return *this;
	}
	template <typename other_type>
	Ref(this_type) operator=(ConstRef(CCppObjectPtr<other_type>) _copy)
	{
		if (NotPtrCheck(_p))
			release();
		attach(_copy._p);
		if (NotPtrCheck(_p))
			addRef();
		return *this;
	}
	Ref(this_type) operator=(RefRef(this_type) _copy)
	{
		if (this != &_copy)
		{
			if (NotPtrCheck(_p))
				release();
			attach(_copy._p);
			_copy.detach();
		}
		return *this;
	}
	template <typename other_type>
	Ref(this_type) operator=(RefRef(CCppObjectPtr<other_type>) _copy)
	{
		if (NotPtrCheck(_p))
			release();
		attach(_copy._p);
		_copy.detach();
		return *this;
	}

	operator Ptr(element_type)()
	{
		return _p;
	}

	operator Ptr(element_type)() const
	{
		return _p;
	}

	operator bool() const
	{
		return NotPtrCheck(_p);
	}

	Ptr(element_type) get() const
	{
		return _p;
	}

	Ptr(element_type) operator->() const
	{ 
		assert(_p);
		return _p; 
	}

	Ref(element_type) operator*() const
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
		_p = nullptr;
		return _ret;
	}

	sdword addRef()
	{
		assert(_p);
		return _p->addRef();
	}

	void attach(Ptr(element_type) p)
	{
		assert(p);
		_p = p;
	}

	template <typename other_type>
	void attach(Ptr(other_type) p)
	{
		assert(p);
		_p = CastAnyPtr(element_type, p);
	}

	Ptr(element_type) detach()
	{
		Ptr(element_type) p = _p;

		_p = nullptr;
		return p;
	}

private:
	Ptr(element_type) _p;
};

template <typename cppobj>
__inline bool operator == (ConstRef(CCppObjectPtr<cppobj>) a, ConstRef(CCppObjectPtr<cppobj>) b)
{
	return a.get() == b.get();
}

template <typename cppobj>
__inline bool operator == (ConstRef(CCppObjectPtr<cppobj>) a, std::nullptr_t b)
{
	return a.get() == nullptr;
}

template <typename cppobj>
__inline bool operator == (std::nullptr_t a, ConstRef(CCppObjectPtr<cppobj>) b)
{
	return nullptr == b.get();
}

template <typename cppobj>
__inline bool operator != (ConstRef(CCppObjectPtr<cppobj>) a, ConstRef(CCppObjectPtr<cppobj>) b)
{
	return a.get() != b.get();
}

template <typename cppobj>
__inline bool operator != (ConstRef(CCppObjectPtr<cppobj>) a, std::nullptr_t b)
{
	return a.get() != nullptr;
}

template <typename cppobj>
__inline bool operator != (std::nullptr_t a, ConstRef(CCppObjectPtr<cppobj>) b)
{
	return nullptr != b.get();
}

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
class CCppObjectEqualFunctor
{
public:
	bool operator()(ConstPtr(T) r1, ConstPtr(T) r2) const
	{
		return r1 == r2;
	}
};

template <typename C, typename D>
sword __stdcall TCppObjectFindUserFunc(ConstPointer ArrayItem, ConstPointer DataItem, Pointer Context)
{
	ConstPtr(C) pCA = CastAnyConstPtr(C, ArrayItem);
	ConstPtr(C) pCD = CastAnyConstPtr(C, DataItem);
	Ptr(D) pD = CastAnyPtr(D, Context);

	if ((*pD)(pCA, pCD))
		return 0;
	return 1;
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

