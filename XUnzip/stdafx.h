/******************************************************************************
    
	This file is part of XUnzip, which is part of UserLib.

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

#include "PDEFS.H"

#include "CppObject.h"
#include "StringLiteral.h"
#include "StringBuffer.h"
#include "ByteBuffer.h"
#include "DataVector.h"
#include "FilePath.h"

#include <iostream>

#ifdef _UNICODE
#define COUT std::wcout
#define CERR std::wcerr
#define OSTREAM std::wostream
#else
#define COUT std::cout
#define CERR std::cerr
#define OSTREAM std::ostream
#endif
using std::endl;

void XUnzipViewFiles(ConstRef(CFilePath) fzipfile, ConstRef(TMBCharList) filespecs, ConstRef(TMBCharList) excludespecs, CStringLiteral mode);
void XUnzipExtractFiles(ConstRef(CFilePath) fzipfile, WBool usefolders, WBool overwrite, ConstRef(TMBCharList) filespecs, ConstRef(TMBCharList) excludespecs);
void XUnzipFreshenFiles(ConstRef(CFilePath) fzipfile, WBool usefolders, ConstRef(TMBCharList) filespecs, ConstRef(TMBCharList) excludespecs);
void XUnzipNewerFiles(ConstRef(CFilePath) fzipfile, WBool usefolders, ConstRef(TMBCharList) filespecs, ConstRef(TMBCharList) excludespecs);
