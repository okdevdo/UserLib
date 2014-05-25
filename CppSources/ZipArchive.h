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
/**
 *  \file ZipArchive.h
 *  \brief Zip archive reader and writer.
 */
#pragma once

#include "CppSources.h"
#include "Archive.h"
#include "okDateTime.h"
#include "DataVector.h"

class CPPSOURCES_API CZipArchive;
class CPPSOURCES_API CZipArchiveFile: public CArchiveFile
{
public:
	CZipArchiveFile(Ref(CZipArchive) _archive, ConstRef(LSearchResultType) dataPtr);
	virtual ~CZipArchiveFile();

	virtual void Read(Ref(CByteBuffer) _buffer);

protected:
	Ref(CZipArchive) m_ziparchive;
	LSearchResultType m_dataPtr;

private:
	CZipArchiveFile();
	CZipArchiveFile(ConstRef(CZipArchiveFile));
	Ref(CZipArchiveFile) operator=(ConstRef(CZipArchiveFile));
};

class CPPSOURCES_API CZipArchiveIterator: public CArchiveIterator
{
public:
	CZipArchiveIterator(Ref(CZipArchive) _archive);
	virtual ~CZipArchiveIterator();

	virtual archive_file_t GetType() const;
	virtual CStringBuffer GetFileName() const;
	virtual Ptr(CArchiveFile) GetFile();

	virtual bool Next();
	virtual void Skip();

protected:
	Ref(CZipArchive) m_ziparchive;
	LSearchResultType m_dataPtr;

private:
	CZipArchiveIterator();
	CZipArchiveIterator(ConstRef(CZipArchiveIterator));
	Ref(CZipArchiveIterator) operator=(ConstRef(CZipArchiveIterator));
};

class CPPSOURCES_API CZipArchive: public CArchive
{
public:
	typedef struct _tagFileCacheItem
	{
		Pointer localFileHeader;
		Pointer fileHeader;
		CByteLinkedBuffer fileContent;

		void Reserve(CFile::TFileSize sz, CFile::TFileSize parts); 
	} TFileCacheItem;

	typedef CDataSVectorT<TFileCacheItem> TFileCacheItems;

public:
	CZipArchive(Ptr(CFile) _file);
	virtual ~CZipArchive(void);

	virtual Ptr(CArchiveIterator) begin();

	virtual void AddOpen();
	virtual void AddFile(ConstRef(CFilePath) fname, ConstRef(CArchiveProperties) props);
	virtual void AddDelete(ConstRef(CFilePath) fname);
	virtual void AddClose();

protected:
	CDataVector m_Disks;
	TFileCacheItems m_FileCache;
	bool m_opened;
	bool m_modified;

	friend class CZipArchiveIterator;
	friend class CZipArchiveFile;

private:
	CZipArchive();
	CZipArchive(ConstRef(CZipArchive));
	Ref(CZipArchive) operator=(ConstRef(CZipArchive));
};

