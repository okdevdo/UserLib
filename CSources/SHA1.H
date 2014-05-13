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
/* public api for steve reid's public domain SHA-1 implementation */
/* this file is in the public domain */

#include "CSOURCES.H"

#ifndef __SHA1_H
#define __SHA1_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	dword state[5];
	dword count[2];
	byte  buffer[64];
} SHA1_CTX;

#define SHA1_DIGEST_SIZE 20

CSOURCES_API void sha1_init(SHA1_CTX* context);
CSOURCES_API void sha1_update(SHA1_CTX* context, const byte* data, const dword len);
CSOURCES_API void sha1_finish(SHA1_CTX* context, byte digest[SHA1_DIGEST_SIZE]);

#ifdef __cplusplus
}
#endif

#endif /* __SHA1_H */