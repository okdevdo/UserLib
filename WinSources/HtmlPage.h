/******************************************************************************
    
	This file is part of WinSources, which is part of UserLib.

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

#include "DataVector.h"
#include "FilePath.h"
#include "ByteLinkedBuffer.h"

#include "WinSources.h"

class WINSOURCES_API CHtmlPage: public CCppObject
{
public:
	class WINSOURCES_API ReplacePointer: public CCppObject
	{
	public:
		CByteLinkedBuffer::Iterator _begin;
		CByteLinkedBuffer::Iterator _end;
		CFilePath _filePath;

		ReplacePointer(ConstRef(CByteLinkedBuffer::Iterator) begin, ConstRef(CByteLinkedBuffer::Iterator) end, ConstRef(CFilePath) path);
		virtual ~ReplacePointer();
	};

	typedef CDataVectorT<ReplacePointer> ReplacePointers;

	class WINSOURCES_API ContentPointer: public CCppObject
	{
	public:
		CByteLinkedBuffer::Iterator _begin;
		CByteLinkedBuffer::Iterator _end;

		ContentPointer(ConstRef(CByteLinkedBuffer::Iterator) begin, ConstRef(CByteLinkedBuffer::Iterator) end);
		virtual ~ContentPointer();
	};

	typedef CDataVectorT<ContentPointer> ContentPointers;

	class WINSOURCES_API ResourceItem: public CCppObject
	{
	public:
		CStringBuffer _resource;
		CFilePath _filePath;
		ContentPointers _pointers;
		bool _updated;

		ResourceItem(ConstRef(CStringBuffer) resource);
		virtual ~ResourceItem();
	};

	typedef CDataVectorT<ResourceItem> ResourceItems;

	class WINSOURCES_API ServerItem: public CCppObject
	{
	public:
		CStringBuffer _server;
		ResourceItems _items;

		ServerItem(ConstRef(CStringBuffer) server);
		virtual ~ServerItem();
	};

	typedef CDataVectorT<ServerItem> ServerItems;

public:
	CHtmlPage(void);
	CHtmlPage(ConstRef(CFilePath) fpath, ConstRef(CStringBuffer) spath, ConstRef(CByteLinkedBuffer) buffer);
	virtual ~CHtmlPage(void);

	__inline ConstRef(CFilePath) GetCachePath() const { return _cachePath; }
	__inline ConstRef(CStringBuffer) GetServerPath() const { return _serverPath; }
	__inline ConstRef(CByteLinkedBuffer) GetDocumentBuffer() const { return _documentBuffer; }
	__inline ConstRef(ServerItems) GetServerItems() const { return _serverItems; }

	void ReplaceImages();
	bool GetAssocPath(ConstRef(CStringBuffer) url, Ref(CFilePath) path);

protected:
	CFilePath _cachePath;
	CStringBuffer _serverPath;
	CByteLinkedBuffer _documentBuffer;
	ServerItems _serverItems;
};

