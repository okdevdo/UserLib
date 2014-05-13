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
#include "DataBTree.h"

class CPPSOURCES_API CByteLinkedBuffer
{
public:
	typedef struct _tag_BufferItem
	{
		BPointer _buffer;
		dword _bufferSize;
	} TBufferItem;

	typedef CDataSBTreeT<TBufferItem> TBufferItems;

	class CPPSOURCES_API Iterator
	{
	public:
		Iterator(ConstRef(TBufferItems) bufferItems, dword ix = 0):
		    _bufferItems(bufferItems), // AddRef
			_index(ix),
			_pos(0),
			_totalLength(0)
			{
			}
		Iterator(ConstRef(Iterator) copy):
		    _bufferItems(copy._bufferItems), // AddRef
			_index(copy._index),
			_pos(copy._pos),
			_totalLength(copy._totalLength)
			{
			}
		~Iterator();
		ConstRef(Iterator) operator=(ConstRef(Iterator) copy);
		Iterator& operator++();
		Iterator& operator--();
		Iterator& operator+=(dword delta);
		Iterator& operator-=(dword delta);
		int operator-(Iterator& other);
		Iterator& Find(BPointer str, dword strLen);
		Iterator& FindAny(BPointer str, dword strLen);
		Iterator& FindAnyStr(BArray strs, DWPointer strslen, dword strscnt);
		bool Compare(BPointer str, dword strLen);
		Iterator& Find(byte ch);
		BPointer operator *();
		__inline operator bool() const { return (_index < _bufferItems.Count()); }

		__inline dword GetIndex() const { return _index; }
		__inline dword GetPosition() const { return _pos; }
		dword GetBufferLength() const;
		__inline dword GetTotalLength() const { return _totalLength; }

		void GetSubBuffer(Ref(CByteBuffer) buffer) const;

	protected:
		TBufferItems _bufferItems;
		dword _index;
		dword _pos;
		dword _totalLength;

	private:
		Iterator();
	};

public:
	CByteLinkedBuffer(void);
	CByteLinkedBuffer(ConstRef(CByteLinkedBuffer) copy);
	virtual ~CByteLinkedBuffer(void);

	ConstRef(CByteLinkedBuffer) operator = (ConstRef(CByteLinkedBuffer) copy);

	void Clear();
	void Close();
	TListCnt AddRef();
	TListCnt Release();
	Iterator Begin() const;
	Iterator Index(dword ix) const;
	void SetBegin(Iterator it);
	BPointer AddBufferItem(dword size);	
	void AddBufferItem(ConstRef(CByteBuffer) buf);
	void AddBufferItem(BPointer buf, dword len);
	BPointer GetBufferItem(dword ix, dword* size = NULL) const;
	dword GetBufferItemSize(dword ix) const;
	void GetBufferItem(dword ix, Ref(CByteBuffer) buffer);
	void SetBufferItemSize(dword ix, dword size);
	void RemoveBufferItem(dword ix);
	BPointer InsertBufferItem(dword ix, dword size);
	void InsertBufferItem(dword ix, ConstRef(CByteBuffer) buf);
	void InsertBufferItem(dword ix, BPointer buf, dword len);
	void GetBuffer(Ref(CByteBuffer) buffer) const;
	void GetSubBuffer(Iterator it, Ref(CByteBuffer) buffer) const;
	void ReplaceSubBuffer(Iterator it, dword len, ConstRef(CByteBuffer) buffer);
	void InsertBuffer(Iterator it, ConstRef(CByteBuffer) buffer);
	void InsertBuffer(Iterator it, BPointer buf, dword len);

	__inline dword GetBufferItemCount() const { return _bufferItems.Count(); }
	__inline dword GetTotalLength() const { return _totalLength; }

protected:
	TBufferItems _bufferItems;
	dword _totalLength;
};

__inline bool operator < (ConstRef(CByteLinkedBuffer::Iterator) it1, ConstRef(CByteLinkedBuffer::Iterator) it2)
{
	return it1.GetTotalLength() < it2.GetTotalLength();
}

__inline bool operator <= (ConstRef(CByteLinkedBuffer::Iterator) it1, ConstRef(CByteLinkedBuffer::Iterator) it2)
{
	return it1.GetTotalLength() <= it2.GetTotalLength();
}

__inline bool operator == (ConstRef(CByteLinkedBuffer::Iterator) it1, ConstRef(CByteLinkedBuffer::Iterator) it2)
{
	return it1.GetTotalLength() == it2.GetTotalLength();
}

__inline bool operator > (ConstRef(CByteLinkedBuffer::Iterator) it1, ConstRef(CByteLinkedBuffer::Iterator) it2)
{
	return it1.GetTotalLength() > it2.GetTotalLength();
}

__inline bool operator >= (ConstRef(CByteLinkedBuffer::Iterator) it1, ConstRef(CByteLinkedBuffer::Iterator) it2)
{
	return it1.GetTotalLength() >= it2.GetTotalLength();
}

__inline bool operator != (ConstRef(CByteLinkedBuffer::Iterator) it1, ConstRef(CByteLinkedBuffer::Iterator) it2)
{
	return it1.GetTotalLength() != it2.GetTotalLength();
}

