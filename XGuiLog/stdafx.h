/******************************************************************************
    
	This file is part of XGuiLog, which is part of UserLib.

    Copyright (C) 1995-2014  Oliver Kreis (okdev10@arcor.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by 
	the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/
// stdafx.h : Includedatei für Standardsystem-Includedateien
// oder häufig verwendete projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.
//

#pragma once

#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN             // Selten verwendete Teile der Windows-Header nicht einbinden.
#define NOMCX
#define NOPROFILER
#define NOKANJI
#define NOCOMM
#define NOWH
#define NOSOUND
#define NOSERVICE
#define NONLS
#define OEMRESOURCE
#define NORASTEROPS
#define _WINSOCKAPI_
// Windows-Headerdateien:
#include <windows.h>
#include <windowsx.h>

#include <math.h>
#include <gdiplus.h>

inline void Convert2RectF(Gdiplus::RectF* pRectf, RECT* pRect)
{
	pRectf->X = (Gdiplus::REAL)(pRect->left);
	pRectf->Y = (Gdiplus::REAL)(pRect->top);
	pRectf->Width = (Gdiplus::REAL)(pRect->right - pRect->left);
	pRectf->Height = (Gdiplus::REAL)(pRect->bottom - pRect->top);
}

inline void Convert2Rect(RECT* pRect, Gdiplus::RectF* pRectf)
{
	pRect->left = (LONG)(floor(pRectf->X + 0.5));
	pRect->top = (LONG)(floor(pRectf->Y + 0.5));
	pRect->right = (LONG)(floor(pRectf->X + pRectf->Width + 0.5));
	pRect->bottom = (LONG)(floor(pRectf->Y + pRectf->Height + 0.5));
}

#include "pdefs.h"

#include "CppObject.h"
#include "StringLiteral.h"
#include "StringBuffer.h"
#include "ByteBuffer.h"
