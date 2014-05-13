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
#include "File.h"
#include "DataHashtable.h"

class CPPSOURCES_API CArchive;
class CPPSOURCES_API CArchiveFile: public CFile
{
public:
	CArchiveFile(Ref(CArchive) _archive, dword _fLength);
	virtual ~CArchiveFile();

	virtual TFileSize GetSize();
	virtual void SetSize(TFileSize);
	virtual TFileOffset GetFilePos();
	virtual void SetFilePos(TFileOffset);

	virtual void Read(Ref(CByteBuffer) _buffer);
	virtual void Read(Ref(CByteLinkedBuffer) _buffer);
	virtual void Write(ConstRef(CByteBuffer) _buffer);
	virtual void Write(ConstRef(CByteLinkedBuffer) _buffer);

protected:
	Ref(CArchive) m_archive;
	dword m_length;
	dword m_offset;

private:
	CArchiveFile();
	CArchiveFile(ConstRef(CArchiveFile));
	Ref(CArchiveFile) operator=(ConstRef(CArchiveFile));
};

class CPPSOURCES_API CArchiveProperty
{
public:
	CArchiveProperty(): m_name(), m_value(0), m_isNull(true) {}
	CArchiveProperty(CStringLiteral name, sqword value): m_name(name), m_value(value), m_isNull(false) {}
	CArchiveProperty(ConstRef(CArchiveProperty) copy): m_name(copy.m_name), m_value(copy.m_value), m_isNull(copy.m_isNull) {}

	__inline Ref(CArchiveProperty) operator = (ConstRef(CArchiveProperty) copy) { m_name = copy.m_name; m_value = copy.m_value; m_isNull = copy.m_isNull; return *this; }

	__inline CStringLiteral key() const { return m_name; }
	__inline sqword value() const { return m_value; }
	__inline bool isNull() const { return m_isNull; }

private:
	CStringLiteral m_name;
	sqword m_value;
	bool m_isNull;
};

class CPPSOURCES_API CArchiveProperties
{
public:
	CArchiveProperties(dword maxEntries = 64);

	void GetProperty(CStringLiteral name, Ref(sqword) value, Ref(bool) isNull) const;
	void SetProperty(CStringLiteral name, sqword value);
	void ClearProperties();

protected:
	CHashLinkedListT<CArchiveProperty, CStringLiteral, HashFunctorString> m_properties;

private:
	CArchiveProperties(ConstRef(CArchiveProperties));
	Ref(CArchiveProperties) operator = (ConstRef(CArchiveProperties) copy);
};

class CPPSOURCES_API CArchiveIterator: public CCppObject
{
public:
	typedef enum
	{
		ARCHIVE_FILE_INVALID,
		ARCHIVE_FILE_REGULAR,
		ARCHIVE_FILE_HARDLINK,
		ARCHIVE_FILE_SYMLINK,
		ARCHIVE_FILE_DIRECTORY
	}
	archive_file_t;

	CArchiveIterator(Ref(CArchive) _archive);
	virtual ~CArchiveIterator();

	virtual archive_file_t GetType() const = 0;
	virtual CStringBuffer GetFileName() const = 0;
	virtual CStringBuffer GetLinkName() const;
	virtual Ptr(CArchiveFile) GetFile() = 0;

	void GetProperty(CStringLiteral name, Ref(sqword) value, Ref(bool) isNull);
	void SetProperty(CStringLiteral name, sqword value);
	void ClearProperties();

	virtual bool Next() = 0;
	virtual void Skip() = 0;

protected:
	Ref(CArchive) m_archive;
	CArchiveProperties m_properties;

private:
	CArchiveIterator();
	CArchiveIterator(ConstRef(CArchiveIterator));
	Ref(CArchiveIterator) operator=(ConstRef(CArchiveIterator));
};

class CPPSOURCES_API CArchive: public CCppObject
{
public:
	CArchive(Ptr(CFile) _file);
	virtual ~CArchive();

	__inline Ptr(CFile) GetFile() { return m_file; }
	__inline void SetFile(Ptr(CFile) _file) { m_file = _file; }

	virtual Ptr(CArchiveIterator) begin() = 0;

	virtual void AddOpen();
	virtual void AddFile(ConstRef(CFilePath) fname, ConstRef(CArchiveProperties) props);
	virtual void AddDelete(ConstRef(CFilePath) fname);
	virtual void AddDirectory(ConstRef(CFilePath) fname, ConstRef(CArchiveProperties) props);
	virtual void AddLink(ConstRef(CFilePath) fname, ConstRef(CFilePath) lname, ConstRef(CArchiveProperties) props, bool bHardLink = false);
	virtual void AddClose();

protected:
	Ptr(CFile) m_file;

private:
	CArchive();
	CArchive(ConstRef(CArchive));
	Ref(CArchive) operator=(ConstRef(CArchive));
};

DECL_EXCEPTION(CPPSOURCES_API, CArchiveException, CBaseException)
