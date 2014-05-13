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
#include "Archive.h"

IMPL_EXCEPTION(CArchiveException, CBaseException)

CArchiveFile::CArchiveFile(Ref(CArchive) _archive, dword _fLength):
	  CFile(), m_archive(_archive), m_length(_fLength), m_offset(0)
{
}

CArchiveFile::~CArchiveFile()
{
}

CFile::TFileSize CArchiveFile::GetSize()
{
	return m_length;
}

void CArchiveFile::SetSize(TFileSize _size)
{
	m_length = Cast(dword, _size);
}

CFile::TFileOffset CArchiveFile::GetFilePos()
{
	return m_offset;
}

void CArchiveFile::SetFilePos(TFileOffset _pos)
{
	m_offset = Cast(dword, _pos);
	if ( m_offset > m_length )
		m_offset = m_length;
}

void CArchiveFile::Read(Ref(CByteBuffer) _buffer)
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
			throw OK_NEW_OPERATOR CArchiveException(__FILE__LINE__ _T("unexpected eof"));
		m_offset += want;
	}
}

void CArchiveFile::Read(Ref(CByteLinkedBuffer) _buffer)
{
	throw OK_NEW_OPERATOR CArchiveException(__FILE__LINE__ _T("not implemented"));
}

void CArchiveFile::Write(ConstRef(CByteBuffer) _buffer)
{
	throw OK_NEW_OPERATOR CArchiveException(__FILE__LINE__ _T("not implemented"));
}

void CArchiveFile::Write(ConstRef(CByteLinkedBuffer) _buffer)
{
	throw OK_NEW_OPERATOR CArchiveException(__FILE__LINE__ _T("not implemented"));
}

CArchiveProperties::CArchiveProperties(dword maxEntries):
    m_properties(maxEntries * 5)
{
}

void CArchiveProperties::GetProperty(CStringLiteral name, Ref(sqword) value, Ref(bool) isNull) const
{
	CArchiveProperty prop(m_properties.search(name));

	value = prop.value();
	isNull = prop.isNull();
}

void CArchiveProperties::SetProperty(CStringLiteral name, sqword value)
{
	CArchiveProperty prop(name, value);

	m_properties.insert(prop);
}

void CArchiveProperties::ClearProperties()
{
	m_properties.clear();
}

CArchiveIterator::CArchiveIterator(Ref(CArchive) _archive):
    m_archive(_archive), m_properties(64)
{
}

CArchiveIterator::~CArchiveIterator()
{
}

CStringBuffer CArchiveIterator::GetLinkName() const
{
	CStringBuffer tmp;

	return tmp;
}

void CArchiveIterator::GetProperty(CStringLiteral name, Ref(sqword) value, Ref(bool) isNull)
{
	m_properties.GetProperty(name, value, isNull);
}

void CArchiveIterator::SetProperty(CStringLiteral name, sqword value)
{
	m_properties.SetProperty(name, value);
}

void CArchiveIterator::ClearProperties()
{
	m_properties.ClearProperties();
}

CArchive::CArchive(Ptr(CFile) _file):
    m_file(_file)
{
}

CArchive::~CArchive()
{
}

void CArchive::AddOpen()
{
}

void CArchive::AddFile(ConstRef(CFilePath) fname, ConstRef(CArchiveProperties) props)
{
}

void CArchive::AddDelete(ConstRef(CFilePath) fname)
{
}

void CArchive::AddDirectory(ConstRef(CFilePath) fname, ConstRef(CArchiveProperties) props)
{
}

void CArchive::AddLink(ConstRef(CFilePath) fname, ConstRef(CFilePath) lname, ConstRef(CArchiveProperties) props, bool bHardLink)
{
}

void CArchive::AddClose()
{
}
