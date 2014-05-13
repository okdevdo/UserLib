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
#ifndef __TFMEM_H__
#define __TFMEM_H__

#include "CSOURCES.H"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __DEBUG__
CSOURCES_API void* TFallocDbg(size_t sz, const char* file, int line);
#define TFalloc(sz) TFallocDbg((sz), __FILE__, __LINE__)
CSOURCES_API void* TFreallocDbg(void* p, size_t newsz, const char* file, int line);
#define TFrealloc(p,sz) TFreallocDbg((p), (sz), __FILE__, __LINE__)
#else
CSOURCES_API void* TFalloc(size_t sz);
CSOURCES_API void* TFrealloc(void* p, size_t newsz);
#endif
CSOURCES_API void TFfree(void* p);
CSOURCES_API bool TFisalloc(void* p);
CSOURCES_API size_t TFsize(void* p);
CSOURCES_API dword TFrefcnt(void* p);
CSOURCES_API dword TFincrefcnt(void* p);
CSOURCES_API dword TFdecrefcnt(void* p);

#ifdef __cplusplus
}
#endif

#endif
