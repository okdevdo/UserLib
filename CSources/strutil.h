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
#ifndef __STRUTIL_H__
#define __STRUTIL_H__

#include "csources.h"
#include <ctype.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/*						 */
/* rtl memory operations */
/*						 */
#ifndef OK_COMP_MSC
#define __Min(a, b) (((a)<(b))?(a):(b))
#endif
// funcs *_s return errno_t. Zero on success.
// memmove, memset return p1.
// memcmp returns int. < 0: p1 < p2, == 0: p1 == p2, > 0: p1 > p2.
// memchr returns ptr to found char
#define s_memcpy(p1,p2,sz)                      memmove(CastPointer(p1), CastConstPointer(p2), Cast(size_t,(sz)))
#ifdef OK_COMP_MSC
#define s_memcpy_s(p1,sz1,p2,sz2)               memmove_s(CastPointer(p1), Cast(size_t,(sz1)), CastConstPointer(p2), Cast(size_t,(sz2)))
#else
#define s_memcpy_s(p1,sz1,p2,sz2)               memmove(CastPointer(p1), CastConstPointer(p2), __Min(Cast(size_t,(sz1)), Cast(size_t,(sz2))))
#endif
#define s_memmove(p1,p2,sz)						memmove(CastPointer(p1), CastConstPointer(p2), Cast(size_t,(sz)))
#ifdef OK_COMP_MSC
#define s_memmove_s(p1,sz1,p2,sz2)              memmove_s(CastPointer(p1), Cast(size_t,(sz1)), CastConstPointer(p2), Cast(size_t,(sz2)))
#else
#define s_memmove_s(p1,sz1,p2,sz2)              memmove(CastPointer(p1), CastConstPointer(p2), __Min(Cast(size_t,(sz1)), Cast(size_t,(sz2))))
#endif
#define s_memset(p,val,sz)						memset(CastPointer(p), Cast(int,(val)), Cast(size_t,(sz)))
#define s_memcmp(p1,p2,sz)						memcmp(CastConstPointer(p1), CastConstPointer(p2), Cast(size_t,(sz)))
#define s_memchr(p1,ch,sz)						memchr(CastConstPointer(p1), Cast(int,(ch)), Cast(size_t,(sz)))

/*						 */
/* rtl string operations */
/*						 */

// funcs *_s return errno_t. Zero on success.
// *cmp return int. < 0: p1 < p2, == 0: p1 == p2, > 0: p1 > p2.
// strlen returns size_t.
// strchr, strrchr, strpbrk, strstr, strtokf, strtokn return CPointer.
// strspn returns size_t.
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_UNIX
#define s_strcpy(p1,sz,p2)			strcpy(CastCPointer(p1), CastCConstPointer(p2))
#define s_strncpy(p1,sz1,p2,sz2)    strncpy(CastCPointer(p1), CastCConstPointer(p2), __Min(Cast(size_t,(sz1)), Cast(size_t,(sz2))))
#define s_strcat(p1,sz,p2)			strcat(CastCPointer(p1), CastCConstPointer(p2))
#endif
#ifdef OK_SYS_WINDOWS
#define s_strcpy(p1,sz,p2)			_tcscpy(CastCPointer(p1), CastCConstPointer(p2))
#define s_strncpy(p1,sz1,p2,sz2)    _tcsncpy(CastCPointer(p1), CastCConstPointer(p2), __Min(Cast(size_t,(sz1)), Cast(size_t,(sz2))))
#define s_strcat(p1,sz,p2)			_tcscat(CastCPointer(p1), CastCConstPointer(p2))
#endif
#endif
#ifdef OK_COMP_MSC
#define s_strcpy(p1,sz,p2)			_tcscpy_s(CastCPointer(p1), Cast(size_t,(sz)), CastCConstPointer(p2))
#define s_strncpy(p1,sz1,p2,sz2)	_tcsncpy_s(CastCPointer(p1), Cast(size_t,(sz1)), CastCConstPointer(p2), Cast(size_t,(sz2)))
#define s_strcat(p1,sz,p2)			_tcscat_s(CastCPointer(p1), Cast(size_t,(sz)), CastCConstPointer(p2))
#endif
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_UNIX
#define s_strcmp(p1,p2)				strcmp(CastCConstPointer(p1), CastCConstPointer(p2))
#define s_strncmp(p1,p2,sz)			strncmp(CastCConstPointer(p1), CastCConstPointer(p2), Cast(size_t,(sz)))
#define s_stricmp(p1,p2)			stricmp(CastCConstPointer(p1), CastCConstPointer(p2))
#define s_strnicmp(p1,p2,sz)		strnicmp(CastCConstPointer(p1), CastCConstPointer(p2), Cast(size_t,(sz)))
#endif
#ifdef OK_SYS_WINDOWS
#define s_strcmp(p1,p2)				_tcscmp(CastCConstPointer(p1), CastCConstPointer(p2))
#define s_strncmp(p1,p2,sz)			_tcsncmp(CastCConstPointer(p1), CastCConstPointer(p2), Cast(size_t,(sz)))
#define s_stricmp(p1,p2)			_tcsicmp(CastCConstPointer(p1), CastCConstPointer(p2))
#define s_strnicmp(p1,p2,sz)		_tcsnicmp(CastCConstPointer(p1), CastCConstPointer(p2), Cast(size_t,(sz)))
#endif
#endif
#ifdef OK_COMP_MSC
#define s_strcmp(p1,p2)				_tcscmp(CastCConstPointer(p1), CastCConstPointer(p2))
#define s_strncmp(p1,p2,sz)			_tcsncmp(CastCConstPointer(p1), CastCConstPointer(p2), Cast(size_t,(sz)))
#define s_stricmp(p1,p2)			_tcsicmp(CastCConstPointer(p1), CastCConstPointer(p2))
#define s_strnicmp(p1,p2,sz)		_tcsnicmp(CastCConstPointer(p1), CastCConstPointer(p2), Cast(size_t,(sz)))
#endif
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_UNIX
#define s_strlen(p,sz)				strlen(CastCConstPointer(p))
#endif
#ifdef OK_SYS_WINDOWS
#define s_strlen(p,sz)				_tcslen(CastCConstPointer(p))
#endif
#endif
#ifdef OK_COMP_MSC
#define s_strlen(p,sz)				Castdword(_tcsnlen(CastCConstPointer(p), Cast(size_t,(sz))))
#define s_strtokf(p1,p2,context)	_tcstok_s(CastCPointer(p1), CastCPointer(p2), CastAny(CArray,context))
#define s_strtokn(p2,context)		_tcstok_s(NULL, CastCPointer(p2), CastAny(CArray,context))
#define s_strspn(p,pc)				_tcsspn(CastCPointer(p), CastCPointer(pc))
#endif
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_UNIX
#define s_strchr(p,c)				strchr(CastCConstPointer(p), Cast(int,c))
#define s_strrchr(p,c)				strrchr(CastCConstPointer(p), Cast(int,c))
#endif
#ifdef OK_SYS_WINDOWS
#define s_strchr(p,c)				_tcschr(CastCConstPointer(p), Cast(int,c))
#define s_strrchr(p,c)				_tcsrchr(CastCConstPointer(p), Cast(int,c))
#endif
#endif
#ifdef OK_COMP_MSC
#define s_strchr(p,c)				_tcschr(CastCConstPointer(p), Cast(int,c))
#define s_strrchr(p,c)				_tcsrchr(CastCConstPointer(p), Cast(int,c))
#endif

#ifdef OK_COMP_MSC
#if _MSC_VER <= 1700
long long wcstoll(const wchar_t *_Str, wchar_t **_EndPtr, int _Radix);
long long strtoll(const char *_Str, char **_EndPtr, int _Radix);
unsigned long long wcstoull(const wchar_t *_Str, wchar_t **_EndPtr, int _Radix);
unsigned long long strtoull(const char *_Str, char **_EndPtr, int _Radix);
#ifdef UNICODE
#define _tcstoll wcstoll
#define _tcstoull wcstoull
#else
#define _tcstoll strtoll
#define _tcstoull strtoull
#endif
#endif
#define s_strtold(n,e)				_tcstold((n), (e))
#define s_strtod(n,e)				_tcstod((n), (e))
#define s_strtof(n,e)				_tcstof((n), (e))
#define s_strtol(n,e,r)				_tcstol((n), (e), (r))
#define s_strtoul(n,e,r)			_tcstoul((n), (e), (r))
#define s_strtoll(n,e,r)			_tcstoll((n), (e), (r))
#define s_strtoull(n,e,r)			_tcstoull((n), (e), (r))
#define s_atoi(n)					_ttoi((n))
#define s_ltoa(v,b,r)				_ltot((v), (b), (r))
#define s_lltoa(v,b,r)				_i64tot((v), (b), (r))
#endif
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_UNIX
#define s_strtold(n,e)				strtold((n), (e))
#define s_strtod(n,e)				strtod((n), (e))
#define s_strtof(n,e)				strtof((n), (e))
#define s_strtol(n,e,r)				strtol((n), (e), (r))
#define s_strtoul(n,e,r)			strtoul((n), (e), (r))
#define s_strtoll(n,e,r)			strtoll((n), (e), (r))
#define s_strtoull(n,e,r)			strtoull((n), (e), (r))
#define s_atoi(n)					atoi((n))
CSOURCES_API char *ltoa(long value, char *str, int radix);
#define s_ltoa(v,b,r)				ltoa((v), (b), (r))
CSOURCES_API char *lltoa(long long value, char *str, int radix);
#define s_lltoa(v,b,r)				lltoa((v), (b), (r))
#endif
#ifdef OK_SYS_WINDOWS32
#if __MINGW32_MAJOR_VERSION < 4
long long wcstoll(const wchar_t *_Str, wchar_t **_EndPtr, int _Radix);
long long strtoll(const char *_Str, char **_EndPtr, int _Radix);
unsigned long long wcstoull(const wchar_t *_Str, wchar_t **_EndPtr, int _Radix);
unsigned long long strtoull(const char *_Str, char **_EndPtr, int _Radix);
#ifdef UNICODE
#define s_strtof(n,e)				wcstof((n), (e))
#define s_strtod(n,e)				wcstod((n), (e))
#define s_strtold(n,e)				wcstold((n), (e))
#define s_strtol(n,e,r)				wcstol((n), (e), (r))
#define s_strtoul(n,e,r)			wcstoul((n), (e), (r))
#define s_strtoll(n,e,r)			wcstoll((n), (e), (r))
#define s_strtoull(n,e,r)			wcstoull((n), (e), (r))
#define s_ltoa(v,b,r)				_ltow((v), (b), (r))
#define s_lltoa(v,b,r)				_i64tow((v), (b), (r))
#define s_atoi(n)					_wtoi((n))
#else
#define s_strtof(n,e)				strtof((n), (e))
#define s_strtod(n,e)				strtod((n), (e))
#define s_strtold(n,e)				strtold((n), (e))
#define s_strtol(n,e,r)				strtol((n), (e), (r))
#define s_strtoul(n,e,r)			strtoul((n), (e), (r))
#define s_strtoll(n,e,r)			strtoll((n), (e), (r))
#define s_strtoull(n,e,r)			strtoull((n), (e), (r))
#define s_ltoa(v,b,r)				_ltoa((v), (b), (r))
#define s_lltoa(v,b,r)				_i64toa((v), (b), (r))
#define s_atoi(n)					atoi((n))
#endif
#else
#ifdef UNICODE
#define s_strtof(n,e)				wcstof((n), (e))
#define s_strtod(n,e)				wcstod((n), (e))
#define s_strtold(n,e)				wcstold((n), (e))
#define s_strtol(n,e,r)				wcstol((n), (e), (r))
#define s_strtoul(n,e,r)			wcstoul((n), (e), (r))
#define s_strtoll(n,e,r)			_wcstoi64((n), (e), (r))
#define s_strtoull(n,e,r)			_wcstoui64((n), (e), (r))
#define s_ltoa(v,b,r)				_ltow((v), (b), (r))
#define s_lltoa(v,b,r)				_i64tow((v), (b), (r))
#define s_atoi(n)					_wtoi((n))
#else
#define s_strtof(n,e)				strtof((n), (e))
#define s_strtod(n,e)				strtod((n), (e))
#define s_strtold(n,e)				strtold((n), (e))
#define s_strtol(n,e,r)				strtol((n), (e), (r))
#define s_strtoul(n,e,r)			strtoul((n), (e), (r))
#define s_strtoll(n,e,r)			strtoll((n), (e), (r))
#define s_strtoull(n,e,r)			strtoull((n), (e), (r))
#define s_ltoa(v,b,r)				_ltoa((v), (b), (r))
#define s_lltoa(v,b,r)				_i64toa((v), (b), (r))
#define s_atoi(n)					atoi((n))
#endif
#endif
#endif
#ifdef OK_SYS_WINDOWS64
#ifdef UNICODE
#define s_strtof(n,e)				wcstof((n), (e))
#define s_strtod(n,e)				wcstod((n), (e))
#define s_strtold(n,e)				wcstold((n), (e))
#define s_strtol(n,e,r)				wcstol((n), (e), (r))
#define s_strtoul(n,e,r)			wcstoul((n), (e), (r))
#define s_strtoll(n,e,r)			_wcstoi64((n), (e), (r))
#define s_strtoull(n,e,r)			_wcstoui64((n), (e), (r))
#define s_ltoa(v,b,r)				_ltow((v), (b), (r))
#define s_lltoa(v,b,r)				_i64tow((v), (b), (r))
#define s_atoi(n)					_wtoi((n))
#else
#define s_strtof(n,e)				strtof((n), (e))
#define s_strtod(n,e)				strtod((n), (e))
#define s_strtold(n,e)				strtold((n), (e))
#define s_strtol(n,e,r)				strtol((n), (e), (r))
#define s_strtoul(n,e,r)			strtoul((n), (e), (r))
#define s_strtoll(n,e,r)			strtoll((n), (e), (r))
#define s_strtoull(n,e,r)			strtoull((n), (e), (r))
#define s_ltoa(v,b,r)				_ltoa((v), (b), (r))
#define s_lltoa(v,b,r)				_i64toa((v), (b), (r))
#define s_atoi(n)					atoi((n))
#endif
#endif
#endif

CSOURCES_API void s_cvthexstr2byteptr(CConstPointer str, dword strlen, BPointer bptr);
CSOURCES_API void s_cvtbyteptr2hexstr(BConstPointer bptr, dword blen, CPointer str);

/*						 */
/* character test macros */
/*						 */

#if OK_SYS_WINDOWS
#define s_isascii(c)				_istascii(c)
#define s_isalnum(c)				_istalnum(c)
#define s_isalpha(c)				_istalpha(c)
#define s_isctrl(c)					_istcntrl(c)
#define s_iscsym(c)					(_istalnum(c) || ((c) == _T('_')))
#define s_iscsymf(c)				(_istalpha(c) || ((c) == _T('_')))
#define s_isdigit(c)				_istdigit(c)
#define s_isgraph(c)				_istgraph(c)
#define s_islower(c)				_istlower(c)
#define s_isprint(c)				_istprint(c)
#define s_ispunct(c)				_istpunct(c)
#define s_isspace(c)				_istspace(c)
#define s_isupper(c)				_istupper(c)
#endif
#ifdef OK_SYS_UNIX
#define s_isascii(c)				isascii(c)
#define s_isalnum(c)				isalnum(c)
#define s_isalpha(c)				isalpha(c)
#define s_isctrl(c)					iscntrl(c)
#define s_iscsym(c)					(isalnum(c) || ((c) == _T('_')))
#define s_iscsymf(c)				(isalpha(c) || ((c) == _T('_')))
#define s_isdigit(c)				isdigit(c)
#define s_isgraph(c)				isgraph(c)
#define s_islower(c)				islower(c)
#define s_isprint(c)				isprint(c)
#define s_ispunct(c)				ispunct(c)
#define s_isspace(c)				isspace(c)
#define s_isupper(c)				isupper(c)
#endif
CSOURCES_API bool	 s_iscword( mbchar c );
CSOURCES_API bool	 s_iscfword( mbchar c );
CSOURCES_API sword   s_strcnt( CConstPointer , mbchar );

/*               */
/* strpos series */
/*               */

// strpos returns 0, if not found, 1..n if found.
CSOURCES_API dword    s_strpos( CConstPointer , mbchar );
CSOURCES_API dword    s_strwpos( CConstPointer , mbchar );
CSOURCES_API dword    s_stripos( CConstPointer , mbchar );
CSOURCES_API dword    s_striwpos( CConstPointer , mbchar );

// strppos returns NULL, if not found, ptr to mbchar if found.
CSOURCES_API CPointer    s_strppos( CConstPointer , mbchar );
CSOURCES_API CPointer    s_strpwpos( CConstPointer , mbchar );
CSOURCES_API CPointer    s_strpipos( CConstPointer , mbchar );
CSOURCES_API CPointer    s_strpiwpos( CConstPointer , mbchar );

/*                       */
/* reverse strpos series */
/*                       */

// strrpos returns 0, if not found, 1..n if found.
CSOURCES_API dword    s_strrpos( CConstPointer , mbchar );
CSOURCES_API dword    s_strrwpos( CConstPointer , mbchar );
CSOURCES_API dword    s_strripos( CConstPointer , mbchar );
CSOURCES_API dword    s_strriwpos( CConstPointer , mbchar );

// strprpos returns NULL, if not found, ptr to mbchar if found.
CSOURCES_API CPointer    s_strprpos( CConstPointer , mbchar );
CSOURCES_API CPointer    s_strprwpos( CConstPointer , mbchar );
CSOURCES_API CPointer    s_strpripos( CConstPointer , mbchar );
CSOURCES_API CPointer    s_strpriwpos( CConstPointer , mbchar );

/*				  */
/* strpbrk series */
/*				  */

CSOURCES_API CPointer    s_strpbrk( CConstPointer , CConstPointer );
CSOURCES_API CPointer    s_strpnonbrk( CConstPointer , CConstPointer );
CSOURCES_API dword       s_strpospbrk( CConstPointer , CConstPointer );
CSOURCES_API CPointer    s_strptran( CPointer , CConstPointer , CConstPointer );
CSOURCES_API CPointer    s_stripbrk( CConstPointer , CConstPointer );
CSOURCES_API CPointer    s_stripnonbrk( CConstPointer , CConstPointer );
CSOURCES_API dword       s_stripospbrk( CConstPointer , CConstPointer );
CSOURCES_API CPointer    s_strwpbrk( CConstPointer , CConstPointer );
CSOURCES_API CPointer    s_striwpbrk( CConstPointer , CConstPointer );
CSOURCES_API CPointer    s_strpskip( CConstPointer , CConstPointer );

/*					  */
/* strposwhite series */
/*					  */

CSOURCES_API CPointer    s_strpwhite( CConstPointer );
CSOURCES_API dword       s_strposwhite( CConstPointer );
CSOURCES_API CPointer    s_strpnwhite( CConstPointer , dword );
CSOURCES_API dword       s_strnposwhite( CConstPointer , dword );
CSOURCES_API CPointer    s_strpnonwhite( CConstPointer );
CSOURCES_API CPointer    s_strpword( CConstPointer );
CSOURCES_API CPointer    s_strpnonword( CConstPointer );

/*					   */
/* strrposwhite series */
/*					   */

CSOURCES_API CPointer    s_strrnpwhite( CConstPointer , sdword );
CSOURCES_API dword       s_strrnposwhite( CConstPointer , sdword );
CSOURCES_API CPointer    s_strrnpnonwhite( CConstPointer , sdword );
CSOURCES_API dword       s_strrnposnonwhite( CConstPointer , sdword );

/*			     */
/* strstr series */
/*				 */

CSOURCES_API CPointer	 s_strstr( CConstPointer , CConstPointer );
CSOURCES_API CPointer    s_strnstr2( CConstPointer , CConstPointer , dword );
CSOURCES_API CPointer    s_strwstr2( CConstPointer , CConstPointer );
CSOURCES_API CPointer    s_strnistr2( CConstPointer , CConstPointer , dword );
CSOURCES_API CPointer    s_striwstr2( CConstPointer , CConstPointer );

/*			       */
/* strsplit series */
/*				   */
CSOURCES_API void s_strsplitany(CPointer p, CConstPointer delimiter, CArray output, dword maxoutputsize, dword* outputsize);
CSOURCES_API void s_strsplit(CPointer p, CConstPointer delimiter, CArray output, dword maxoutputsize, dword* outputsize);
CSOURCES_API void s_strsplitquoted(CPointer p, CConstPointer quote, CConstPointer delimiter, CArray output, dword maxoutputsize, dword* outputsize);

typedef void(__stdcall *TSplitFunc)(dword recno, dword fieldno, CConstPointer data, Pointer context);

CSOURCES_API void s_strsplitquoted2(CPointer p, CConstPointer recorddelimiter, CConstPointer fieldquote, CConstPointer fielddelimiter, TSplitFunc output, Pointer outputcontext);

/*		  	      */
/* strxcmp series */
/*				  */
CSOURCES_API bool s_strxcmp(CConstPointer , CConstPointer);

/*                              */
/* strpos implementation macros */
/*                              */
#ifdef OK_SYS_WINDOWS
#define m_toupper(c)						((s_isascii(c) && s_islower(c))?_totupper(c):(c))
#define m_tolower(c)						((s_isascii(c) && s_isupper(c))?_totlower(c):(c))
#endif
#ifdef OK_SYS_UNIX
#define m_toupper(c)						((s_isascii(c) && s_islower(c))?toupper(c):(c))
#define m_tolower(c)						((s_isascii(c) && s_isupper(c))?tolower(c):(c))
#endif
#define m_strdoloop(p_iterate,func)			while ( (0 != *p_iterate) && (func) ) ++p_iterate
#define m_stripos(p_iterate,ch)				m_strdoloop(p_iterate,Castdword(ch) != m_toupper(Castdword(*p_iterate)))
#define m_strpos(p_iterate,ch)				m_strdoloop(p_iterate,Castdword(ch) != Castdword(*p_iterate))
#define m_strfindwhite(p_iterate)			m_strdoloop(p_iterate,!s_isspace(*p_iterate))
#define m_streatwhite(p_iterate)			m_strdoloop(p_iterate,s_isspace(*p_iterate))
#define m_strwdoloop(p_iterate,func)		m_streatwhite(p_iterate); while ( (0 != *p_iterate) && (func) ) { m_strfindwhite(p_iterate); m_streatwhite(p_iterate); }
#define m_strwpos(p_iterate,ch)				m_strwdoloop(p_iterate,Castdword(ch) != Castdword(*p_iterate))
#define m_striwpos(p_iterate,ch)			m_strwdoloop(p_iterate,Castdword(ch) != m_toupper(Castdword(*p_iterate)))
#define m_strpbrk_template(s1,s2,p,cond)	while ( NotStrEmpty(s1) ) { p = s2; m_strpos((p),(*s1)); if ( cond ) break; s1++; }
#define m_strpbrk(s1,s2,p)					m_strpbrk_template(s1,s2,p,(0!=(*p)))
#define m_strpnonbrk(s1,s2,p)				m_strpbrk_template(s1,s2,p,(0==(*p)))

#ifdef __cplusplus
}
#endif


#endif
/* STRUTIL.H */

