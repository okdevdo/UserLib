/******************************************************************************
    
	This file is part of CSources, which is part of UserLib.

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
#ifndef __PDEFTYPE_H__
#define __PDEFTYPE_H__

/* definitions of toolbox types */
#undef TRUE
#define TRUE			1
#undef FALSE
#define FALSE			0

#if defined(__cplusplus)
    typedef unsigned char		byte;
    typedef char				sbyte;
#ifdef OK_SYS_WINDOWS
    typedef _TCHAR				mbchar;
#endif
#ifdef OK_SYS_UNIX
#ifdef _UNICODE
    typedef wchar_t				mbchar;
#else
    typedef char				mbchar;
#endif	
#endif
    typedef unsigned short		word;
    typedef short				sword;
#ifdef OK_SYS_WINDOWS
	typedef unsigned long		dword;
	typedef long				sdword;
	typedef unsigned long		TUnixULong;
	typedef long				TUnixLong;
#endif
#ifdef OK_SYS_UNIX
	typedef unsigned long		TUnixULong;
	typedef long				TUnixLong;
#ifdef OK_CPU_32BIT
    typedef unsigned long		dword;
    typedef long				sdword;
#endif
#ifdef OK_CPU_64BIT
    typedef unsigned int		dword;
    typedef int					sdword;
#endif
#endif
#ifdef OK_COMP_MSC
	typedef unsigned __int64	qword;
	typedef __int64				sqword;
#endif
#ifdef OK_COMP_GNUC
	typedef unsigned long long	qword;
	typedef long long			sqword;
#endif
#ifdef OK_CPU_64BIT
#ifdef OK_COMP_MSC
	typedef __int64 IntPointer;
	typedef unsigned __int64 UIntPointer;
	typedef __int64 LongPointer;
	typedef unsigned __int64 ULongPointer;
#endif
#ifdef OK_COMP_GNUC
	typedef long long IntPointer;
	typedef unsigned long long UIntPointer;
	typedef long long LongPointer;
	typedef unsigned long long ULongPointer;
#endif
#endif
#ifdef OK_CPU_32BIT
	typedef int IntPointer;
	typedef unsigned int UIntPointer;
	typedef long LongPointer;
	typedef unsigned long ULongPointer;
#endif
	typedef void *				Pointer;
    typedef const void *		ConstPointer;
    typedef void **				Handle;
    typedef void **				Array;
    typedef byte *				BPointer;
    typedef const byte *		BConstPointer;
    typedef byte **				BArray;
    typedef sbyte *				SBPointer;
    typedef const sbyte *		SBConstPointer;
    typedef sbyte **			SBArray;
    typedef mbchar *			CPointer;
    typedef mbchar *			CString;
    typedef const mbchar *		CConstPointer;
    typedef mbchar **			CArray;
    typedef word *				WPointer;
    typedef const word *		WConstPointer;
    typedef word **				WArray;
    typedef sword *				SWPointer;
    typedef const sword *		SWConstPointer;
    typedef sword **			SWArray;
    typedef dword *				DWPointer;
    typedef const dword *		DWConstPointer;
    typedef dword **			DWArray;
    typedef sdword *			SDWPointer;
    typedef const sdword *		SDWConstPointer;
    typedef sdword **			SDWArray;
    typedef qword *				QWPointer;
    typedef const qword *		QWConstPointer;
    typedef qword **			QWArray;
    typedef sqword *			SQWPointer;
    typedef const sqword *		SQWConstPointer;
    typedef sqword **			SQWArray;

    typedef bool				WBool;
    typedef byte				WByte;
    typedef mbchar				WChar;
    typedef sword				WInt;
    typedef word				WUInt;
    typedef sdword				WLong;
    typedef dword				WULong;
    typedef float				WFloat;
    typedef double				WDouble;
#else
    #define bool			unsigned char
    #define true			1
    #define false			0
	
    #define byte			unsigned char
    #define sbyte			char
#ifdef OK_SYS_WINDOWS
    #define mbchar          _TCHAR
#endif
#ifdef OK_SYS_UNIX
#ifdef _UNICODE
	#define mbchar			wchar_t
#else
	#define mbchar			char
#endif	
#endif
    #define word			unsigned short
    #define sword			short
#ifdef OK_SYS_WINDOWS
	#define dword			unsigned long
	#define sdword			long
	#define TUnixULong		unsigned long
	#define TUnixLong		long
#endif
#ifdef OK_SYS_UNIX
	#define TUnixULong		unsigned long
	#define TUnixLong		long
#ifdef OK_CPU_32BIT
	#define dword			unsigned long
	#define sdword			long
#endif
#ifdef OK_CPU_64BIT
	#define dword			unsigned int
	#define sdword			int
#endif
#endif
#ifdef OK_COMP_MSC
	#define qword			unsigned __int64
	#define sqword			__int64
#endif
#ifdef OK_COMP_GNUC
	#define qword			unsigned long long
	#define sqword			long long
#endif
#ifdef OK_CPU_64BIT
#ifdef OK_COMP_MSC
	#define IntPointer      __int64
	#define UIntPointer     unsigned __int64
	#define LongPointer     __int64
	#define ULongPointer    unsigned __int64
#endif
#ifdef OK_COMP_GNUC
	#define IntPointer      long long
	#define UIntPointer     unsigned long long
	#define LongPointer     long long
	#define ULongPointer    unsigned long long
#endif
#endif
#ifdef OK_CPU_32BIT
	#define IntPointer      int
	#define UIntPointer     unsigned int
	#define LongPointer     long
	#define ULongPointer    unsigned long
#endif
    #define Pointer         void *
    #define ConstPointer    const void *
    #define Handle          void **
    #define Array			void **
    #define BPointer        byte *
    #define BConstPointer   const byte *
    #define BArray			byte **
    #define SBPointer       sbyte *
    #define SBConstPointer  const sbyte *
    #define SBArray			sbyte **
    #define CPointer        mbchar *
    #define CString         mbchar *
    #define CConstPointer   const mbchar *
    #define CArray			mbchar **
    #define WPointer        word *
    #define WConstPointer   const word *
    #define WArray			word **
    #define SWPointer       sword *
    #define SWConstPointer  const sword *
    #define SWArray			sword **
    #define DWPointer       dword *
    #define DWConstPointer  const dword *
    #define DWArray			dword **
    #define SDWPointer      sdword *
    #define SDWConstPointer const sdword *
    #define SDWArray		sdword **
    #define QWPointer       qword *
    #define QWConstPointer  const qword *
    #define QWArray			qword **
    #define SQWPointer      sqword *
    #define SQWConstPointer const sqword *
    #define SQWArray		sqword **

    #define WBool           bool
    #define WByte           byte
    #define WChar           mbchar
    #define WInt            sword
    #define WUInt           word
    #define WLong           sdword
    #define WULong          dword
    #define WFloat          float
    #define WDouble         double
#endif
#define szbool      sizeof(bool)
#define szchar      sizeof(mbchar)
#define szbyte      sizeof(byte)
#define szsbyte     sizeof(sbyte)
#define szword      sizeof(word)
#define szsword     sizeof(sword)
#define szdword     sizeof(dword)
#define szsdword    sizeof(sdword)
#define szqword     sizeof(qword)
#define szsqword    sizeof(sqword)
#define szPointer   sizeof(Pointer)

#define Ptr(type)               type *
#define Ref(type)               type&
#define RefRef(type)            type&&
#define ConstPtr(type)          const type *
#define ConstRef(type)          const type&
#define ConstRefRef(type)       const type&&
#define PtrPtr(type)			type **
#if defined(__cplusplus)
	// Integrals
    #define Cast(type,v)				(static_cast<type>(v))
    #define CastAny(type,v)				(reinterpret_cast<type>(v))
    #define CastMutable(type,v)			(const_cast<type>(v))
    #define CastStatic(type,p)			(static_cast<type>(p))
	// Pointers
    #define CastAnyPtr(type,p)			(reinterpret_cast<Ptr(type)>(p))
    #define CastAnyConstPtr(type,p)		(reinterpret_cast<ConstPtr(type)>(p))
    #define CastMutablePtr(type,p)		(const_cast<Ptr(type)>(p))
    #define CastStaticPtr(type,p)		(static_cast<Ptr(type)>(p))
    #define CastDynamicPtr(type,p)		(dynamic_cast<Ptr(type)>(p))
#else
	// Integrals
    #define Cast(type,v)				((type)(v))
    #define CastAny(type,v)				((type)(v))
    #define CastMutable(type,v)			((type)(v))
    #define CastStatic(type,v)			((type)(v))
	// Pointers
    #define CastAnyPtr(type,p)			((Ptr(type))(p))
    #define CastAnyConstPtr(type,p)		((ConstPtr(type))(p))
    #define CastMutablePtr(type,p)		((Ptr(type))(p))
    #define CastStaticPtr(type,p)		((Ptr(type))(p))
    #define CastDynamicPtr(type,p)		((Ptr(type))(p))
#endif
#define Castbyte(v)             Cast(byte,v)
#define Castsbyte(v)            Cast(sbyte,v)
#define Castchar(v)             Cast(mbchar,v)
#define Castword(v)             Cast(word,v)
#define Castsword(v)            Cast(sword,v)
#define Castdword(v)            Cast(dword,v)
#define Castsdword(v)           Cast(sdword,v)
#define Castqword(v)            Cast(qword,v)
#define Castsqword(v)           Cast(sqword,v)

#define CastPointer(p)          CastAnyPtr(void,(p))
#define CastConstPointer(p)     CastAnyConstPtr(void,(p))
#define CastBPointer(p)         CastAnyPtr(byte,(p))
#define CastBConstPointer(p)    CastAnyConstPtr(byte,(p))
#define CastCPointer(p)         CastAnyPtr(mbchar,(p))
#define CastCConstPointer(p)    CastAnyConstPtr(mbchar,(p))
#define CastWPointer(p)         CastAnyPtr(word,(p))
#define CastWConstPointer(p)    CastAnyConstPtr(word,(p))
#define CastSWPointer(p)        CastAnyPtr(sword,(p))
#define CastSWConstPointer(p)   CastAnyConstPtr(sword,(p))
#define CastDWPointer(p)        CastAnyPtr(dword,(p))
#define CastDWConstPointer(p)   CastAnyConstPtr(dword,(p))
#define CastSDWPointer(p)       CastAnyPtr(sdword,(p))
#define CastSDWConstPointer(p)  CastAnyConstPtr(sdword,(p))
#define CastQWPointer(p)        CastAnyPtr(qword,(p))
#define CastQWConstPointer(p)   CastAnyConstPtr(qword,(p))
#define CastSQWPointer(p)       CastAnyPtr(sqword,(p))
#define CastSQWConstPointer(p)  CastAnyConstPtr(sqword,(p))

/* definitions of toolbox shortcuts */
#ifdef __cplusplus
#define PtrCheck(p)             (nullptr == (p))
#define NotPtrCheck(p)          (nullptr != (p))
#else
#define PtrCheck(p)             (NULL==(p))
#define NotPtrCheck(p)          (NULL!=(p))
#endif
#define DerefPtr(type,p)				(*CastAnyPtr(type,p))
#define DerefAnyPtr(type,p)				(*CastAnyPtr(type,p))
#define DerefStaticPtr(type,p)			(*CastStaticPtr(type,p))
#define DerefDynamicPtr(type,p)			(*CastDynamicPtr(type,p))

#define DerefCPointer(p)				DerefPtr(mbchar,p)
#define DerefCString(p,ix)				(*(CastCPointer(p)+ix))
#define DerefSCPointer(p)				(*CastSCPointer(p))
#define DerefWPointer(p)				(*CastWPointer(p))
#define DerefSWPointer(p)				(*CastSWPointer(p))
#define DerefDWPointer(p)				(*CastDWPointer(p))
#define DerefSDWPointer(p)				(*CastSDWPointer(p))
#define DerefQWPointer(p)				(*CastQWPointer(p))
#define DerefSQWPointer(p)				(*CastSQWPointer(p))

#define StrEmpty(p)             ( PtrCheck(p) || (0 == DerefCPointer(p)) )
#define NotStrEmpty(p)          (!(StrEmpty(p)))
#define ConstStrEmpty(p)        ( PtrCheck(p) || (0 == DerefCPointer(CastMutablePtr(mbchar, p))) )
#define NotConstStrEmpty(p)     (!(ConstStrEmpty(p)))

#define _Lownibble(x)           ((x) & 0x0F)
#define _Highnibble(x)          _Lownibble((x) >> 4)
#define _Lowbyte(x)             ((x) & 0x00FF)
#define _Highbyte(x)            _Lowbyte((x) >> 8)
#define SET_LOWBYTE(a,b)        a = ((((word)(a)) & 0xFF00) | ((word)(_Lowbyte(b))))
#define SET_HIGHBYTE(a,b)       a = ((((word)_Lowbyte(b)) << 8) | (((word)(a)) & 0x00FF))
#define _Word(a,b)             ((word)(((word)(((word)_Lowbyte(b)) << 8)) | ((word)_Lowbyte(a))))
#define _Lowword(x)             ((x) & 0xFFFF)
#define _Highword(x)            _Lowword((x) >> 16)
#define SET_LOWWORD(a,b)        a = ((((dword)(a)) & 0xFFFF0000) | ((dword)(_Lowword(b))))
#define SET_HIGHWORD(a,b)       a = ((((dword)_Lowword(b)) << 16) | ((dword)(_Lowword(a))))
#define _DWord(a,b)             ((dword)(((dword)(((dword)_Lowword(b)) << 16)) | ((dword)_Lowword(a))))
#define _Lowdword(x)            ((x) & 0xFFFFFFFF)
#define _Highdword(x)           _Lowdword((x) >> 32)
#define SET_LOWDWORD(a,b)       a = ((((qword)(a)) & 0xFFFFFFFF00000000) | ((qword)(_Lowdword(b))))
#define SET_HIGHDWORD(a,b)      a = ((((qword)_Lowdword(b)) << 32) | ((qword)(_Lowdword(a))))
#define _QWord(a,b)             ((qword)(((qword)(((qword)_Lowdword(b)) << 32)) | ((qword)_Lowdword(a))))

// Logical operators
#define BitNot(b)           (~(b))

#define SetBitN(a,x)        ((a) | (1 << (x)))
#define SetBit(a,x)         ((a) | (x))
#define GetBitN(a,x)        ((a) & (1 << (x)))
#define GetBit(a,x)         ((a) & (x))
#define DelBitN(a,x)        ((a) & BitNot(1 << (x)))
#define DelBit(a,x)         ((a) & BitNot(x))
#define FlipBitN(a,x)       (DelBitN(a,x) | BitNot(GetBitN(a,x)))
#define FlipBit(a,x)        (DelBit(a,x) | BitNot(GetBit(a,x)))

#define TestBit(a,x)        ((x) == ((a) & (x)))
#define TestBitN(a,x)       ((1 << (x)) == ((a) & (1 << (x))))
#define Test(a,x)           (0 != ((a) & (x)))

#define BitOr(b1,b2)        SetBit((b1),(b2))
#define BitAnd(b1,b2)       GetBit((b1),(b2))
#define BitXor(b1,b2)       BitAnd(BitOr((b1),BitNot((b2))),BitOr(BitNot((b1)),(b2)))

#define BITNOT(v)           (v) = BitNot(v)

#define SETBIT(v,bim)       (v) |= (bim)
#define SETBITN(v,bitn)     (v) |= (1<<(bitn))
#define GETBIT(v,bim)       (v) &= (bim)
#define GETBITN(v,bitn)     (v) &= (1<<(bitn))
#define DELBIT(v,bim)       (v) &= BitNot(bim)
#define DELBITN(v,bitn)     (v) &= BitNot(1<<(bitn))
#define FLIPBIT(v,bim)      (v) = FlipBit((v),(bim))
#define FLIPBITN(v,bitn)    (v) = FlipBitN((v),(bitn))

#define BITOR(v,bim)        SETBIT((v),(bim))
#define BITAND(v,bim)       GETBIT((v),(bim))
#define BITXOR(v,bim)       (v) = BitXor((v),(bim))

#include <limits.h>
#define CALC_BITS(type)		(sizeof(type) * CHAR_BIT)
#define _mskGENERIC(size)   ((((1UL << ((size)-1U))-1UL) << 1U)+1UL)
#define mskGENERIC(type)    Cast(type,_mskGENERIC(CALC_BITS(type)))
#define mskBYTE             mskGENERIC(byte)
#define mskWORD             mskGENERIC(word)
#define mskDWORD            mskGENERIC(dword)
#define mskQWORD            mskGENERIC(qword)
#define mskSIZE_T           mskGENERIC(size_t)
#define mskPOINTER          mskGENERIC(Pointer)

#define CALC_MEMSIZE(n,c)	(((n) % (c))?(((n) + (c)) / (c)):((n) / (c)))
#define CALC_MEMTYPE(n,type)	(((n) % CALC_BITS(type))?(((n) + CALC_BITS(type)) / CALC_BITS(type)):((n) / CALC_BITS(type)))

/* definitions of toolbox common macros */
#define Min(a,b)                (((a)<(b))?(a):(b))
#define Max(a,b)                (((a)>(b))?(a):(b))

/* end of pdeftype.h */
#endif
