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
#pragma once

#include "CppSources.h"
#include "Archive.h"

#ifdef OK_SYS_WINDOWS
#ifndef MAX_PATH
#define MAX_PATH _MAX_PATH
#endif
#endif
#ifdef OK_SYS_UNIX
#define MAX_PATH 260
#endif

typedef struct
{
  char name[100];		/*   0 */
  char mode[8];			/* 100 */
  char uid[8];			/* 108 */
  char gid[8];			/* 116 */
  char size[12];		/* 124 */
  char mtime[12];		/* 136 */
  char chksum[8];		/* 148 */
  char typeflag;		/* 156 */
  char linkname[100];	/* 157 */
  char magic[6];		/* 257 */
  char version[2];		/* 263 */
  char uname[32];		/* 265 */
  char gname[32];		/* 297 */
  char devmajor[8];		/* 329 */
  char devminor[8];		/* 337 */
  char prefix[155];		/* 345 */
  char junk[12];		/* 500 */
}
tar_header_type;

class CPPSOURCES_API CTarArchive;
class CPPSOURCES_API CTarArchiveIterator: public CArchiveIterator
{
public:
	CTarArchiveIterator(Ref(CTarArchive) _archive);
	virtual ~CTarArchiveIterator();

	virtual archive_file_t GetType() const;
	virtual CStringBuffer GetFileName() const;
	virtual CStringBuffer GetLinkName() const;
	virtual Ptr(CArchiveFile) GetFile();

	virtual bool Next();
	virtual void Skip();

protected:
	dword m_err;
	bool m_bHaveLongName;
	dword m_FileOffset;
	dword m_FileLength;
	tar_header_type m_TarHeader;
	char m_FileName[MAX_PATH + 512];
	CByteBuffer m_Buffer;

	bool ReadHeader();

private:
	CTarArchiveIterator();
	CTarArchiveIterator(ConstRef(CTarArchiveIterator));
	Ref(CTarArchiveIterator) operator=(ConstRef(CTarArchiveIterator));
};

class CPPSOURCES_API CTarArchive: public CArchive
{
public:
	CTarArchive(Ptr(CFile) _file);
	virtual ~CTarArchive(void);

	virtual Ptr(CArchiveIterator) begin();

private:
	CTarArchive();
	CTarArchive(ConstRef(CTarArchive));
	Ref(CTarArchive) operator=(ConstRef(CTarArchive));
};

