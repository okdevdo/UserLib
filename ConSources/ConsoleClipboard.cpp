/******************************************************************************
    
	This file is part of ConSources, which is part of UserLib.

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
#include "CON_PCH.H"
#include "ConsoleClipboard.h"

#ifdef OK_SYS_WINDOWS
#ifdef _UNICODE
#define _CLIPBOARDTEXTFORMAT CF_UNICODETEXT
#else
#define _CLIPBOARDTEXTFORMAT CF_TEXT
#endif
#endif
#ifdef OK_SYS_UNIX
CStringBuffer CConsoleClipboard::_clipboardtext;

void CConsoleClipboard::FreeInstance()
{
	_clipboardtext.Clear();
}
#endif

CConsoleClipboard::CConsoleClipboard(void)
{
}


CConsoleClipboard::~CConsoleClipboard(void)
{
}

CStringBuffer CConsoleClipboard::GetClipboardText()
{
	CStringBuffer tmp;

#ifdef OK_SYS_WINDOWS
	if (!IsClipboardFormatAvailable(_CLIPBOARDTEXTFORMAT))
		return tmp;

	HWND hwnd = GetConsoleWindow();

	if ( !hwnd )
		return tmp;
    if (!OpenClipboard(hwnd)) 
		return tmp;
 
    HGLOBAL hglb = GetClipboardData(_CLIPBOARDTEXTFORMAT); 
    if (hglb != nullptr) 
    { 
        LPTSTR  lptstr = (LPTSTR)GlobalLock(hglb); 
        if (lptstr != nullptr) 
        {
			dword tlen = s_strlen(lptstr, INT_MAX);
			tmp.SetSize(__FILE__LINE__ tlen + 1);
			s_strcpy(CastMutable(CPointer, tmp.GetString()), tlen + 1, lptstr);
            GlobalUnlock(hglb); 
        } 
    } 
    CloseClipboard();
#endif
#ifdef OK_SYS_UNIX
	tmp = _clipboardtext;
#endif
	return tmp;
 }

bool CConsoleClipboard::SetClipboardText(ConstRef(CStringBuffer) text)
{
#ifdef OK_SYS_WINDOWS
	HWND hwnd = GetConsoleWindow();
	HGLOBAL hglbCopy;
	LPTSTR  lptstrCopy;

	if ( !hwnd )
		return false;
	if ( !OpenClipboard(hwnd) ) 
		return false;
	EmptyClipboard();
	hglbCopy = GlobalAlloc(GMEM_MOVEABLE, text.GetSize() * sizeof(TCHAR));
	if ( !hglbCopy )
	{
		CloseClipboard();
		return false;
	}
    lptstrCopy = (LPTSTR)GlobalLock(hglbCopy);
	if ( !lptstrCopy )
	{
		CloseClipboard();
		return false;
	}
	s_memcpy(lptstrCopy, CastMutable(CPointer, text.GetString()), text.GetSize() * sizeof(TCHAR)); 
    GlobalUnlock(hglbCopy);
	SetClipboardData(_CLIPBOARDTEXTFORMAT, hglbCopy);
	CloseClipboard();
#endif
#ifdef OK_SYS_UNIX
	_clipboardtext = text;
#endif
	return true;
}

