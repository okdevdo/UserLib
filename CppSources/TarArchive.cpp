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
#include "TarArchive.h"

#include <stdio.h>

#ifdef _MSC_VER
#pragma warning( disable : 4996 )
#endif

class CPPSOURCES_API CTarArchiveFile: public CArchiveFile
{
public:
	CTarArchiveFile(Ref(CArchive) _archive, dword _fLength);
	virtual ~CTarArchiveFile();

	virtual void Read(Ref(CByteBuffer) _buffer);

protected:

private:
	CTarArchiveFile();
	CTarArchiveFile(ConstRef(CTarArchiveFile));
	Ref(CTarArchiveFile) operator=(ConstRef(CTarArchiveFile));
};

CTarArchiveFile::CTarArchiveFile(Ref(CArchive) _archive, dword _fLength):
	  CArchiveFile(_archive, _fLength)
{
}

CTarArchiveFile::~CTarArchiveFile()
{
}

void CTarArchiveFile::Read(Ref(CByteBuffer) _buffer)
{
	dword bufSize = _buffer.get_BufferSize();
	dword restLen = m_length - m_offset;
	dword want = Min(bufSize, restLen);
	
	_buffer.set_BufferSize(__FILE__LINE__ want);
	if ( PtrCheck(m_archive.GetFile()) )
	{
		m_offset += want;
		return;
	}
	if ( want )
	{
		m_archive.GetFile()->Read(_buffer);
		if ( _buffer.get_BufferSize() < want )
			throw OK_NEW_OPERATOR CArchiveException(__FILE__LINE__ 
				_T("CArchiveException in CTarArchiveFile::Read: Unexpected eof in %s"), 
				m_archive.GetFile()->GetPath().GetString());
		m_offset += want;

		dword roundup = (512 - (want % 512)) % 512;

		if ( (m_offset == m_length) && (roundup > 0) )
		{
			CByteBuffer throwaway(__FILE__LINE__ roundup);

			m_archive.GetFile()->Read(throwaway);
			if ( throwaway.get_BufferSize() < roundup )
				throw OK_NEW_OPERATOR CArchiveException(__FILE__LINE__ 
					_T("CArchiveException in CTarArchiveFile::Read: Unexpected eof in %s"), 
					m_archive.GetFile()->GetPath().GetString());
		}
	}
}

CTarArchiveIterator::CTarArchiveIterator(Ref(CTarArchive) _archive):
    CArchiveIterator(_archive), m_err(0), m_bHaveLongName(false), m_FileOffset(0), m_FileLength(0), m_Buffer(__FILE__LINE__ 512)
{
	if (sizeof (m_TarHeader) != 512)
		throw OK_NEW_OPERATOR CArchiveException(__FILE__LINE__ _T("Compilation Error: sizeof (m_TarHeader) != 512"));
	m_FileName[0] = 0;
}

CTarArchiveIterator::~CTarArchiveIterator()
{
}

CArchiveIterator::archive_file_t CTarArchiveIterator::GetType() const
{
	switch (m_TarHeader.typeflag)
	{
	case 0:
	case '0':
	case '7':
		return ARCHIVE_FILE_REGULAR;
	case '1':
		return ARCHIVE_FILE_HARDLINK;
	case '2':
		return ARCHIVE_FILE_SYMLINK;
	case '5':
		return ARCHIVE_FILE_DIRECTORY;
	default:
		return ARCHIVE_FILE_INVALID;
	}
}

CStringBuffer CTarArchiveIterator::GetFileName() const
{
	CByteBuffer buf(__FILE__LINE__ CastAnyPtr(byte, CastMutablePtr(char, m_FileName)), MAX_PATH + 512);
	CStringBuffer tmp;

	tmp.convertFromByteBuffer(buf);
	return tmp;
}

CStringBuffer CTarArchiveIterator::GetLinkName() const
{
	CStringBuffer tmp;

	switch (m_TarHeader.typeflag)
	{
	case 0:
	case '0':
	case '7':
		break;
	case '1':
	case '2':
		{
			CByteBuffer buf(__FILE__LINE__ CastAnyPtr(byte, CastMutablePtr(char, m_TarHeader.linkname)), 100);

			tmp.convertFromByteBuffer(buf);
		}
		break;
	case '5':
	default:
		break;
	}
	return tmp;
}

Ptr(CArchiveFile) CTarArchiveIterator::GetFile()
{
	return OK_NEW_OPERATOR CTarArchiveFile(m_archive, m_FileLength);
}

bool CTarArchiveIterator::ReadHeader()
{
	if ( PtrCheck(m_archive.GetFile()) )
		return false;
	m_Buffer.set_BufferSize(__FILE__LINE__ 512);
	m_archive.GetFile()->Read(m_Buffer);

	if (m_Buffer.get_BufferSize() != 512)
		return false;

	BPointer bp = m_Buffer.get_Buffer();
	bool bEnd = true;

	for ( int i = 0; i < 512; ++i )
		if ( *bp++ != 0 )
		{
			bEnd = false;
			break;
		}
	if ( bEnd )
		return false;
	s_memcpy_s(&m_TarHeader, 512, m_Buffer.get_Buffer(), 512);

	sscanf(m_TarHeader.size, "%lo", &m_FileLength);
	m_FileOffset = 0;

	ClearProperties();

	dword tmp;

	sscanf (m_TarHeader.mtime, "%lo", &tmp);
	SetProperty(_T("FILETIME"), tmp);

	sscanf (m_TarHeader.mode, "%lo", &tmp);
	SetProperty(_T("FILEMODE"), tmp);

	return true;
}

bool CTarArchiveIterator::Next()
{
	if ( !ReadHeader() )
		return false;

	if (!m_bHaveLongName && m_TarHeader.typeflag != 'L')
	{
		memcpy (m_FileName, m_TarHeader.name, 100);
		m_FileName[100] = 0;
	}
	else if (m_bHaveLongName)
		m_bHaveLongName = 0;

	switch (m_TarHeader.typeflag)
	{
	case 'L':			/* GNU tar long name extension */
		/* we read the 'file' into the long m_FileName, then call back into here
		* to find out if the actual file is a real file, or a special file..
		*/
		{
			if (m_FileLength > MAX_PATH)
			{
				Skip();
				//fprintf (stderr, "error: long file name exceeds %d characters\n",
				//	MAX_PATH);
				m_err++;
				if ( !ReadHeader() )
					return false;
				Skip();
				return Next();
			}

			char *c = m_FileName;

			while (m_FileLength > m_FileOffset)
			{
				int need = ((m_FileLength - m_FileOffset) > 512) ? 512 : (m_FileLength - m_FileOffset);

				m_Buffer.set_BufferSize(__FILE__LINE__ 512);
				m_archive.GetFile()->Read(m_Buffer);

				if (m_Buffer.get_BufferSize() != 512)
					return false;

				BPointer bp = m_Buffer.get_Buffer();

				s_memcpy_s (c, need, bp, need);
				c += need;
				m_FileOffset += need;
			}
			*c = 0;
			m_bHaveLongName = 1;
			return Next();
		}

	case '3':			/* char */
	case '4':			/* block */
	case '6':			/* fifo */
		//fprintf (stderr, "warning: not extracting special file %s\n",
		//	state.m_FileName);
		m_err++;
		return Next();

	case 0:			    /* regular file */
	case '0':			/* regular file also */
	case '1':			/* hard link */
	case '2':			/* symbolic link */
	case '5':			/* directory */
	case '7':			/* contiguous file */
		break;


	default:
		//fprintf (stderr, "error: unknown (or unsupported) file type `%c'\n",
		//	state.m_TarHeader.typeflag);
		m_err++;
		Skip();
		return Next();
	}
	return true;
}

void CTarArchiveIterator::Skip()
{
	if ( PtrCheck(m_archive.GetFile()) )
		return;
	while (m_FileLength > m_FileOffset)
	{
		m_archive.GetFile()->Read(m_Buffer);
		if ( m_Buffer.get_BufferSize() < 512 )
			throw OK_NEW_OPERATOR CArchiveException(__FILE__LINE__ _T("unexpected eof"));
		m_FileOffset += 512;
	}
	m_FileLength = 0;
	m_FileOffset = 0;
}

CTarArchive::CTarArchive(Ptr(CFile) _file):
    CArchive(_file)
{
}

CTarArchive::~CTarArchive(void)
{
}

Ptr(CArchiveIterator) CTarArchive::begin()
{
	return OK_NEW_OPERATOR CTarArchiveIterator(*this);
}
