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
#include "CRC32Value.h"

#define                 P_32        0xEDB88320L

static bool _crc32_tab_init = false;
static dword _crc32_tab[256];

static void _init_crc32_tab( void ) 
{
	if ( _crc32_tab_init )
		return;

    word i, j;
    dword crc;

    for (i=0; i<256; i++) {

        crc = Cast(dword,i);

        for (j=0; j<8; j++) {

            if ( crc & 0x00000001L ) crc = ( crc >> 1 ) ^ P_32;
            else                     crc =   crc >> 1;
        }

        _crc32_tab[i] = crc;
    }
	_crc32_tab_init = true;
}

static dword _update_crc32( dword crc, byte c ) 
{

    dword tmp, long_c;

    long_c = 0x000000ffL & Cast(dword,c);

    tmp = crc ^ long_c;
    crc = (crc >> 8) ^ _crc32_tab[ tmp & 0xff ];

    return crc;

}

CCRC32Value::CCRC32Value(void):
    _value(0xffffffffL)
{
}

CCRC32Value::~CCRC32Value(void)
{
}

void CCRC32Value::Init()
{
	_init_crc32_tab();
	_value = 0xffffffffL;
}

void CCRC32Value::Append(ConstRef(CByteBuffer) buf)
{
	BPointer b = buf.get_Buffer();
	dword cnt = buf.get_BufferSize();

	for ( dword ix = 0; ix < cnt; ++ix )
		_value = _update_crc32(_value, *b++);
}

void CCRC32Value::Finish()
{
	_value ^= 0xffffffffL;
}
