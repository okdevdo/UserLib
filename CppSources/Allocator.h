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
 *  \file Allocator.h
 *  \brief Managed allocator for Standard template container.
 */
#pragma once

#include "CppSources.h"

//	Traits that describes an object T
template<typename T>
class ObjectTraits 
{
public : 
	//	convert an ObjectTraits<T> to ObjectTraits<U>
	template<typename U>
	struct rebind {
		typedef ObjectTraits<U> other;
	};

public : 
	inline explicit ObjectTraits() {}
	inline ~ObjectTraits() {}
	inline explicit ObjectTraits(ObjectTraits  const&) {}
	template <typename U>
	inline explicit ObjectTraits(ObjectTraits<U> const&) {}

	//	address
	inline T* address(T& r) { return &r; }
	inline T const* address(T const& r) { return &r; }

	inline static void construct(T* p, const T& t) { new(p) T(t); }
	inline static void destroy(T* p) { p->~T(); }
};	//	end of class ObjectTraits

//	a standard allocation policy using the free store
template<typename T>
class StandardAllocPolicy 
{
public : 
	//	typedefs
	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef qword dword_type;
	typedef qword size_type;
	typedef ptrdiff_t difference_type;

public : 
	//	convert an StandardAllocPolicy<T> to StandardAllocPolicy<U>
	template<typename U>
	struct rebind {
		typedef StandardAllocPolicy<U> other;
	};

public : 
	inline explicit StandardAllocPolicy() {}
	inline ~StandardAllocPolicy() {}
	inline explicit StandardAllocPolicy(StandardAllocPolicy const&) {}
	template <typename U>
	inline explicit StandardAllocPolicy(StandardAllocPolicy<U> const&) {}

	//	memory allocation
	inline pointer allocate(dword_type cnt, pointer = 0) 
	{ 
		return reinterpret_cast<pointer>(TFalloc(Castdword(cnt * sizeof (T)))); 
	}
	inline void deallocate(pointer p, dword_type) 
	{ 
		TFfree(p);
	}

	//	size
	inline dword_type max_size() const { return ((dword_type)UINT_MAX) / ((dword_type)sizeof(T)); }
};	//	end of class StandardAllocPolicy

//	determines if memory from another allocator can be deallocated from this one
template<typename T, typename T2>
inline bool operator==(StandardAllocPolicy<T> const&, StandardAllocPolicy<T2> const&) { 
	return true;
}
template<typename T, typename OtherAllocator>
inline bool operator==(StandardAllocPolicy<T> const&, OtherAllocator const&) { 
	return false; 
}

//	Policy driven allocator object
template<typename T, typename Policy = StandardAllocPolicy<T>, typename Traits = ObjectTraits<T> >
class Allocator : public Policy, public Traits {
private : 
	typedef Policy AllocationPolicy;
	typedef Traits TTraits;

public : 
	typedef typename AllocationPolicy::size_type size_type;
	typedef typename AllocationPolicy::dword_type dword_type;
	typedef typename AllocationPolicy::difference_type difference_type;
	typedef typename AllocationPolicy::pointer pointer;
	typedef typename AllocationPolicy::const_pointer const_pointer;
	typedef typename AllocationPolicy::reference reference;
	typedef typename AllocationPolicy::const_reference const_reference;
	typedef typename AllocationPolicy::value_type value_type;

public : 
//#ifdef _MSC_VER
	template<typename U>
	struct rebind {
//		typedef Allocator<U, typename AllocationPolicy::rebind<U>::other> other;
		typedef Allocator<U> other;
	};
//#endif

public : 
	inline explicit Allocator() {}
	inline ~Allocator() {}
	inline Allocator(Allocator const& rhs): Policy(rhs), Traits(rhs) {}
	template <typename U>
	inline explicit Allocator(Allocator<U> const&) {}
	template <typename U, typename P, typename T2>
	inline Allocator(Allocator<U, P, T2> const& rhs):Traits(rhs), Policy(rhs) {}

	//	memory allocation
	inline pointer allocate(dword_type cnt, pointer hint = 0) 
	{ 
		return AllocationPolicy::allocate(cnt, hint);
	}
	inline void deallocate(pointer p, dword_type cnt) 
	{
		AllocationPolicy::deallocate(p, cnt);
	}
};	//	end of class Allocator

//	determines if memory from another allocator can be deallocated from this one
template<typename T, typename P, typename Tr>
inline bool operator==(Allocator<T, P, Tr> const& lhs, Allocator<T, P, Tr> const& rhs) { 
	return operator==((P&)(lhs), (P&)(rhs)); 
}
