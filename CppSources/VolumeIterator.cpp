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
#include "VolumeIterator.h"

#ifdef OK_SYS_WINDOWS
#include <direct.h>
#endif

CVolumeIterator::CVolumeIterator(void):
#ifdef OK_SYS_WINDOWS
	_driveno(0), _curdriveno(-1)
#endif
#ifdef OK_SYS_UNIX
	_bAtEnd(false)
#endif
{
    Begin();
}

CVolumeIterator::~CVolumeIterator(void)
{
    Close();
}

void CVolumeIterator::Begin()
{
#ifdef OK_SYS_WINDOWS
	_driveno = _getdrives();
	_curdriveno = 0;
	_next();
#endif
#ifdef OK_SYS_UNIX
	_bAtEnd = false;
#endif
}

void CVolumeIterator::Next()
{
#ifdef OK_SYS_WINDOWS
	++_curdriveno;
	_next();
#endif
#ifdef OK_SYS_UNIX
	_bAtEnd = true;
#endif
}

void CVolumeIterator::Close()
{
#ifdef OK_SYS_WINDOWS
	_curdriveno = -1;
#endif
#ifdef OK_SYS_UNIX
	_bAtEnd = true;
#endif
}

CFilePath CVolumeIterator::get_Path()
{
	CFilePath result;
#ifdef OK_SYS_WINDOWS
	mbchar buf[4];

	buf[0] = _T('A') + _curdriveno;
	buf[1] = _T(':');
	buf[2] = _T('\\');
	buf[3] = 0;
#endif
#ifdef OK_SYS_UNIX
	mbchar buf[2];

	buf[0] = _T('/');
	buf[1] = 0;
#endif	
	result.set_Root(buf);

	return result;
}

#ifdef OK_SYS_WINDOWS
void CVolumeIterator::_next()
{
	while ( ((_driveno & (1 << _curdriveno)) == 0) && (_curdriveno < 26) )
		++_curdriveno;
	if ( _curdriveno == 26 )
		Close();
}
#endif