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
#include "ByteLinkedBuffer.h"

static void __stdcall CByteLinkedBufferDeleteFunc(ConstPointer data, Pointer context)
{
	CByteLinkedBuffer::TBufferItem* p = CastAnyPtr(CByteLinkedBuffer::TBufferItem, CastMutable(Pointer, data));

	TFfree(p->_buffer);
}

CByteLinkedBuffer::Iterator::~Iterator()
{
	if ( !(_bufferItems.Release()) )
		_bufferItems.Close(CByteLinkedBufferDeleteFunc, NULL);
}

ConstRef(CByteLinkedBuffer::Iterator) CByteLinkedBuffer::Iterator::operator=(ConstRef(CByteLinkedBuffer::Iterator) copy)
{
	_bufferItems.Copy(copy._bufferItems, CByteLinkedBufferDeleteFunc, NULL);
	_index = copy._index;
	_pos = copy._pos;
	_totalLength = copy._totalLength;
	return *this;
}

CByteLinkedBuffer::Iterator& CByteLinkedBuffer::Iterator::operator++()
{
	if ( _index < _bufferItems.Count() )
	{
		TBufferItems::Iterator it = _bufferItems.Index(_index);

		if ( _pos < ((*it)->_bufferSize) )
		{
			++_pos;
			++_totalLength;
		}
		else
		{
			++_index;
			_pos = 0;
            if ( _index >= _bufferItems.Count() )
            {
                _totalLength = 0;
                return *this;
            }
		}
	}
	return *this;
}

CByteLinkedBuffer::Iterator& CByteLinkedBuffer::Iterator::operator--()
{
	if ( _index < _bufferItems.Count() )
	{
		TBufferItems::Iterator it = _bufferItems.Index(_index);

		assert(_pos <= (*it)->_bufferSize);

		if ( _pos > 0 )
		{
			--_pos;
			--_totalLength;
		}
        else if ( _index > 0 )
		{
			--_index;
            it = _bufferItems.Index(_index);
			_pos = (*it)->_bufferSize;
		}
        else
        {
            _index = _bufferItems.Count();
            _pos = 0;
            _totalLength = 0;
        } 
	}
	return *this;
}

CByteLinkedBuffer::Iterator& CByteLinkedBuffer::Iterator::operator+=(dword delta)
{
	if ( _index < _bufferItems.Count() )
	{
		TBufferItems::Iterator it = _bufferItems.Index(_index);

		assert(_pos <= (*it)->_bufferSize);

		dword size = (*it)->_bufferSize - _pos;

		if ( delta <= size )
		{
			size = delta;
			_pos += delta;
			delta = 0;
		}
		else
		{
			_pos = 0;
			++_index;
            if ( _index >= _bufferItems.Count() )
            {
                _totalLength = 0;
                return *this;
            }
			delta -= size;
			++it;
		}
		_totalLength += size;
		while ( delta > 0 )
		{
			size = (*it)->_bufferSize;
			if ( delta <= size )
			{
				size = delta;
				_pos += delta;
				delta = 0;
			}
			else
			{
				_pos = 0;
				++_index;
                if ( _index >= _bufferItems.Count() )
                {
                    _totalLength = 0;
                    return *this;
                }
				++it;
				delta -= size;
			}
			_totalLength += size;
		}
	}
	return *this;
}

CByteLinkedBuffer::Iterator& CByteLinkedBuffer::Iterator::operator-=(dword delta)
{
	if ( _index < _bufferItems.Count() )
	{
		TBufferItems::Iterator it = _bufferItems.Index(_index);
		dword size = _pos + 1;

		if ( delta <= size )
		{
			size = delta;
			_pos -= delta;
			delta = 0;
		}
		else if ( _index == 0 )
        {
            _pos = 0;
            _totalLength = 0;
            _index = _bufferItems.Count();
            return *this;
        }
        else
        {
			delta -= size;
			--_index;
			--it;
			_pos = (*it)->_bufferSize - 1;
		}
		_totalLength -= size;
		while ( delta > 0 )
		{
			size = _pos + 1;
			if ( delta <= size )
			{
				size = delta;
				_pos -= delta;
				delta = 0;
			}
			else if ( _index == 0 )
            {
                _pos = 0;
                _totalLength = 0;
                _index = _bufferItems.Count();
                return *this;
            }
            else
            {
				delta -= size;
				--_index;
				--it;
				_pos = (*it)->_bufferSize - 1;
			}
			_totalLength -= size;
		}
	}
	return *this;
}

int CByteLinkedBuffer::Iterator::operator-(Iterator& other)
{
	if ( _index >= _bufferItems.Count() )
		return 0;
	if ( other._index >= other._bufferItems.Count() )
		return 0;
	if ( _index == other._index )
		return _pos - other._pos;
	if ( _index < other._index )
	{
		int result;
		dword index = _index;
		TBufferItems::Iterator it = _bufferItems.Index(index);

		result = (*it)->_bufferSize - _pos;
		++it;
		++index;
		while ( it && (index < other._index) )
		{
			result += (*it)->_bufferSize;
			++it;
			++index;
		}
		if ( index < other._index )
			return -result;
		result += other._pos;
		return -result;
	}
	int result;
	dword index = other._index;
	TBufferItems::Iterator it = _bufferItems.Index(index);

	result = (*it)->_bufferSize - other._pos;
	++it;
	++index;
	while ( it && (index < _index) )
	{
		result += (*it)->_bufferSize;
		++it;
		++index;
	}
	if ( index < _index )
		return result;
	result += _pos;
	return result;
}

CByteLinkedBuffer::Iterator& CByteLinkedBuffer::Iterator::Find(BPointer str, dword strLen)
{
	if ( (str == NULL) || (strLen == 0) )
		return *this;
	Find(*str);
	while ( _index < _bufferItems.Count() )
	{
		if ( Compare(str, strLen) )
			return *this;
		operator++();
		if ( _index >= _bufferItems.Count() )
			return *this;
		Find(*str);
	}
	return *this;
}

CByteLinkedBuffer::Iterator& CByteLinkedBuffer::Iterator::FindAny(BPointer str, dword strLen)
{
	if ( (str == NULL) || (strLen == 0) )
		return *this;
	if ( _index < _bufferItems.Count() )
	{
		TBufferItems::Iterator it = _bufferItems.Index(_index);
		BPointer bufP = (*it)->_buffer + _pos;
		dword size = (*it)->_bufferSize;

		while ( _pos < size )
		{
			if ( NotPtrCheck(s_memchr(str, *bufP, strLen)) )
				return *this;
			++bufP;
			++_pos;
			++_totalLength;
		}
		++_index;
		++it;	
		_pos = 0;
		while ( _index < _bufferItems.Count() )
		{
			bufP = (*it)->_buffer;
			size = (*it)->_bufferSize;
			while ( _pos < size )
			{
				if ( NotPtrCheck(s_memchr(str, *bufP, strLen)) )
					return *this;
				++bufP;
				++_pos;
				++_totalLength;
			}
			++_index;
			++it;
			_pos = 0;
		}
	}
	return *this;
}

CByteLinkedBuffer::Iterator& CByteLinkedBuffer::Iterator::FindAnyStr(BArray strs, DWPointer strslen, dword strscnt)
{
	BPointer strsB = CastAny(BPointer, TFalloc(strscnt * szbyte));
	BPointer p = strsB;

	for (dword i = 0; i < strscnt; i++)
		*p++ = strs[i][0];
	FindAny(strsB, strscnt);
	while (_index < _bufferItems.Count())
	{
		TBufferItems::Iterator it = _bufferItems.Index(_index);
		BPointer bufP = (*it)->_buffer + _pos;

		p = CastAny(BPointer, CastMutablePtr(void, s_memchr(strsB, *bufP, strscnt)));
		if (Compare(strs[Castdword(p-strsB)], strslen[Castdword(p-strsB)]))
			break;
		operator++();
		if (_index >= _bufferItems.Count())
			break;
		FindAny(strsB, strscnt);
	}
	TFfree(strsB);
	return *this;
}

bool CByteLinkedBuffer::Iterator::Compare(BPointer str, dword strLen)
{
	if ( _index < _bufferItems.Count() )
	{
		TBufferItems::Iterator it = _bufferItems.Index(_index);
		BPointer bufP = (*it)->_buffer + _pos;
		dword size = (*it)->_bufferSize;
		dword pos = _pos;

		while ( (pos < size) && (strLen > 0) )
		{
			if ( *bufP++ != *str++ )
				return false;
			++pos;
			--strLen;
		}
		++it;
		while ( it && (strLen > 0) )
		{
			pos = 0;
			size = (*it)->_bufferSize;
			bufP = (*it)->_buffer;
			while ( (pos < size) && (strLen > 0) )
			{
				if ( *bufP++ != *str++ )
					return false;
				++pos;
				--strLen;
			}
			++it;
		}
		return (strLen == 0);
	}
	return false;
}

CByteLinkedBuffer::Iterator& CByteLinkedBuffer::Iterator::Find(byte ch)
{
	if ( _index < _bufferItems.Count() )
	{
		TBufferItems::Iterator it = _bufferItems.Index(_index);
		BPointer bufP = (*it)->_buffer + _pos;
		dword size = (*it)->_bufferSize;

		while ( _pos < size )
		{
			if ( *bufP++ == ch )
				return *this;
			++_pos;
			++_totalLength;
		}
		++_index;
		++it;	
		_pos = 0;
		while ( _index < _bufferItems.Count() )
		{
			bufP = (*it)->_buffer;
			size = (*it)->_bufferSize;
			while ( _pos < size )
			{
				if ( *bufP++ == ch )
					return *this;
				++_pos;
				++_totalLength;
			}
			++_index;
			++it;
			_pos = 0;
		}
	}
	return *this;
}

BPointer CByteLinkedBuffer::Iterator::operator *()
{
	if ( _index >= _bufferItems.Count() )
		return NULL;

	TBufferItems::Iterator it = _bufferItems.Index(_index);

	if ( (*it)->_bufferSize <= _pos )
		return NULL;
	return (*it)->_buffer + _pos;
}

dword CByteLinkedBuffer::Iterator::GetBufferLength() const
{
	if ( _index >= _bufferItems.Count() )
		return 0;

	TBufferItems::Iterator it = _bufferItems.Index(_index);

	if ( (*it)->_bufferSize <= _pos )
		return 0;
	return (*it)->_bufferSize - _pos;
}

void CByteLinkedBuffer::Iterator::GetSubBuffer(Ref(CByteBuffer) buffer) const
{
	if (_index >= _bufferItems.Count())
		return;

	dword len = buffer.get_BufferSize();

	if (0 == len)
		return;

	Iterator it(*this);
	BPointer bufP = buffer.get_Buffer();
	dword size = it.GetBufferLength();

	if (0 == size)
	{
		++it;
		size = it.GetBufferLength();
	}
	if (size > len)
	{
		size = len;
		len = 0;
	}
	else
		len -= size;
	s_memcpy(bufP, *it, size);
	bufP += size;
	it += size;
	while (it && (len > 0))
	{
		size = it.GetBufferLength();
		if (0 == size)
		{
			++it;
			size = it.GetBufferLength();
		}
		if (size > len)
		{
			size = len;
			len = 0;
		}
		else
			len -= size;
		s_memcpy(bufP, *it, size);
		bufP += size;
		it += size;
	}
	if (len > 0)
	{
		len = buffer.get_BufferSize() - len;
		buffer.set_BufferSize(__FILE__LINE__ len);
	}
}

CByteLinkedBuffer::CByteLinkedBuffer(void):
	_bufferItems(__FILE__LINE__ 256),
	_totalLength(0)
{
}

CByteLinkedBuffer::CByteLinkedBuffer(ConstRef(CByteLinkedBuffer) copy):
    _bufferItems(copy._bufferItems),
	_totalLength(copy._totalLength)
{
}

CByteLinkedBuffer::~CByteLinkedBuffer(void)
{
	Close();
}

ConstRef(CByteLinkedBuffer) CByteLinkedBuffer::operator = (ConstRef(CByteLinkedBuffer) copy)
{
	_bufferItems.Copy(copy._bufferItems, CByteLinkedBufferDeleteFunc, NULL);
	_totalLength = copy._totalLength;
	return *this;
}

TListCnt CByteLinkedBuffer::AddRef()
{
	return _bufferItems.AddRef();
}

TListCnt CByteLinkedBuffer::Release()
{
	TListCnt cnt = _bufferItems.Release();

	if ( !cnt )
		_bufferItems.Close(CByteLinkedBufferDeleteFunc, NULL);
	return cnt;
}

void CByteLinkedBuffer::Close()
{
	if ( !(_bufferItems.Release()) )
		_bufferItems.Close(CByteLinkedBufferDeleteFunc, NULL);
	_totalLength = 0;
}

void CByteLinkedBuffer::Clear()
{
	for ( dword ix = GetBufferItemCount(); ix > 0; --ix )
		RemoveBufferItem(ix - 1);
	_totalLength = 0;
}

CByteLinkedBuffer::Iterator CByteLinkedBuffer::Begin() const
{
	Iterator it(_bufferItems);

	return it;
}

CByteLinkedBuffer::Iterator CByteLinkedBuffer::Index(dword ix) const
{
	Iterator it(_bufferItems, ix);

	return it;
}

void CByteLinkedBuffer::SetBegin(CByteLinkedBuffer::Iterator it)
{
	dword imax = it.GetIndex();

	if ( imax >= GetBufferItemCount() )
	{
		Clear();
		return;
	}
	for ( dword ix = imax; ix > 0; --ix )
		RemoveBufferItem(ix - 1);
	if ( it.GetPosition() > 0 )
	{
		dword bufSz;
		BPointer bufP = GetBufferItem(0, &bufSz);

		if ( bufSz <= it.GetPosition() )
		{
			RemoveBufferItem(0);
			return;
		}
		bufSz -= it.GetPosition();
		s_memcpy(bufP, bufP + it.GetPosition(), bufSz);
		SetBufferItemSize(0, bufSz);
	}
}

BPointer CByteLinkedBuffer::AddBufferItem(dword size)
{
	if (0 == size)
		return NULL;

	TBufferItem item;

	item._bufferSize = size;
	item._buffer = CastAny(BPointer, TFalloc(size));

	_bufferItems.Append(&item);
	_totalLength += size;
	return item._buffer;
}

void CByteLinkedBuffer::AddBufferItem(ConstRef(CByteBuffer) buf)
{
	AddBufferItem(buf.get_Buffer(), buf.get_BufferSize());
}

void CByteLinkedBuffer::AddBufferItem(BPointer buf, dword len)
{
	if (PtrCheck(buf) || (0 == len))
		return;

	TBufferItem item;

	item._bufferSize = len;
	item._buffer = CastAny(BPointer, TFalloc(len));
	s_memmove(item._buffer, buf, len);
	_bufferItems.Append(&item);
	_totalLength += len;
}

BPointer CByteLinkedBuffer::GetBufferItem(dword ix, dword* size) const
{
	if ( ix >= _bufferItems.Count() )
		return NULL;

	TBufferItems::Iterator it = _bufferItems.Index(ix);

	if ( size )
		*size = (*it)->_bufferSize;
	return (*it)->_buffer;
}

dword CByteLinkedBuffer::GetBufferItemSize(dword ix) const
{
	if ( ix >= _bufferItems.Count() )
		return 0;

	TBufferItems::Iterator it = _bufferItems.Index(ix);

	return (*it)->_bufferSize;
}

void CByteLinkedBuffer::GetBufferItem(dword ix, Ref(CByteBuffer) buffer)
{
	if (ix >= _bufferItems.Count())
	{
		buffer.set_BufferSize(__FILE__LINE__ 0);
		return;
	}

	TBufferItems::Iterator it = _bufferItems.Index(ix);

	buffer.set_BufferSize(__FILE__LINE__ (*it)->_bufferSize);
	s_memmove(buffer.get_Buffer(), (*it)->_buffer, (*it)->_bufferSize);
}

void CByteLinkedBuffer::SetBufferItemSize(dword ix, dword size)
{
	if (ix >= _bufferItems.Count())
		return;

	if (0 == size)
	{
		RemoveBufferItem(ix);
		return;
	}

	TBufferItems::Iterator it = _bufferItems.Index(ix);

	if ( size > Castdword(TFsize((*it)->_buffer)) )
		(*it)->_buffer = (BPointer)TFrealloc((*it)->_buffer, size);

	_totalLength -= (*it)->_bufferSize;
	(*it)->_bufferSize = size;
	_totalLength += size;
}

void CByteLinkedBuffer::RemoveBufferItem(dword ix)
{
	if ( ix >= _bufferItems.Count() )
		return;

	TBufferItems::Iterator it = _bufferItems.Index(ix);

	_totalLength -= (*it)->_bufferSize;
	_bufferItems.Remove(it, CByteLinkedBufferDeleteFunc, NULL);
}

BPointer CByteLinkedBuffer::InsertBufferItem(dword ix, dword size)
{
	if ((ix >= _bufferItems.Count()) || (0 == size))
		return NULL;

	TBufferItems::Iterator it = _bufferItems.Index(ix);
	TBufferItem item;

	item._bufferSize = size;
	item._buffer = CastAny(BPointer, TFalloc(size));

	_bufferItems.InsertAfter(it, &item);
	_totalLength += size;
	return item._buffer;
}

void CByteLinkedBuffer::InsertBufferItem(dword ix, ConstRef(CByteBuffer) buf)
{
	InsertBufferItem(ix, buf.get_Buffer(), buf.get_BufferSize());
}

void CByteLinkedBuffer::InsertBufferItem(dword ix, BPointer buf, dword len)
{
	if ((ix >= _bufferItems.Count()) || PtrCheck(buf) || (0 == len))
		return;

	TBufferItems::Iterator it = _bufferItems.Index(ix);
	TBufferItem item;

	item._bufferSize = len;
	item._buffer = CastAny(BPointer, TFalloc(len));
	s_memmove(item._buffer, buf, len);

	_bufferItems.InsertAfter(it, &item);
	_totalLength += len;
}

void CByteLinkedBuffer::GetBuffer(Ref(CByteBuffer) buffer) const
{
	buffer.set_BufferSize(__FILE__LINE__ _totalLength);

	BPointer bufP = buffer.get_Buffer();
	TBufferItems::Iterator it = _bufferItems.Begin();
	dword size;

	while ( it )
	{
		size = (*it)->_bufferSize;
		s_memcpy(bufP, (*it)->_buffer, size);
		bufP += size;
		++it;
	}
}

void CByteLinkedBuffer::GetSubBuffer(CByteLinkedBuffer::Iterator it, Ref(CByteBuffer) buffer) const
{
	if ( !it )
		return;

	dword len = buffer.get_BufferSize();

	if (0 == len)
		return;

	BPointer bufP = buffer.get_Buffer();
	dword size = it.GetBufferLength();

	if ( 0 == size )
	{
		++it;
		size = it.GetBufferLength();
	}
	if ( size > len )
	{
		size = len;
		len = 0;
	}
	else
		len -= size;
	s_memcpy(bufP, *it, size);
	bufP += size;
	it += size;
	while ( it && (len > 0) )
	{
		size = it.GetBufferLength();
		if ( 0 == size )
		{
			++it;
			size = it.GetBufferLength();
		}
		if ( size > len )
		{
			size = len;
			len = 0;
		}
		else
			len -= size;
		s_memcpy(bufP, *it, size);
		bufP += size;
		it += size;
	}
	if (len > 0)
	{
		len = buffer.get_BufferSize() - len;
		buffer.set_BufferSize(__FILE__LINE__ len);
	}
}

void CByteLinkedBuffer::ReplaceSubBuffer(CByteLinkedBuffer::Iterator it, dword len, ConstRef(CByteBuffer) buffer)
{
	if ( !it )
		return;

	BPointer bufP = buffer.get_Buffer();
	dword len1 = buffer.get_BufferSize();
	dword size = it.GetBufferLength();

	if ( 0 == size )
	{
		++it;
		size = it.GetBufferLength();
	}
	if ( size > Min(len,len1) )
		size = Min(len,len1);
	len -= size;
	len1 -= size;
	s_memcpy(*it, bufP, size);
	bufP += size;
	it += size;
	while ( it && (len > 0) && (len1 > 0) )
	{
		size = it.GetBufferLength();
		if ( 0 == size )
		{
			++it;
			size = it.GetBufferLength();
		}
		if ( size > Min(len,len1) )
			size = Min(len,len1);
		len -= size;
		len1 -= size;
		s_memcpy(*it, bufP, size);
		bufP += size;
		it += size;
	}
	if ( !it )
	{
		size = Min(len,len1);
		if ( size > 0 )
		{
			BPointer bufP2 = AddBufferItem(size);

			s_memcpy(bufP2, bufP, size);
		}
	}
	else if ( len > 0 ) // len1 == 0
	{
		if ( it.GetPosition() > 0 )
		{
			size = it.GetBufferLength();
			if ( 0 == size )
			{
				++it;
				size = it.GetBufferLength();
			}
			if ( size > len )
			{
				s_memcpy(*it, (*it) + len, size - len);
				SetBufferItemSize(it.GetIndex(), it.GetPosition() + size - len);
				len = 0;
			}
			else
			{
				SetBufferItemSize(it.GetIndex(), it.GetPosition());
				len -= size;
				++it;
			}
		}
		while ( it && (len > 0) )
		{
			size = it.GetBufferLength();
			if ( 0 == size )
			{
				++it;
				size = it.GetBufferLength();
			}
			if ( size > len )
			{
				s_memcpy(*it, (*it) + len, size - len);
				SetBufferItemSize(it.GetIndex(), size - len);
				len = 0;
			}
			else
			{
				RemoveBufferItem(it.GetIndex());
				len -= size;
			}
		}
	}
	else if ( len1 > 0 ) // len == 0
	{
		if ( it.GetPosition() > 0 )
		{
			BPointer bufP2 = InsertBufferItem(it.GetIndex(), len1);

			size = it.GetBufferLength();
			if ( 0 == size )
			{
				++it;
				size = it.GetBufferLength();
			}
			if ( size > len1 )
			{
				s_memcpy(bufP2, (*it) + size - len1, len1);
				s_memcpy((*it) + len1, *it, size - len1);
				s_memcpy(*it, bufP, len1);
			}
			else
			{
				s_memcpy(bufP2 + len1 - size, *it, size);
				s_memcpy(*it, bufP, size);
				bufP += size;
				s_memcpy(bufP2, bufP, len1 - size);
			}
		}
		else
		{
			size = it.GetBufferLength();
			if ( 0 == size )
			{
				++it;
				size = it.GetBufferLength();
			}
			SetBufferItemSize(it.GetIndex(), size + len1);
			s_memcpy(*it + len1, (*it), size);
			s_memcpy(*it, bufP, len1);
		}
	}
}

void CByteLinkedBuffer::InsertBuffer(Iterator it, ConstRef(CByteBuffer) buffer)
{
	InsertBuffer(it, buffer.get_Buffer(), buffer.get_BufferSize());
}

void CByteLinkedBuffer::InsertBuffer(Iterator it, BPointer buf, dword len)
{
	if ((!it) || PtrCheck(buf) || (0 == len))
		return;
	
	dword size = GetBufferItemSize(it.GetIndex());

	SetBufferItemSize(it.GetIndex(), size + len);

	BPointer bufP1 = GetBufferItem(it.GetIndex());

	s_memmove(bufP1 + it.GetPosition() + len, bufP1 + it.GetPosition(), size - it.GetPosition());
	s_memcpy(bufP1 + it.GetPosition(), buf, len);
}