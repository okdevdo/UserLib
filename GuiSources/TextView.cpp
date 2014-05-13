/******************************************************************************
    
	This file is part of GuiSources, which is part of UserLib.

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
#include "stdafx.h"
#include "TextView.h"
#include "GuiApplication.h"
#include "ResourceManager.h"
#include "Resource.h"
#include "DataVector.h"

#define SHIFTED 0x8000

#define MOUSEHIT_NULL					0
#define MOUSEHIT_LBUTTONDOWN			1
#define MOUSEHIT_LBUTTONMOVE			2
#define MOUSEHIT_LBUTTONUP				3
#define MOUSEHIT_LBUTTONDOUBLEDOWN		4

#ifdef _UNICODE
static const wchar_t* _whitespace = L"\x0009\x000A\x000B\x000C\x000D\x0020\x0085\x00A0\x1680\x180E\x2000\x2001\x2002\x2003\x2004\x2005\x2006\x2007\x2008\x2009\x200A\x2028\x2029\x202F\x205F\x3000";

static BOOL IsWhitespace(wchar_t pChar)
{
	const wchar_t* p = _whitespace;

	while ( *p != 0 )
	{
		if ( *p == pChar )
			return TRUE;
		++p;
	}
	return FALSE;
}
#else
static const char* _whitespace = "\x09\x0A\x0B\x0C\x0D\x20";

static BOOL IsWhitespace(char pChar)
{
	const char* p = _whitespace;

	while ( *p != 0 )
	{
		if ( *p == pChar )
			return TRUE;
		++p;
	}
	return FALSE;
}
#endif

__inline bool operator ==(ConstRef(POINT) pa, ConstRef(POINT) pb)
{
	return ((pa.x == pb.x) && (pa.y == pb.y));
}

__inline bool operator <(ConstRef(POINT) pa, ConstRef(POINT) pb)
{
	return ((pa.y < pb.y) || ((pa.y == pb.y) && (pa.x < pb.x)));
}

__inline bool operator <=(ConstRef(POINT) pa, ConstRef(POINT) pb)
{
	return ((pa.y < pb.y) || ((pa.y == pb.y) && (pa.x <= pb.x)));
}

class CStringTabbedConstIterator 
{
public:
    CStringTabbedConstIterator( CConstPointer _str = 0, dword tabsize = 4 ): 
		_origin(CastMutable(CPointer,_str)), 
		_current(CastMutable(CPointer,_str)),
		_tabsize(tabsize),
		_cursize(0)
	{
	}

    CStringTabbedConstIterator( CConstPointer _strOrigin, CConstPointer _strCurrent, dword tabsize = 4 ): 
		_origin(CastMutable(CPointer,_strOrigin)), 
		_current(CastMutable(CPointer,_strCurrent)),
		_tabsize(tabsize),
		_cursize(0)
	{
		calc_cursize();
	}

	CStringTabbedConstIterator(ConstRef(CStringConstIterator) _str, dword tabsize = 4): 
		_origin(CastMutable(CPointer,_str.GetOrigin())), 
		_current(CastMutable(CPointer,_str.GetCurrent())),
		_tabsize(tabsize),
		_cursize(0)
	{
		calc_cursize();
	}

	CStringTabbedConstIterator(ConstRef(CStringTabbedConstIterator) _str): 
		_origin(_str._origin), 
		_current(_str._current),
		_tabsize(_str._tabsize),
		_cursize(_str._cursize)
	{
	}

    ~CStringTabbedConstIterator()
	{
	}

    __inline CStringTabbedConstIterator& operator = (CConstPointer _str)
	{
		_current = CastMutable(CPointer,_str);
		_origin = _current;
		_cursize = 0;
		return *this;
	}

    __inline CStringTabbedConstIterator& operator = (ConstRef(CStringConstIterator) _str)
	{
		_current = CastMutable(CPointer,_str.GetCurrent());
		_origin = CastMutable(CPointer,_str.GetOrigin());
		calc_cursize();
		return *this;
	}

    __inline CStringTabbedConstIterator& operator = (ConstRef(CStringTabbedConstIterator) _str)
	{
		_current = _str._current;
		_origin = _str._origin;
		_tabsize = _str._tabsize;
		_cursize = _str._cursize;
		return *this;
	}

public:
	__inline dword get_tabsize() const { return _tabsize; }
	__inline void set_tabsize(dword s) { _tabsize = s; }

	__inline dword get_cursize() const { return _cursize; }
	__inline void set_cursize(WLong& cursize) 
	{
		bool bTab = false;
		WLong cnt = 0;
		WLong tcnt = 0;
		WLong ltcnt = 0;

		_current = _origin;
		while ( ((cursize > cnt) || (*_current == 0x09)) && (*_current != 0) )
		{
			switch ( *_current )
			{
			case 0x09:
				if ( !bTab )
				{
					ltcnt = cnt;
					tcnt = ((cnt + _tabsize) / _tabsize) * _tabsize;
					bTab = true;
				}
				else if ( cnt == tcnt )
				{
					if ( cursize < cnt )
					{
						_cursize = ltcnt;
						cursize -= cnt;
						return;
					}
					++_current;
					if ( (cursize == cnt) || (*_current == 0) )
					{
						_cursize = cnt;
						cursize -= _cursize;
						return;
					}
					if ( *_current != 0x09 )
					{
						++_current;
						bTab = false;
					}
					else
					{
						ltcnt = tcnt;
						tcnt += _tabsize;
					}
				}
				break;
			default:
				++_current;
				bTab = false;
				break;
			}
			++cnt;
		}
		_cursize = cnt;
		cursize -= _cursize;
	}

	__inline WULong GetSize() const { return (NULL == _current) ? 0 : (s_strlen( _current, INT_MAX ) + 1); }
	__inline WULong GetLength() const { return (NULL == _current) ? 0 : s_strlen( _current, INT_MAX ); }
	__inline WULong GetTabbedLength() const
	{
		CPointer text = _current;
		bool bTab = false;
		WLong cnt = _cursize;
		WLong tcnt = 0;

		if ( NULL == _current )
			return 0;
		while ( *text != 0 )
		{
			switch ( *text )
			{
			case 0x09:
				if ( !bTab )
				{
					tcnt = ((cnt + _tabsize) / _tabsize) * _tabsize;
					bTab = true;
				}
				else if ( cnt == tcnt )
				{
					++text;
					if ( *text == 0 )
						return cnt - _cursize;
					if ( *text != 0x09 )
					{
						++text;
						bTab = false;
					}
					else
						tcnt += _tabsize;
				}
				break;
			default:
				++text;
				bTab = false;
				break;
			}
			++cnt;
		}
		return cnt - _cursize;
	}

	__inline CConstPointer GetCurrent() const { return _current; }
	__inline CConstPointer GetOrigin() const { return _origin; }
    __inline WBool IsTab() const { return ((NULL == _current) || (*_current == 0)) ? false : (*_current == _T('\t')); }
    __inline WBool IsEnd() const { return (NULL == _current) ? true : (*_current == 0); }
	__inline const WChar operator[] (dword ix) const { return (NULL == _current) ? 0 : *(_current + ix); }

    __inline CStringTabbedConstIterator& operator++()
	{
		if ( !IsEnd() )
		{
			if ( IsTab() )
				_cursize = ((_cursize + _tabsize) / _tabsize) * _tabsize;
			else
				++_cursize;
			++_current;
		}
		return *this;
	}

    CStringTabbedConstIterator operator++(int)
	{
		CStringTabbedConstIterator tmp = *this;
		++(*this);
		return tmp;
	}

	CStringTabbedConstIterator& FindTab()
	{
		while ( !IsEnd() )
		{
			if ( IsTab() )
				break;
			++_current;
			++_cursize;
		}
		return *this;
	}

protected:
    CPointer _origin;
    CPointer _current;
	dword _tabsize;
	dword _cursize;

	void calc_cursize()
	{
		CPointer text = _origin;
		bool bTab = false;
		WLong cnt = 0;
		WLong tcnt = 0;

		while ( (text != _current) && (*text != 0) )
		{
			switch ( *text )
			{
			case 0x09:
				if ( !bTab )
				{
					tcnt = ((cnt + _tabsize) / _tabsize) * _tabsize;
					bTab = true;
				}
				else if ( cnt == tcnt )
				{
					++text;
					if ( (text == _current) || (*text == 0) )
					{
						_cursize = cnt;
						if ( *text == 0 )
							_current = text;
						return;
					}
					if ( *text != 0x09 )
					{
						++text;
						bTab = false;
					}
					else
						tcnt += _tabsize;
				}
				break;
			default:
				++text;
				bTab = false;
				break;
			}
			++cnt;
		}
		_cursize = cnt;
		if ( *text == 0 )
			_current = text;
	}
};

class CTextViewChars: public CCppObject
{
	typedef CDataVectorT<CStringBuffer> TBuffer;

public:
	CTextViewChars(): 
		m_buffer(__FILE__LINE__ 1024, 1024)
	{
		m_sellb.x = -1;
		m_sellb.y = -1;
		m_selub.x = -1;
		m_selub.y = -1;
		m_lastSellb.x = -1;
		m_lastSellb.y = -1;
		m_lastSelub.x = -1;
		m_lastSelub.y = -1;
		m_fontSize.cx = 0;
		m_fontSize.cy = 0;
		m_linebreak.SetString(__FILE__LINE__ _T("\r\n"));
		m_tabsize = 4;
	}
	virtual ~CTextViewChars() {}

	__inline dword get_tabsize() const { return m_tabsize; }
	__inline void set_tabsize(dword tabsize) { m_tabsize = tabsize; }

	__inline ConstRef(SIZE) get_fontsize() const { return m_fontSize; }
	__inline void set_fontsize(ConstRef(SIZE) sz) { m_fontSize = sz; }
	__inline void set_fontsize(LONG cx, LONG cy) { m_fontSize.cx = cx; m_fontSize.cy = cy; }

	__inline ConstRef(CStringBuffer) get_linebreak() { return m_linebreak; }
	__inline void set_linebreak(ConstRef(CStringBuffer) t) { m_linebreak = t; }

	__inline dword get_LineCount() const { return m_buffer.Count(); }
	__inline dword get_CharCount() const { return m_buffer.GetLength(); }
	__inline dword get_MaxLineLength() const { return m_buffer.GetMaxLineLength(); }
	__inline dword get_LineLength(LONG line) const 
	{ 
		CStringTabbedConstIterator tmp(*(m_buffer.Index(line)), m_tabsize);

		return tmp.GetTabbedLength();
	}
	__inline POINT get_EndPoint() const
	{
		POINT pt = { get_LineLength(get_LineCount() - 1), get_LineCount() - 1 };

		return pt;
	}

	__inline void Clear() 
	{
		m_buffer.Close();
		m_buffer.Open(__FILE__LINE__ 1024, 1024);
		m_sellb.x = -1;
		m_sellb.y = -1;
		m_selub.x = -1;
		m_selub.y = -1;
		m_lastSellb.x = -1;
		m_lastSellb.y = -1;
		m_lastSelub.x = -1;
		m_lastSelub.y = -1;
	}

	__inline void Normalize(POINT& pos)
	{
		dword len;

		if ( (get_LineCount() == 0) || (pos.y < 0) )
		{
			pos.y = 0;
			pos.x = 0;
		}
		else if ( Cast(dword, pos.y) >= get_LineCount() )
		{
			pos.y = get_LineCount() - 1;
			pos.x = get_LineLength(pos.y);
		}
		else if ( pos.x < 0 )
			pos.x = 0;
		else
		{
			len = get_LineLength(pos.y);

			if ( Cast(dword, pos.x) > len )
				pos.x = len;
		}
	}

	__inline void get_NextChar(POINT& pos)
	{
		Normalize(pos);
		++(pos.x);
		if ( Cast(dword, pos.x) > get_LineLength(pos.y) )
		{
			if ( Cast(dword, pos.y) < (get_LineCount() - 1) )
			{
				++(pos.y);
				pos.x = 0;
			}
			else
				--(pos.x);
		}
	}

	__inline void get_PrevChar(POINT& pos)
	{
		Normalize(pos);
		--(pos.x);
		if ( pos.x < 0 )
		{
			if ( pos.y > 0 )
			{
				--(pos.y);
				pos.x = get_LineLength(pos.y);
			}
			else
				++(pos.x);
		}
	}

	__inline void get_NextLineStart(POINT& start)
	{
		if ( (get_LineCount() == 0) || (start.y < 0) )
		{
			start.y = 0;
			start.x = 0;
		}
		else if ( Cast(dword, start.y) >= (get_LineCount() - 1) )
		{
			start.y = get_LineCount() - 1;
			start.x = get_LineLength(start.y);
		}
		else if ( (start.x >= 0) && (Cast(dword, start.x) >= get_LineLength(start.y)) )
		{
			++(start.y);
			start.x = get_LineLength(start.y);
		}
		else
			start.x = get_LineLength(start.y);
	}

	__inline void get_PrevLineStart(POINT& start)
	{
		if ( (get_LineCount() == 0) || (start.y <= 0) )
		{
			start.y = 0;
			start.x = 0;
		}
		else if ( Cast(dword, start.y) >= get_LineCount() )
		{
			start.y = get_LineCount() - 1;
			start.x = 0;
		}
		else if ( start.x <= 0 )
		{
			--(start.y);
			start.x = 0;
		}
		else
			start.x = 0;
	}

	__inline void get_NextLineIndex(POINT& start)
	{
		if ( Cast(dword, start.y) < (get_LineCount() - 1) )
		{
			dword len = get_LineLength(start.y + 1);

			++(start.y);
			if ( len < Cast(dword, start.x) )
				start.x = len;
		}
		else
			start = get_EndPoint();
	}

	__inline void get_PrevLineIndex(POINT& start)
	{
		if ( start.y > 0 )
		{
			dword len = get_LineLength(start.y - 1);

			--(start.y);
			if ( start.x < 0 )
				start.x = 0;
			if ( len < Cast(dword, start.x) )
				start.x = len;
		}
		else
		{
			start.y = 0;
			start.x = 0;
		}
	}

	__inline void get_NextPageIndex(POINT& start, LONG pgsize)
	{
		if ( Cast(dword, start.y) <= (get_LineCount() - pgsize - 3) )
		{
			dword len;

			start.y += pgsize - 2;
			len = get_LineLength(start.y);
			if ( len < Cast(dword, start.x) )
				start.x = len;
		}
		else
			start = get_EndPoint();
	}

	__inline void get_PrevPageIndex(POINT& start, LONG pgsize)
	{
		if ( start.y >= (pgsize - 2) )
		{
			dword len;

			start.y -= pgsize - 2;
			if ( start.x < 0 )
				start.x = 0;
			len = get_LineLength(start.y);
			if ( len < Cast(dword, start.x) )
				start.x = len;
		}
		else
		{
			start.y = 0;
			start.x = 0;
		}
	}

	__inline void SetText(ConstRef(CStringBuffer) t)
	{
		Clear();
		m_buffer.Split(t, m_linebreak);
	}

	__inline POINT SetText(POINT pos, LPCTSTR lptstr, dword len0)
	{
		Normalize(pos);

		if ( PtrCheck(lptstr) || (len0 == 0) )
			return pos;

		CStringBuffer tmp(__FILE__LINE__ lptstr, len0);
		TBuffer tmpB(__FILE__LINE__ 128, 1024);

		tmpB.Split(tmp, m_linebreak);

		TBuffer::Iterator it = m_buffer.Index(pos.y);
		CStringBuffer tmp1 = *it;
		CStringBuffer tmp2;
		CStringTabbedConstIterator itS(tmp1, m_tabsize);
		WLong insX = pos.x;
		dword len;

		itS.set_cursize(insX);
		if ( insX < 0 )
		{
			tmp1.DeleteString(Castdword(itS.GetCurrent() - itS.GetOrigin()));
			tmp2.Fill(_T(' '), m_tabsize);
			tmp1.InsertString(Castdword(itS.GetCurrent() - itS.GetOrigin()), tmp2);
			itS = tmp1;
			insX = pos.x;
			itS.set_cursize(insX);
			assert(insX == 0);
		}
		insX = Castdword(itS.GetCurrent() - itS.GetOrigin());
		if ( tmpB.Count() == 1 )
		{
			CStringBuffer tmp2 = *(tmpB.Index(0));

			len = tmp2.GetLength();
			tmp1.InsertString(insX, tmp2.GetString(), len);
			m_buffer.SetData(it, tmp1);
			itS = tmp1;
			insX = pos.x;
			itS.set_cursize(insX);
			insX = itS.get_cursize();
			for ( ; len > 0; --len )
				++itS;
			pos.x += itS.get_cursize() - insX;
			return pos;
		}

		CStringBuffer tmp3;

		len = tmp1.GetLength();
		tmp1.SubString(insX, len - insX, tmp2);
		tmp1.DeleteString(insX, len - insX);
		tmp1.AppendString((*(tmpB.Index(0))).GetString());

		tmp3 = *(tmpB.Index(tmpB.Count() - 1));
		len = tmp3.GetLength();
		tmp2.PrependString(tmp3.GetString(), len);
		itS = tmp3;
		pos.x = itS.GetTabbedLength();

		m_buffer.SetData(it, tmp1);
		m_buffer.InsertAfter(it, tmp2);

		if ( tmpB.Count() > 2 )
		{
			TBuffer::Iterator itB = tmpB.Index(1);
			TBuffer::Iterator itBE = tmpB.Index(tmpB.Count() - 1);
			
			while ( itB != itBE )
			{
				it = m_buffer.InsertAfter(it, *itB);
				++itB;
				++(pos.y);
			}
		}
		++(pos.y);
		return pos;
	}

	__inline CStringBuffer GetText()
	{
		return m_buffer.Join(m_linebreak);
	}

	__inline CStringBuffer GetText(POINT start, POINT ende)
	{
		Normalize(start);
		Normalize(ende);

		TBuffer::Iterator it = m_buffer.Index(start.y);
		TBuffer::Iterator itE = m_buffer.Index(ende.y);
		CStringBuffer tmp;
		CStringBuffer tmp1 = *it;
		CStringTabbedConstIterator itS(tmp1, m_tabsize);
		WLong insX = start.x;

		itS.set_cursize(insX);
		if ( it == itE )
		{
			tmp1.SubString(start.x, ende.x - start.x, tmp);
			return tmp;
		}

		CStringBuffer tmp2;

		tmp1.SubString(start.x, tmp1.GetLength() - start.x, tmp);
		++it;
		while ( it != itE )
		{
			tmp += m_linebreak;
			tmp += m_buffer.GetData(it);
			++it;
		}
		tmp1 = *it;
		tmp1.SubString(0, ende.x, tmp2);
		tmp += m_linebreak;
		tmp += tmp2;
		return tmp;
	}

	__inline void Remove(POINT start, POINT ende)
	{
		Normalize(start);
		Normalize(ende);

		TBuffer::Iterator it = m_buffer.Index(start.y);
		TBuffer::Iterator itE = m_buffer.Index(ende.y);
		CStringBuffer tmp;
		CStringBuffer tmp1;

		tmp = *it;
		if ( it == itE )
		{
			tmp.DeleteString(start.x, ende.x - start.x);
			m_buffer.SetData(it, tmp);
			return;
		}
		tmp.DeleteString(start.x, tmp.GetLength() - start.x);
		tmp1 = *itE;
		tmp1.DeleteString(0, ende.x);
		tmp += tmp1;
		m_buffer.SetData(it, tmp);
		while ( it != itE )
		{
			TBuffer::Iterator itE1 = itE;

			--itE1;
			m_buffer.Remove(itE);
			itE = itE1;
		}
	}

	__inline POINT get_LowerBound() { return m_sellb; }
	__inline POINT get_UpperBound() { return m_selub; }

	__inline POINT get_LastLowerBound() { return m_lastSellb; }
	__inline POINT get_LastUpperBound() { return m_lastSelub; }

	__inline void reset_Bounds(void) { m_lastSellb = m_sellb; m_lastSelub = m_selub; }

	__inline void reset_Selection(void)
	{
		if ( ((m_sellb.x < 0) && (m_sellb.y < 0)) || ((m_selub.x < 0) && (m_selub.y < 0)) )
			return;
		reset_Bounds();
		m_sellb.x = -1;
		m_sellb.y = -1;
		m_selub.x = -1;
		m_selub.y = -1;
	}

	__inline bool has_Selection(void) 
	{ 
		return ((m_sellb.x >= 0) && (m_sellb.y >= 0) && (m_selub.x >= 0) && (m_selub.y >= 0) 
			&& (((m_sellb.y == m_selub.y) && (m_sellb.x <= m_selub.x)) || (m_sellb.y < m_selub.y))); 
	}

	__inline void expand_Selection(POINT newlbub, POINT fix)
	{
		reset_Bounds();
		assert((fix == m_sellb) || (fix == m_selub));
		if ( fix == m_sellb )
		{
			if ( newlbub <= fix )
				set_Selection(newlbub, fix);
			else
				m_selub = newlbub;
		}
		else
		{
			if ( fix <= newlbub )
				set_Selection(fix, newlbub);
			else
				m_sellb = newlbub;
		}
	}

	__inline void set_Selection(POINT lb, POINT ub)
	{
		POINT lastSellb = m_sellb;
		POINT lastSelub = m_selub;

		this->reset_Selection();

		m_lastSellb = lastSellb;
		m_lastSelub = lastSelub;

		if ( lb <= ub )
		{
			m_sellb = lb;
			m_selub = ub;
		}
		else
		{
			m_sellb = ub;
			m_selub = lb;
		}
	}

	__inline bool is_whitespace(POINT ix)
	{
		TBuffer::Iterator it = m_buffer.Index(ix.y);

		if ( it )
		{
			CStringBuffer tmp = *it;
			CStringConstIterator itS(tmp);

			itS += ix.x;
			if ( (!(itS.IsEnd())) && IsWhitespace(itS[0]) )
				return true;
		}
		return false;
	}

	__inline POINT skip_Whitespace(POINT start, bool forward = true)
	{
		if ( forward )
		{
			POINT ix = start;
			TBuffer::Iterator it = m_buffer.Index(ix.y);

			if ( it )
			{
				CStringBuffer tmp = *it;
				CStringConstIterator itS(tmp);
				bool bStart = true;

				itS += ix.x;
				while ( !(itS.IsEnd()) )
				{
					if ( !(IsWhitespace(itS[0])) )
					{
						if ( !bStart )
						{
							--(ix.x);
							return ix;
						}
					}
					++itS;
					++(ix.x);
					bStart = false;
				}
				++it;
			}
			if ( it )
			{
				++(ix.y);
				ix.x = 0;
			}
			return ix;
		}
		else
		{
			POINT ix = start;
			TBuffer::Iterator it = m_buffer.Index(ix.y);

			if ( it )
			{
				CStringBuffer tmp = *it;
				CStringConstIterator itS(tmp);
				bool bStart = true;

				itS += ix.x;
				while ( !(itS.IsBegin()) )
				{
					if ( !(IsWhitespace(itS[0])) )
					{
						if ( !bStart )
						{
							++(ix.x);
							return ix;
						}
					}
					--itS;
					--(ix.x);
					bStart = false;
				}
				--it;
			}
			if ( it )
			{
				CStringBuffer tmp = *it;

				--(ix.y);
				ix.x = tmp.GetLength();
			}
			return ix;
		}
	}

	__inline POINT skip_NonWhitespace(POINT start, bool forward = true)
	{
		if ( forward )
		{
			POINT ix = start;
			TBuffer::Iterator it = m_buffer.Index(ix.y);

			if ( it )
			{
				CStringBuffer tmp = *it;
				CStringConstIterator itS(tmp);
				bool bStart = true;

				itS += ix.x;
				while ( !(itS.IsEnd()) )
				{
					if ( IsWhitespace(itS[0]) )
					{
						if ( !bStart )
						{
							--(ix.x);
							return ix;
						}
					}
					++itS;
					++(ix.x);
					bStart = false;
				}
				++it;
			}
			if ( it )
			{
				++(ix.y);
				ix.x = 0;
			}
			return ix;
		}
		else
		{
			POINT ix = start;
			TBuffer::Iterator it = m_buffer.Index(ix.y);

			if ( it )
			{
				CStringBuffer tmp = *it;
				CStringConstIterator itS(tmp);
				bool bStart = true;

				itS += ix.x;
				while ( !(itS.IsBegin()) )
				{
					if ( IsWhitespace(itS[0]) )
					{
						if ( !bStart )
						{
							++(ix.x);
							return ix;
						}
					}
					--itS;
					--(ix.x);
					bStart = false;
				}
				--it;
			}
			if ( it )
			{
				CStringBuffer tmp = *it;
				
				--(ix.y);
				ix.x = tmp.GetLength();
			}
			return ix;
		}
	}

	__inline void DrawText(Ptr(Gdiplus::Graphics) pGraphics, Ptr(Gdiplus::Font) pFont, Gdiplus::PointF pt, RECT rect, Ptr(Gdiplus::Brush) pBackground, Ptr(Gdiplus::Brush) pForeGround)
	{
		SIZE sz = get_fontsize();
		Gdiplus::RectF rectF;
		TBuffer::Iterator it = m_buffer.Index(rect.top);
		LONG line = rect.top;
		LONG tlen;

		if ( line > rect.bottom )
			return;
		while ( it )
		{
			CStringBuffer tmp = *it;
			CStringTabbedConstIterator itS(tmp, m_tabsize);
			CStringTabbedConstIterator itS1;
			WLong insX;
			bool bEnde = false;

			rectF.X = pt.X;
			rectF.Y = pt.Y;
			rectF.Width = Cast(Gdiplus::REAL, (rect.right - rect.left + 1) * sz.cx);
			rectF.Height = Cast(Gdiplus::REAL, sz.cy);
			pGraphics->FillRectangle(pBackground, rectF);

			insX = rect.left;
			itS.set_cursize(insX);
			if ( insX < 0 )
			{
				pt.X += Cast(Gdiplus::REAL, (-insX) * sz.cx);
				++itS;
			}
			while ( (!(itS.IsEnd())) && (!bEnde) )
			{
				itS1 = itS;
				itS.FindTab();
				tlen = Castdword(itS.GetCurrent() - itS1.GetCurrent());
				if ( tlen > (rect.right - rect.left + 1) )
				{
					tlen = rect.right - rect.left + 1;
					bEnde = true;
				}
				pGraphics->DrawString(itS1.GetCurrent(), tlen, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), pForeGround);
				pt.X += Cast(Gdiplus::REAL, tlen * sz.cx);
				if ( !bEnde && itS.IsTab() )
				{
					itS1 = itS;
					++itS;
					pt.X += Cast(Gdiplus::REAL, (itS.get_cursize() - itS1.get_cursize()) * sz.cx);
				}
			}
			pt.X = rectF.X;
			++it;
			pt.Y += sz.cy;
			++line;
			if ( line > rect.bottom )
				break;
		}
	}

	__inline bool TestDrawAllNormal(POINT _scrollPos, RECT _clientR)
	{
		if ( !has_Selection() )
			return true;
		if ( m_selub < _scrollPos )
			return true;

		POINT mx = _scrollPos;

		mx.x += _clientR.right / m_fontSize.cx;
		mx.y += _clientR.bottom / m_fontSize.cy;

		if ( mx <= m_sellb )
			return true;
		return false;
	}

	__inline bool TestDrawAllSelected(POINT _scrollPos, RECT _clientR)
	{
		if ( _scrollPos < m_sellb )
			return false;

		POINT mx = _scrollPos;

		mx.x += _clientR.right / m_fontSize.cx;
		mx.y += _clientR.bottom / m_fontSize.cy;

		if ( mx <= m_selub )
			return true;

		if ( (m_selub.y == (get_LineCount() - 1)) && (Cast(dword, m_selub.x) >= get_LineLength(get_LineCount() - 1)) )
			return true;

		if ( (Cast(dword, m_selub.y) >= get_LineCount()) && (m_selub.x >= 0) )
			return true;

		return false;
	}

	__inline bool TestDrawAllUpperSelected(POINT _scrollPos, RECT _clientR)
	{
		if ( _scrollPos < m_sellb )
			return false;

		POINT mx = _scrollPos;

		mx.x += _clientR.right / m_fontSize.cx;
		mx.y += _clientR.bottom / m_fontSize.cy;

		if ( m_selub < mx )
			return true;
		return false;
	}

	__inline bool TestDrawAllLowerSelected(POINT _scrollPos, RECT _clientR)
	{
		if ( _scrollPos < m_sellb )
		{
			POINT mx = _scrollPos;

			mx.x += _clientR.right / m_fontSize.cx;
			mx.y += _clientR.bottom / m_fontSize.cy;

			if ( mx <= m_selub )
				return true;

			if ( (m_selub.y == (get_LineCount() - 1)) && (Cast(dword, m_selub.x) >= get_LineLength(get_LineCount() - 1)) )
				return true;

			if ( (Cast(dword, m_selub.y) >= get_LineCount()) && (m_selub.x >= 0) )
				return true;
		}
		return false;
	}

	__inline POINT firstPosInClientRect(const Gdiplus::RectF& clientRectF, POINT scrollPos, POINT endPos)
	{
		POINT result;

		result.x = scrollPos.x;
		result.y = scrollPos.y;

		Gdiplus::PointF pt(Cast(Gdiplus::REAL, (endPos.x - scrollPos.x) * m_fontSize.cx), Cast(Gdiplus::REAL, (endPos.y - scrollPos.y) * m_fontSize.cy));

		if ( clientRectF.Contains(pt) )
			return result;

		POINT pt1 = { Cast(LONG, floor(((clientRectF.GetRight() / 2) / m_fontSize.cx) + 0.5)), 
			Cast(LONG, floor(((clientRectF.GetBottom() / 2) / m_fontSize.cy) + 0.5)) };

		if ( endPos.x > pt1.x )
			result.x = endPos.x - pt1.x;
		else
			result.x = 0;
		if ( endPos.y > pt1.y )
			result.y = endPos.y - pt1.y;
		else
			result.y = 0;
		return result;
	}

	__inline bool isPosInClientRect(const Gdiplus::RectF& clientRectF, POINT scrollPos, POINT pos)
	{
		Gdiplus::PointF pt(Cast(Gdiplus::REAL, (pos.x - scrollPos.x) * m_fontSize.cx), Cast(Gdiplus::REAL, (pos.y - scrollPos.y) * m_fontSize.cy));

		if ( clientRectF.Contains(pt) )
			return true;
		return false;
	}

	__inline bool isAnyLineEndNotInClientRect(const Gdiplus::RectF& clientRectF, POINT _scrollPos)
	{
		TBuffer::Iterator it = m_buffer.Index(_scrollPos.y);
		LONG line = 0;

		while ( it )
		{
			CStringBuffer tmp = *it;
			Gdiplus::PointF pt(Cast(Gdiplus::REAL, (tmp.GetLength() - _scrollPos.x) * m_fontSize.cx), Cast(Gdiplus::REAL, line * m_fontSize.cy));

			if ( pt.X > clientRectF.GetRight() )
				return true;
			if ( pt.Y > clientRectF.GetBottom() )
				return false;
			++it;
			++line;
		}
		return false;
	}

protected:
	POINT m_sellb;
	POINT m_selub;
	POINT m_lastSellb;
	POINT m_lastSelub;
	SIZE m_fontSize;
	dword m_tabsize;
	CStringBuffer m_linebreak;
	TBuffer m_buffer;
};

class CTextViewUndoStruct
{
public:
	CStringBuffer deleteChars;
	POINT deletePosStart;
	POINT deletePosEnde;
	CStringBuffer insertChars;
	POINT insertPosStart;
	POINT insertPosEnde;

	CTextViewUndoStruct()
	{
		deletePosStart.x = -1;
		deletePosStart.y = -1;
		deletePosEnde.x = -1;
		deletePosEnde.y = -1;
		insertPosStart.x = -1;
		insertPosStart.y = -1;
		insertPosEnde.x = -1;
		insertPosEnde.y = -1;
	}

	void addRef() 
	{
		deleteChars.addRef();
		insertChars.addRef();
	}
};

typedef CDataSVectorT<CTextViewUndoStruct> CTextViewUndoVector;

static void __stdcall TDeleteFunc_CTextViewUndoStack( ConstPointer data, Pointer context )
{
	Ptr(CTextViewUndoStruct) p = CastAnyPtr(CTextViewUndoStruct, CastMutable(Pointer, data));

	p->deleteChars.Clear();
	p->insertChars.Clear();
}

class CTextViewUndoStack: public CCppObject
{
public:
	CTextViewUndoStack() : m_currentPos(0), m_vector(__FILE__LINE__ 64, 32) {}
	virtual ~CTextViewUndoStack() { m_vector.Close(TDeleteFunc_CTextViewUndoStack, NULL); }

	__inline dword get_CurrentPos() const { return m_currentPos; }
	__inline dword size() const { return m_vector.Count(); }

	__inline void reset()
	{
		m_vector.Close(TDeleteFunc_CTextViewUndoStack, NULL);
		m_vector.Open(__FILE__LINE__ 64, 32);
		m_currentPos = 0;
	}

	__inline void push(CTextViewUndoStruct& undoStruct)
	{
		POINT null;

		null.x = 0;
		null.y = 0;
		if ( (undoStruct.deletePosStart < null) && (undoStruct.deletePosEnde < null) && (undoStruct.insertPosStart < null) && (undoStruct.insertPosEnde < null) )
			return;
		if ( !m_currentPos )
		{
			reset();
			undoStruct.addRef();
			m_vector.Append(&undoStruct);
			m_currentPos = 1;
			return;
		}
		while ( m_currentPos < m_vector.Count() )
			m_vector.Remove(m_vector.Last(), TDeleteFunc_CTextViewUndoStack, NULL);
		undoStruct.addRef();
		m_vector.Append(&undoStruct);
		++m_currentPos;
	}

	__inline CTextViewUndoStruct& current()
	{
		return *(*(m_vector.Index(m_currentPos)));
	}

	__inline void undo()
	{
		if ( m_currentPos > 0 )
			--m_currentPos;
	}

	__inline void redo()
	{
		if ( m_currentPos < m_vector.Count() )
			++m_currentPos;
	}

protected:
	dword m_currentPos;
	CTextViewUndoVector m_vector;
};

BEGIN_MESSAGE_MAP(CControl, CTextView)
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_MESSAGE(WM_GETTEXT,OnGetText)
	ON_WM_MESSAGE(WM_GETTEXTLENGTH,OnGetTextLength)
	ON_WM_MESSAGE(WM_SETTEXT,OnSetText)
	ON_WM_COMMMAND(IDM_CUT,OnEditCutCommand)
	ON_WM_COMMMAND(IDM_COPY,OnEditCopyCommand)
	ON_WM_COMMMAND(IDM_DELETE,OnEditDeleteCommand)
	ON_WM_COMMMAND(IDM_PASTE,OnEditPasteCommand)
	ON_WM_COMMMAND(IDM_UNDO,OnEditUndoCommand)
	ON_WM_COMMMAND(IDM_REDO,OnEditRedoCommand)
END_MESSAGE_MAP()

CTextView::CTextView(LPCTSTR name):
    CControl(name),
	m_mouseHitCode(MOUSEHIT_NULL),
	m_mouseHitModifier(0),
	m_caretCreated(FALSE),
	m_caretShown(FALSE),
	m_timerEnabled(FALSE),
	m_timerDirection(TTimerDirectionNone),
	m_insidePaint(FALSE),
	m_hasselection(FALSE),
	m_chars(NULL),
	m_undostack(NULL)
{
	m_chars = OK_NEW_OPERATOR CTextViewChars();
	m_undostack = OK_NEW_OPERATOR CTextViewUndoStack();
	m_cursorPos.x = 0;
	m_cursorPos.y = 0;
	m_cursorPosOld.x = 0;
	m_cursorPosOld.y = 0;
	m_scrollPos.x = 0;
	m_scrollPos.y = 0;
}

CTextView::CTextView(ConstRef(CStringBuffer) name):
    CControl(name),
	m_mouseHitCode(MOUSEHIT_NULL),
	m_mouseHitModifier(0),
	m_caretCreated(FALSE),
	m_caretShown(FALSE),
	m_timerEnabled(FALSE),
	m_timerDirection(TTimerDirectionNone),
	m_insidePaint(FALSE),
	m_hasselection(FALSE),
	m_chars(NULL),
	m_undostack(NULL)
{
	m_chars = OK_NEW_OPERATOR CTextViewChars();
	m_undostack = OK_NEW_OPERATOR CTextViewUndoStack();
	m_cursorPos.x = 0;
	m_cursorPos.y = 0;
	m_cursorPosOld.x = 0;
	m_cursorPosOld.y = 0;
	m_scrollPos.x = 0;
	m_scrollPos.y = 0;
}

CTextView::~CTextView(void)
{
	delete m_chars;
	delete m_undostack;
	if ( m_caretCreated )
		::DestroyCaret();
}

BOOL CTextView::PreRegisterClass(WNDCLASSEX& cls)
{
	cls.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	cls.lpszClassName	= _T("CTEXTVIEW");
	return TRUE;
}

void CTextView::get_BrushKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_BrushKeys(_keys, _T(".BackgroundColor.Selected"), _T("TextView"));
	add_BrushKeys(_keys, _T(".BackgroundColor.Normal"), _T("TextView"));
	add_BrushKeys(_keys, _T(".ForegroundColor.Selected"), _T("TextView"));
	add_BrushKeys(_keys, _T(".ForegroundColor.Normal"), _T("TextView"));
}

void CTextView::get_FontKeys(Ref(CDataVectorT<CStringBuffer>) _keys)
{
	add_FontKeys(_keys, _T(".Font.Normal"), _T("TextView"));
}

void CTextView::Refresh()
{
	Update(TRUE);
}

dword CTextView::get_tabsize() const
{
	return m_chars->get_tabsize();
}

void CTextView::set_tabsize(dword tabsize)
{
	m_chars->set_tabsize(tabsize);
	if ( is_created() )
		Update(FALSE);
}

CStringBuffer CTextView::get_text() const
{
	return m_chars->GetText();
}

void CTextView::set_text(const CStringBuffer& text) 
{ 
	m_chars->SetText(text);
	if ( is_created() )
		Refresh(TRUE);
}

void CTextView::Refresh(BOOL bUpdate)
{
	m_cursorPos.x = 0;
	m_cursorPos.y = 0;
	m_scrollPos.x = 0;
	m_scrollPos.y = 0;
	m_mouseHitCode = MOUSEHIT_NULL;
	m_mouseHitModifier = 0;
	m_chars->reset_Selection();
	m_undostack->reset();
	Update(bUpdate);
}

bool CTextView::has_selection() const
{
	return m_chars->has_Selection();
}

LRESULT CTextView::OnGetText(WPARAM wParam, LPARAM lParam)
{
	CStringBuffer text = get_text();
	CStringBuffer text1;
	dword len;

	if ( text.GetSize() <= wParam )
		text1 = text;
	else
		text.SubString(0, Castdword(wParam - 1), text1);
	len = text1.GetSize();
	s_memcpy(lParam, CastMutable(CPointer, text1.GetString()), len * szchar);
	return len - 1;
}

LRESULT CTextView::OnGetTextLength(WPARAM wParam, LPARAM lParam)
{
	return get_text().GetLength();
}

LRESULT CTextView::OnSetText(WPARAM wParam, LPARAM lParam)
{
	CStringBuffer text(__FILE__LINE__ CastAny(CPointer, lParam));

	set_text(text);
	return TRUE;
}

LRESULT CTextView::OnPaint(WPARAM wParam, LPARAM lParam)
{
	if ( m_insidePaint )
		return 0;
	m_insidePaint = TRUE;

	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(m_hwnd, &ps);
	if ( !hdc )
	{
		m_insidePaint = FALSE;
		return 0;
	}

	Gdiplus::Graphics graphics(hdc);
	Gdiplus::PointF pt;
	RECT rect;
	SIZE sz;
	Gdiplus::RectF rectF;
	Gdiplus::Font* pFont = get_Font(_T(".Font.Normal"), _T("TextView"));

	if ( m_updateRects )
	{
		graphics.MeasureString(_T("X"), 1, pFont, pt, Gdiplus::StringFormat::GenericTypographic(), &rectF);
		sz.cx = Cast(LONG, floor(rectF.Width + 0.5));
		sz.cy = Cast(LONG, floor(rectF.Height + 0.5));
		m_chars->set_fontsize(sz);
		::SetRectEmpty(&m_maxClientArea);
		m_maxClientArea.bottom = (m_chars->get_LineCount() + 10) * sz.cy;
		m_maxClientArea.right = (m_chars->get_MaxLineLength() + 10) * sz.cx;
		m_updateRects = FALSE;
	}

	Gdiplus::RectF clientrectF;
	RECT clientrect;

	GetClientRect(&clientrect);

	Gdiplus::Bitmap bitmap(clientrect.right, clientrect.bottom);
	Gdiplus::Graphics bitmapgraphics(&bitmap);

	Convert2RectF(&clientrectF, &clientrect);
	sz = m_chars->get_fontsize();
	ShowHorzScrollBar();
	ShowVertScrollBar();
	if ( (m_mouseHitCode != MOUSEHIT_NULL) && (m_chars->get_LineCount() > 0) )
	{
		pt.X = Cast(Gdiplus::REAL, m_mouseHit.x); pt.Y = Cast(Gdiplus::REAL, m_mouseHit.y);
		if ( !(clientrectF.Contains(pt)) )
		{
			switch ( m_mouseHitCode )
			{
			case MOUSEHIT_LBUTTONMOVE:
				if ( (pt.Y < (clientrectF.Y + 10)) && (pt.X >= clientrectF.X) && (pt.X <= (clientrectF.X + clientrectF.Width)) && (m_scrollPos.y > 0) )
				{
					m_timerDirection = TTimerDirectionNorth;
					_StartTimer();
				}
				else if ( (pt.X < (clientrectF.X + 10)) && (pt.Y >= clientrectF.Y) && (pt.Y <= (clientrectF.Y + clientrectF.Height)) && (m_scrollPos.x > 0) )
				{
					m_timerDirection = TTimerDirectionWest;
					_StartTimer();
				}
				else if ( (pt.Y > (clientrectF.Y + clientrectF.Height - 10)) && (pt.X >= clientrectF.X) && (pt.X <= (clientrectF.X + clientrectF.Width)) 
					&& (!(m_chars->isPosInClientRect(clientrectF, m_scrollPos, m_chars->get_EndPoint()))) )
				{
					m_timerDirection = TTimerDirectionSouth;
					_StartTimer();
				}
				else if ( (pt.X > (clientrectF.X + clientrectF.Width - 10)) && (pt.Y >= clientrectF.Y) && (pt.Y <= (clientrectF.Y + clientrectF.Height)) 
					&& (m_chars->isAnyLineEndNotInClientRect(clientrectF, m_scrollPos)) )
				{
					m_timerDirection = TTimerDirectionEast;
					_StartTimer();
				}
				else
				{
					m_timerDirection = TTimerDirectionNone;
					_StopTimer();
				}
				break;
			default:
				break;
			}
		}
		else
		{
			POINT ix;

			ix.x = m_scrollPos.x + (m_mouseHit.x / sz.cx);
			ix.y = m_scrollPos.y + (m_mouseHit.y / sz.cy);
			switch ( m_mouseHitCode )
			{
			case MOUSEHIT_LBUTTONDOWN:
				if ( m_mouseHitModifier == 0 )
				{
					m_cursorPos = ix;
					m_chars->reset_Selection();
				}
				else if ( m_mouseHitModifier == MK_SHIFT )
				{
					if ( m_chars->has_Selection() )
						m_chars->expand_Selection(ix, m_cursorPos);
					else
						m_chars->set_Selection(m_cursorPos, ix);
				}
				break;
			case MOUSEHIT_LBUTTONMOVE:
				_StopTimer();
				m_timerDirection = TTimerDirectionNone;
				if ( m_chars->has_Selection() )
					m_chars->expand_Selection(ix, m_cursorPos);
				else
					m_chars->set_Selection(m_cursorPos, ix);
				break;
			case MOUSEHIT_LBUTTONDOUBLEDOWN:
				if ( m_mouseHitModifier == 0 )
				{
					if ( m_chars->is_whitespace(ix) )
					{
						POINT start = m_chars->skip_Whitespace(ix);
						POINT end = m_chars->skip_NonWhitespace(start);

						m_cursorPos = start;
						m_chars->set_Selection(start, end);
					}
					else
					{
						POINT start = m_chars->skip_NonWhitespace(ix, false);
						POINT end = m_chars->skip_NonWhitespace(ix);

						m_cursorPos = start;
						m_chars->set_Selection(start, end);
					}
				}
				else if ( m_mouseHitModifier == MK_SHIFT )
				{
					if ( m_chars->is_whitespace(ix) )
					{
						POINT end = m_chars->skip_Whitespace(ix, false);
						POINT start = m_chars->skip_NonWhitespace(end, false);

						m_cursorPos = end;
						m_chars->set_Selection(start, end);
					}
					else
					{
						POINT start = m_chars->skip_NonWhitespace(ix, false);
						POINT end = m_chars->skip_NonWhitespace(ix);

						m_cursorPos = start;
						m_chars->set_Selection(start, end);
					}
				}
				break;
			default:
				break;
			}
		}
		m_mouseHitCode = MOUSEHIT_NULL;
		m_mouseHitModifier = 0;
	}

	Gdiplus::Brush* pBackgroundColorNormal = get_Brush(_T(".BackgroundColor.Normal"),  _T("TextView"), Gdiplus::Color::White);
	Gdiplus::Brush* pBackgroundColorSelected = get_Brush(_T(".BackgroundColor.Selected"),  _T("TextView"), Gdiplus::Color::Blue);
	Gdiplus::Brush* pForegroundColorNormal = get_Brush(_T(".ForegroundColor.Normal"),  _T("TextView"), Gdiplus::Color::Black);
	Gdiplus::Brush* pForegroundColorSelected = get_Brush(_T(".ForegroundColor.Selected"),  _T("TextView"), Gdiplus::Color::White);
	BOOL bShowCursor = TRUE;

	bitmapgraphics.FillRectangle(pBackgroundColorNormal, clientrectF);
	if ( m_chars->get_LineCount() == 0 )
	{
		m_insidePaint = FALSE;
		return -1;
	}
	if ( has_focus() )
	{
		if ( m_chars->has_Selection() )
			bShowCursor = FALSE;
		if ( bShowCursor )
		{
			if ( !m_caretShown )
			{
				if ( !m_caretCreated )
				{
					::CreateCaret(m_hwnd, (HBITMAP) NULL, 1, sz.cy);
					m_caretCreated = TRUE;
				}
				::ShowCaret(m_hwnd);
				m_caretShown = TRUE;
			}
		}
		else if ( m_caretShown )
		{
			::HideCaret(m_hwnd);
			m_caretShown = FALSE;
		}
	}
	if ( m_chars->has_Selection() )
	{
		if ( !(m_chars->get_UpperBound() == m_chars->get_LastUpperBound()) )
		{
			m_scrollPos = m_chars->firstPosInClientRect(clientrectF, m_scrollPos, m_chars->get_UpperBound());
			set_VScrollOffset(m_scrollPos.y * sz.cy);
			set_HScrollOffset(m_scrollPos.x * sz.cx);
		}
		else if ( !(m_chars->get_LowerBound() == m_chars->get_LastLowerBound()) )
		{
			m_scrollPos = m_chars->firstPosInClientRect(clientrectF, m_scrollPos, m_chars->get_LowerBound());
			set_VScrollOffset(m_scrollPos.y * sz.cy);
			set_HScrollOffset(m_scrollPos.x * sz.cx);
		}
		m_chars->reset_Bounds();
	}
	else if ( !(m_cursorPos == m_cursorPosOld) )
	{
		m_scrollPos = m_chars->firstPosInClientRect(clientrectF, m_scrollPos, m_cursorPos);
		set_VScrollOffset(m_scrollPos.y * sz.cy);
		set_HScrollOffset(m_scrollPos.x * sz.cx);
		m_cursorPosOld = m_cursorPos;
	}
	if ( bShowCursor )
	{
		::SetRectEmpty(&rect);
		rect.left = (m_cursorPos.x - m_scrollPos.x) * sz.cx;
		rect.top =  (m_cursorPos.y - m_scrollPos.y) * sz.cy;
		::SetCaretPos(rect.left, rect.top);
	}
	if ( m_chars->TestDrawAllNormal(m_scrollPos, clientrect) )
	{
		pt.X = 0; pt.Y = 0;
		rect.left = m_scrollPos.x;
		rect.top = m_scrollPos.y;
		rect.right = rect.left + (clientrect.right / sz.cx);
		rect.bottom = rect.top + (clientrect.bottom / sz.cy);
		m_chars->DrawText(&bitmapgraphics, pFont, pt, rect, pBackgroundColorNormal, pForegroundColorNormal);
	}
	else if ( m_chars->TestDrawAllSelected(m_scrollPos, clientrect) )
	{
		pt.X = 0; pt.Y = 0;
		rect.left = m_scrollPos.x;
		rect.top = m_scrollPos.y;
		rect.right = rect.left + (clientrect.right / sz.cx);
		rect.bottom = rect.top + (clientrect.bottom / sz.cy);
		m_chars->DrawText(&bitmapgraphics, pFont, pt, rect, pBackgroundColorSelected, pForegroundColorSelected);
	}
	else if ( m_chars->TestDrawAllUpperSelected(m_scrollPos, clientrect) )
	{
		POINT mx = m_chars->get_UpperBound();

		pt.X = 0; pt.Y = 0;
		rect.left = m_scrollPos.x;
		rect.top = m_scrollPos.y;
		rect.right = rect.left + (clientrect.right / sz.cx);
		rect.bottom = mx.y - 1;
		m_chars->DrawText(&bitmapgraphics, pFont, pt, rect, pBackgroundColorSelected, pForegroundColorSelected);
		pt.X = 0; pt.Y = Cast(Gdiplus::REAL, (mx.y - m_scrollPos.y) * sz.cy);
		rect.left = m_scrollPos.x;
		rect.top = mx.y;
		rect.right = mx.x;
		rect.bottom = mx.y;
		m_chars->DrawText(&bitmapgraphics, pFont, pt, rect, pBackgroundColorSelected, pForegroundColorSelected);
		pt.X = Cast(Gdiplus::REAL, ((mx.x + 1) - m_scrollPos.x) * sz.cx); pt.Y = Cast(Gdiplus::REAL, (mx.y - m_scrollPos.y) * sz.cy);
		rect.left = mx.x + 1;
		rect.top = mx.y;
		rect.right = m_scrollPos.x + (clientrect.right / sz.cx);
		rect.bottom = mx.y;
		m_chars->DrawText(&bitmapgraphics, pFont, pt, rect, pBackgroundColorNormal, pForegroundColorNormal);
		pt.X = 0; pt.Y = Cast(Gdiplus::REAL, ((mx.y + 1) - m_scrollPos.y) * sz.cy);
		rect.left = m_scrollPos.x;
		rect.top = mx.y + 1;
		rect.right = rect.left + (clientrect.right / sz.cx);
		rect.bottom = m_scrollPos.y + (clientrect.bottom / sz.cy);
		m_chars->DrawText(&bitmapgraphics, pFont, pt, rect, pBackgroundColorNormal, pForegroundColorNormal);
	}
	else if ( m_chars->TestDrawAllLowerSelected(m_scrollPos, clientrect) )
	{
		POINT mx = m_chars->get_LowerBound();

		pt.X = 0; pt.Y = 0;
		rect.left = m_scrollPos.x;
		rect.top = m_scrollPos.y;
		rect.right = rect.left + (clientrect.right / sz.cx);
		rect.bottom = mx.y - 1;
		m_chars->DrawText(&bitmapgraphics, pFont, pt, rect, pBackgroundColorNormal, pForegroundColorNormal);
		pt.X = 0; pt.Y = Cast(Gdiplus::REAL, (mx.y - m_scrollPos.y) * sz.cy);
		rect.left = m_scrollPos.x;
		rect.top = mx.y;
		rect.right = mx.x - 1;
		rect.bottom = mx.y;
		m_chars->DrawText(&bitmapgraphics, pFont, pt, rect, pBackgroundColorNormal, pForegroundColorNormal);
		pt.X = Cast(Gdiplus::REAL, (mx.x - m_scrollPos.x) * sz.cx); pt.Y = Cast(Gdiplus::REAL, (mx.y - m_scrollPos.y) * sz.cy);
		rect.left = mx.x;
		rect.top = mx.y;
		rect.right = m_scrollPos.x + (clientrect.right / sz.cx);
		rect.bottom = mx.y;
		m_chars->DrawText(&bitmapgraphics, pFont, pt, rect, pBackgroundColorSelected, pForegroundColorSelected);
		pt.X = 0; pt.Y = Cast(Gdiplus::REAL, ((mx.y + 1) - m_scrollPos.y) * sz.cy);
		rect.left = m_scrollPos.x;
		rect.top = mx.y + 1;
		rect.right = rect.left + (clientrect.right / sz.cx);
		rect.bottom = m_scrollPos.y + (clientrect.bottom / sz.cy);
		m_chars->DrawText(&bitmapgraphics, pFont, pt, rect, pBackgroundColorSelected, pForegroundColorSelected);
	}
	else
	{
		POINT mxl = m_chars->get_LowerBound();
		POINT mxu = m_chars->get_UpperBound();

		pt.X = 0; pt.Y = 0;
		rect.left = m_scrollPos.x;
		rect.top = m_scrollPos.y;
		rect.right = rect.left + (clientrect.right / sz.cx);
		rect.bottom = mxl.y - 1;
		m_chars->DrawText(&bitmapgraphics, pFont, pt, rect, pBackgroundColorNormal, pForegroundColorNormal);
		pt.X = 0; pt.Y = Cast(Gdiplus::REAL, (mxl.y - m_scrollPos.y) * sz.cy);
		rect.left = m_scrollPos.x;
		rect.top = mxl.y;
		rect.right = mxl.x - 1;
		rect.bottom = mxl.y;
		m_chars->DrawText(&bitmapgraphics, pFont, pt, rect, pBackgroundColorNormal, pForegroundColorNormal);
		if ( mxl.y == mxu.y )
		{
			pt.X = Cast(Gdiplus::REAL, (mxl.x - m_scrollPos.x) * sz.cx); pt.Y = Cast(Gdiplus::REAL, (mxl.y - m_scrollPos.y) * sz.cy);
			rect.left = mxl.x;
			rect.top = mxl.y;
			rect.right = mxu.x;
			rect.bottom = mxl.y;
			m_chars->DrawText(&bitmapgraphics, pFont, pt, rect, pBackgroundColorSelected, pForegroundColorSelected);
		}
		else
		{
			pt.X = Cast(Gdiplus::REAL, (mxl.x - m_scrollPos.x) * sz.cx); pt.Y = Cast(Gdiplus::REAL, (mxl.y - m_scrollPos.y) * sz.cy);
			rect.left = mxl.x;
			rect.top = mxl.y;
			rect.right = m_scrollPos.x + (clientrect.right / sz.cx);
			rect.bottom = mxl.y;
			m_chars->DrawText(&bitmapgraphics, pFont, pt, rect, pBackgroundColorSelected, pForegroundColorSelected);
			pt.X = 0; pt.Y = Cast(Gdiplus::REAL, ((mxl.y + 1) - m_scrollPos.y) * sz.cy);
			rect.left = m_scrollPos.x;
			rect.top = mxl.y + 1;
			rect.right = rect.left + (clientrect.right / sz.cx);
			rect.bottom = mxu.y - 1;
			m_chars->DrawText(&bitmapgraphics, pFont, pt, rect, pBackgroundColorSelected, pForegroundColorSelected);
			pt.X = 0; pt.Y = Cast(Gdiplus::REAL, (mxu.y - m_scrollPos.y) * sz.cy);
			rect.left = m_scrollPos.x;
			rect.top = mxu.y;
			rect.right = mxu.x;
			rect.bottom = mxu.y;
			m_chars->DrawText(&bitmapgraphics, pFont, pt, rect, pBackgroundColorSelected, pForegroundColorSelected);
		}
		pt.X = Cast(Gdiplus::REAL, ((mxu.x + 1) - m_scrollPos.x) * sz.cx); pt.Y = Cast(Gdiplus::REAL, (mxu.y - m_scrollPos.y) * sz.cy);
		rect.left = mxu.x + 1;
		rect.top = mxu.y;
		rect.right = m_scrollPos.x + (clientrect.right / sz.cx);
		rect.bottom = mxu.y;
		m_chars->DrawText(&bitmapgraphics, pFont, pt, rect, pBackgroundColorNormal, pForegroundColorNormal);
		pt.X = 0; pt.Y = Cast(Gdiplus::REAL, ((mxu.y + 1) - m_scrollPos.y) * sz.cy);
		rect.left = m_scrollPos.x;
		rect.top = mxu.y + 1;
		rect.right = rect.left + (clientrect.right / sz.cx);
		rect.bottom = m_scrollPos.y + (clientrect.bottom / sz.cy);
		m_chars->DrawText(&bitmapgraphics, pFont, pt, rect, pBackgroundColorNormal, pForegroundColorNormal);
	}

	graphics.DrawImage(&bitmap, 0, 0);
	EndPaint(m_hwnd, &ps);

	if ( ((!m_hasselection) && (m_chars->has_Selection()))
		|| (m_hasselection && (!(m_chars->has_Selection()))) )
	{
		SendNotifyMessage(NM_EDITORSELCHANGE, (LPARAM) this);
		SendNotifyMessage(NM_SELECTIONCHANGED);
		m_hasselection = m_chars->has_Selection()?TRUE:FALSE;
	}
	m_insidePaint = FALSE;
	return 0;
}

LRESULT CTextView::OnContextMenu(WPARAM wParam, LPARAM lParam)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

	if ( (pt.x < 0) && (pt.y < 0) )
		::GetCursorPos(&pt);

	TNotifyMessageEditorContextMenu param(this, pt);

	SendNotifyMessage(NM_EDITORCONTEXTMENU, (LPARAM)(&param));
	return 0;
}

LRESULT CTextView::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	set_focus();
	if ( wParam == MK_LBUTTON )
	{
		m_mouseHit.x = GET_X_LPARAM(lParam); 
		m_mouseHit.y = GET_Y_LPARAM(lParam); 
		m_mouseHitCode = MOUSEHIT_LBUTTONDOWN; 
		m_mouseHitModifier = 0;

		set_Capture(TRUE);
		Update(FALSE);
		return 0;
	}
	if ( wParam == (MK_LBUTTON | MK_SHIFT) )
	{
		m_mouseHit.x = GET_X_LPARAM(lParam); 
		m_mouseHit.y = GET_Y_LPARAM(lParam); 
		m_mouseHitCode = MOUSEHIT_LBUTTONDOWN; 
		m_mouseHitModifier = MK_SHIFT;

		set_Capture(TRUE);
		Update(FALSE);
		return 0;
	}
	return 0;
}

LRESULT CTextView::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == 0 )
	{
		m_timerDirection = TTimerDirectionNone;
		_StopTimer();
		set_Capture(FALSE);
		return 0;
	}
	if ( wParam == MK_SHIFT )
	{
		m_timerDirection = TTimerDirectionNone;
		_StopTimer();
		set_Capture(FALSE);
		return 0;
	}
	return 0;
}

LRESULT CTextView::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == MK_LBUTTON )
	{
		m_mouseHit.x = GET_X_LPARAM(lParam); 
		m_mouseHit.y = GET_Y_LPARAM(lParam); 
		m_mouseHitCode = MOUSEHIT_LBUTTONDOUBLEDOWN; 
		m_mouseHitModifier = 0;

		set_Capture(FALSE);
		Update(FALSE);
		return 0;
	}
	if ( wParam == (MK_LBUTTON | MK_SHIFT) )
	{
		m_mouseHit.x = GET_X_LPARAM(lParam); 
		m_mouseHit.y = GET_Y_LPARAM(lParam); 
		m_mouseHitCode = MOUSEHIT_LBUTTONDOUBLEDOWN; 
		m_mouseHitModifier = MK_SHIFT;

		set_Capture(FALSE);
		Update(FALSE);
		return 0;
	}
	return 0;
}

LRESULT CTextView::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == MK_LBUTTON )
	{
		m_mouseHit.x = GET_X_LPARAM(lParam); 
		m_mouseHit.y = GET_Y_LPARAM(lParam); 
		m_mouseHitCode = MOUSEHIT_LBUTTONMOVE; 
		m_mouseHitModifier = 0;

		Update(FALSE);
		return 0;
	}
	if ( wParam == (MK_LBUTTON | MK_SHIFT) )
	{
		m_mouseHit.x = GET_X_LPARAM(lParam); 
		m_mouseHit.y = GET_Y_LPARAM(lParam); 
		m_mouseHitCode = MOUSEHIT_LBUTTONMOVE; 
		m_mouseHitModifier = MK_SHIFT;

		Update(FALSE);
		return 0;
	}
	return 0;
}

LRESULT CTextView::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	BOOL bShift = ::GetKeyState(VK_SHIFT) & SHIFTED;
	BOOL bControl = ::GetKeyState(VK_CONTROL) & SHIFTED;

	if ( m_chars->get_LineCount() == 0 )
		return 0;
	switch ( wParam )
	{
	case 0x43: // C
		if ( bControl && (!bShift) )
			SendMessage(WM_COMMAND, MAKEWPARAM(IDM_COPY, 0), 0);
		break;
	case 0x56: // V
		if ( bControl && (!bShift) )
			SendMessage(WM_COMMAND, MAKEWPARAM(IDM_PASTE, 0), 0);
		break;
	case 0x58: // X
		if ( bControl && (!bShift) )
			SendMessage(WM_COMMAND, MAKEWPARAM(IDM_CUT, 0), 0);
		break;
	case 0x59: // Y
		if ( bControl && (!bShift) )
			SendMessage(WM_COMMAND, MAKEWPARAM(IDM_REDO, 0), 0);
		break;
	case 0x5A: // Z
		if ( bControl && (!bShift) )
			SendMessage(WM_COMMAND, MAKEWPARAM(IDM_UNDO, 0), 0);
		break;
	case VK_BACK:
		if ( bShift || bControl )
			break;
		if ( !(m_chars->has_Selection()) )
			m_chars->get_PrevChar(m_cursorPos);
	case VK_DELETE:
		if ( !(bShift || bControl) )
		{
			CTextViewUndoStruct undoStruct;

			if ( m_chars->has_Selection() )
				_Edit_Delete_Selection(undoStruct);
			else
				_Edit_Delete_Char(undoStruct);
			m_undostack->push(undoStruct);
			Update(TRUE);
		}
		break;
	case VK_HOME:
		{
			POINT null = { 0, 0 };

			if ( bShift && bControl )
			{
				if ( m_chars->has_Selection() )
				{
					if ( m_cursorPos == null )
						m_chars->reset_Selection();
					else
						m_chars->expand_Selection(null, m_cursorPos);
				}
				else
					m_chars->set_Selection(null, m_cursorPos);
				m_scrollPos = null;
				Update(FALSE);
			}
			else if ( bShift )
			{
				POINT lb;

				if ( m_chars->has_Selection() )
					lb = (m_cursorPos == m_chars->get_UpperBound())?(m_chars->get_LowerBound()):(m_chars->get_UpperBound());
				else
					lb = m_cursorPos;
				m_chars->get_PrevLineStart(lb);
				if ( m_chars->has_Selection() )
				{
					if ( m_cursorPos == null )
						m_chars->reset_Selection();
					else
						m_chars->expand_Selection(lb, m_cursorPos);
				}
				else
					m_chars->set_Selection(lb, m_cursorPos);
				Update(FALSE);
			}
			else if ( bControl )
			{
				m_chars->reset_Selection();
				m_cursorPos = null;
				Update(FALSE);
			}
			else
			{
				m_chars->reset_Selection();
				m_chars->get_PrevLineStart(m_cursorPos);
				Update(FALSE);
			}
		}
		break;
	case VK_END:
		{
			POINT len = m_chars->get_EndPoint();

			if ( bShift && bControl )
			{
				if ( m_chars->has_Selection() )
				{
					if ( m_cursorPos == len )
						m_chars->reset_Selection();
					else
						m_chars->expand_Selection(len, m_cursorPos);
				}
				else
					m_chars->set_Selection(m_cursorPos, len);
				Update(FALSE);
			}
			else if ( bShift )
			{
				POINT ub;

				if ( m_chars->has_Selection() )
					ub = (m_cursorPos == m_chars->get_LowerBound())?(m_chars->get_UpperBound()):(m_chars->get_LowerBound());
				else
					ub = m_cursorPos;
				m_chars->get_NextLineStart(ub);
				if ( m_chars->has_Selection() )
				{
					if ( m_cursorPos == len )
						m_chars->reset_Selection();
					else
						m_chars->expand_Selection(ub, m_cursorPos);
				}
				else
					m_chars->set_Selection(m_cursorPos, ub);
				Update(FALSE);
			}
			else if ( bControl )
			{
				m_chars->reset_Selection();
				m_cursorPos = len;
				Update(FALSE);
			}
			else
			{
				m_chars->reset_Selection();
				m_chars->get_NextLineStart(m_cursorPos);
				Update(FALSE);
			}
		}
		break;
	case VK_RIGHT:
		{
			POINT len = m_chars->get_EndPoint();

			if ( bShift && bControl )
			{
				if ( m_chars->has_Selection() )
				{
					POINT ix = (m_cursorPos == m_chars->get_LowerBound())?(m_chars->get_UpperBound()):(m_chars->get_LowerBound());

					if ( m_chars->is_whitespace(ix) )
						ix = m_chars->skip_Whitespace(ix);
					ix = m_chars->skip_NonWhitespace(ix);
					ix = m_chars->skip_Whitespace(ix);
					m_chars->expand_Selection(ix, m_cursorPos);
				}
				else
				{
					POINT ix = m_cursorPos;

					if ( !(m_chars->is_whitespace(ix)) )
						ix = m_chars->skip_NonWhitespace(ix);
					ix = m_chars->skip_Whitespace(ix);
					m_chars->set_Selection(ix, m_cursorPos);
				}
				Update(FALSE);
			}
			else if ( bShift )
			{
				if ( m_chars->has_Selection() )
				{
					POINT ub = (m_cursorPos == m_chars->get_LowerBound())?(m_chars->get_UpperBound()):(m_chars->get_LowerBound());

					if ( ub < len )
					{
						m_chars->get_NextChar(ub);
						m_chars->expand_Selection(ub, m_cursorPos);
						Update(FALSE);
					}
				}
				else
				{
					m_chars->set_Selection(m_cursorPos, m_cursorPos);
					Update(FALSE);
				}
			}
			else if ( bControl )
			{
				if ( m_chars->has_Selection() )
				{
					m_cursorPos = m_chars->get_UpperBound();
					m_chars->reset_Selection();
				}
				else
				{
					POINT ix = m_cursorPos;

					if ( m_chars->is_whitespace(ix) )
						ix = m_chars->skip_Whitespace(ix);
					ix = m_chars->skip_NonWhitespace(ix);
					ix = m_chars->skip_Whitespace(ix);
					m_cursorPos = ix;
				}
				Update(FALSE);
			}
			else if ( m_chars->has_Selection() )
			{
				m_cursorPos = m_chars->get_UpperBound();
				m_chars->reset_Selection();
				Update(FALSE);
			}
			else if ( m_cursorPos < len )
			{
				m_chars->get_NextChar(m_cursorPos);
				Update(FALSE);
			}
		}
		break;
	case VK_LEFT:
		{
			POINT null = { 0, 0 };

			if ( bShift && bControl )
			{
				if ( m_chars->has_Selection() )
				{
					POINT ix = (m_cursorPos == m_chars->get_LowerBound())?(m_chars->get_UpperBound()):(m_chars->get_LowerBound());

					if ( m_chars->is_whitespace(ix) )
						ix = m_chars->skip_Whitespace(ix, false);
					ix = m_chars->skip_NonWhitespace(ix, false);
					ix = m_chars->skip_Whitespace(ix, false);
					m_chars->expand_Selection(ix, m_cursorPos);
				}
				else
				{
					POINT ix = m_cursorPos;

					if ( !(m_chars->is_whitespace(ix)) )
						ix = m_chars->skip_NonWhitespace(ix, false);
					ix = m_chars->skip_Whitespace(ix);
					m_chars->set_Selection(ix, m_cursorPos);
				}
				Update(FALSE);
			}
			else if ( bShift )
			{
				if ( m_chars->has_Selection() )
				{
					POINT lb = (m_cursorPos == m_chars->get_UpperBound())?(m_chars->get_LowerBound()):(m_chars->get_UpperBound());

					if ( null < lb )
					{
						m_chars->get_PrevChar(lb);
						m_chars->expand_Selection(lb, m_cursorPos);
						Update(FALSE);
					}
				}
				else
				{
					m_chars->set_Selection(m_cursorPos, m_cursorPos);
					Update(FALSE);
				}
			}
			else if ( bControl )
			{
				if ( m_chars->has_Selection() )
				{
					m_cursorPos = m_chars->get_LowerBound();
					m_chars->reset_Selection();
				}
				else
				{
					POINT ix = m_cursorPos;

					if ( m_chars->is_whitespace(ix) )
						ix = m_chars->skip_Whitespace(ix, false);
					ix = m_chars->skip_NonWhitespace(ix, false);
					ix = m_chars->skip_Whitespace(ix, false);
					m_cursorPos = ix;
				}
				Update(FALSE);
			}
			else if ( m_chars->has_Selection() )
			{
				m_cursorPos = m_chars->get_LowerBound();
				m_chars->reset_Selection();
				Update(FALSE);
			}
			else if ( null < m_cursorPos )
			{
				m_chars->get_PrevChar(m_cursorPos);
				Update(FALSE);
			}
		}
		break;
	case VK_UP:
		if ( bShift && bControl )
		{
		}
		else if ( bShift )
		{
			if ( m_chars->has_Selection() )
			{
				POINT lb = (m_cursorPos == m_chars->get_UpperBound())?(m_chars->get_LowerBound()):(m_chars->get_UpperBound());

				m_chars->get_PrevLineIndex(lb);
				m_chars->expand_Selection(lb, m_cursorPos);
			}
			else
			{
				POINT lb = m_cursorPos;

				m_chars->get_PrevLineIndex(lb);
				m_chars->set_Selection(lb, m_cursorPos);
			}
			Update(FALSE);
		}
		else if ( bControl )
		{
		}
		else if ( m_chars->has_Selection() )
		{
			m_cursorPos = m_chars->get_LowerBound();
			m_chars->reset_Selection();
			Update(FALSE);
		}
		else
		{
			m_chars->get_PrevLineIndex(m_cursorPos);
			Update(FALSE);
		}
		break;
	case VK_DOWN:
		if ( bShift && bControl )
		{
		}
		else if ( bShift )
		{
			if ( m_chars->has_Selection() )
			{
				POINT ub = (m_cursorPos == m_chars->get_LowerBound())?(m_chars->get_UpperBound()):(m_chars->get_LowerBound());

				m_chars->get_NextLineIndex(ub);
				m_chars->expand_Selection(ub, m_cursorPos);
			}
			else
			{
				POINT ub = m_cursorPos;

				m_chars->get_NextLineIndex(ub);
				m_chars->set_Selection(m_cursorPos, ub);
			}
			Update(FALSE);
		}
		else if ( bControl )
		{
		}
		else if ( m_chars->has_Selection() )
		{
			m_cursorPos = m_chars->get_UpperBound();
			m_chars->reset_Selection();
			Update(FALSE);
		}
		else
		{
			m_chars->get_NextLineIndex(m_cursorPos);
			Update(FALSE);
		}
		break;
	case VK_PRIOR:
		{
			RECT r;

			GetClientRect(&r);
			r.bottom /= m_chars->get_fontsize().cy;
			if ( bShift && bControl )
			{
			}
			else if ( bShift )
			{
				if ( m_chars->has_Selection() )
				{
					POINT lb = (m_cursorPos == m_chars->get_UpperBound())?(m_chars->get_LowerBound()):(m_chars->get_UpperBound());

					m_chars->get_PrevPageIndex(lb, r.bottom);
					m_chars->expand_Selection(lb, m_cursorPos);
				}
				else
				{
					POINT lb = m_cursorPos;

					m_chars->get_PrevPageIndex(lb, r.bottom);
					m_chars->set_Selection(lb, m_cursorPos);
				}
				Update(FALSE);
			}
			else if ( bControl )
			{
			}
			else if ( m_chars->has_Selection() )
			{
				m_cursorPos = m_chars->get_LowerBound();
				m_chars->reset_Selection();
				Update(FALSE);
			}
			else
			{
				m_chars->get_PrevPageIndex(m_cursorPos, r.bottom);
				Update(FALSE);
			}
		}
		break;
	case VK_NEXT:
		{
			RECT r;

			GetClientRect(&r);
			r.bottom /= m_chars->get_fontsize().cy;
			if ( bShift && bControl )
			{
			}
			else if ( bShift )
			{
				if ( m_chars->has_Selection() )
				{
					POINT ub = (m_cursorPos == m_chars->get_LowerBound())?(m_chars->get_UpperBound()):(m_chars->get_LowerBound());

					m_chars->get_NextPageIndex(ub, r.bottom);
					m_chars->expand_Selection(ub, m_cursorPos);
				}
				else
				{
					POINT ub = m_cursorPos;

					m_chars->get_NextPageIndex(ub, r.bottom);
					m_chars->set_Selection(m_cursorPos, ub);
				}
				Update(FALSE);
			}
			else if ( bControl )
			{
			}
			else if ( m_chars->has_Selection() )
			{
				m_cursorPos = m_chars->get_UpperBound();
				m_chars->reset_Selection();
				Update(FALSE);
			}
			else
			{
				m_chars->get_NextPageIndex(m_cursorPos, r.bottom);
				Update(FALSE);
			}
		}
		break;
	case VK_ESCAPE:
		if ( bShift || bControl )
			break;
		SendNotifyMessage(NM_EDITORESCAPE, (LPARAM) this);
		break;
	case VK_RETURN:
		if ( !(bShift || bControl) )
		{
			CTextViewUndoStruct undoStruct;

			_Edit_Delete_Selection(undoStruct);
			_Edit_Insert_Chars(undoStruct, m_chars->get_linebreak().GetString(), m_chars->get_linebreak().GetLength());
			m_undostack->push(undoStruct);
			Update(TRUE);
		}
		break;
	case VK_TAB:
		if ( !(bShift || bControl) )
		{
			CTextViewUndoStruct undoStruct;

			_Edit_Delete_Selection(undoStruct);
			_Edit_Insert_Char(undoStruct, _T('\t'));
			m_undostack->push(undoStruct);
			Update(TRUE);
		}
		break;
	default:
		if ( bShift || bControl )
			break;
		if ( (wParam >= VK_F1) && (wParam <= VK_F24) )
		{
			TNotifyMessageEditorFuncKey param(this, wParam);

			SendNotifyMessage(NM_EDITORFUNCKEY, (LPARAM)(&param));
		}
		break;
	}
	return 0;
}

LRESULT CTextView::OnChar(WPARAM wParam, LPARAM lParam)
{
	if ( wParam < 0x20 )
		return 0;

	CTextViewUndoStruct undoStruct;

	_Edit_Delete_Selection(undoStruct);
	_Edit_Insert_Char(undoStruct, Cast(TCHAR, wParam));
	m_undostack->push(undoStruct);
	Update(TRUE);
	return 0;
}

LRESULT CTextView::OnSetFocus(WPARAM wParam, LPARAM lParam)
{
	Update(FALSE);
	return 0;
}

LRESULT CTextView::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
	_StopTimer();
	::DestroyCaret();
	m_caretShown = FALSE;
	m_caretCreated = FALSE;
	SendNotifyMessage(NM_EDITORLOSTFOCUS, (LPARAM) this);
	return 0;
}

LRESULT CTextView::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == 1000 )
	{
		switch ( m_timerDirection )
		{
		case TTimerDirectionNorth:
			if ( m_scrollPos.y == 0 )
			{
				_StopTimer();
				m_timerDirection = TTimerDirectionNone;
				break;
			}
			if ( m_chars->has_Selection() )
			{
				POINT lb = (m_cursorPos == m_chars->get_UpperBound())?(m_chars->get_LowerBound()):(m_chars->get_UpperBound());

				m_chars->get_PrevLineIndex(lb);
				m_chars->expand_Selection(lb, m_cursorPos);
				Update(FALSE);
			}
			else
			{
				POINT lb = m_cursorPos;
					
				m_chars->get_PrevLineIndex(lb);
				m_chars->set_Selection(lb, m_cursorPos);
				Update(FALSE);
			}
			break;
		case TTimerDirectionSouth:
			{
				RECT clientrect;
				Gdiplus::RectF clientrectF;

				GetClientRect(&clientrect);
				Convert2RectF(&clientrectF, &clientrect);
				if ( m_chars->isPosInClientRect(clientrectF, m_scrollPos, m_chars->get_EndPoint()) )
				{
					_StopTimer();
					m_timerDirection = TTimerDirectionNone;
					break;
				}
				if ( m_chars->has_Selection() )
				{
					POINT ub = (m_cursorPos == m_chars->get_LowerBound())?(m_chars->get_UpperBound()):(m_chars->get_LowerBound());

					m_chars->get_NextLineIndex(ub);
					m_chars->expand_Selection(ub, m_cursorPos);
					Update(FALSE);
				}
				else
				{
					POINT ub = m_cursorPos;
						
					m_chars->get_NextLineIndex(ub);
					m_chars->set_Selection(m_cursorPos, ub);
					Update(FALSE);
				}
			}
			break;
		case TTimerDirectionEast:
			if ( m_chars->has_Selection() )
			{
				POINT ub = (m_cursorPos == m_chars->get_LowerBound())?(m_chars->get_UpperBound()):(m_chars->get_LowerBound());
				POINT len = { m_chars->get_LineLength(ub.y), ub.y };

				if ( ub == len )
				{
					_StopTimer();
					m_timerDirection = TTimerDirectionNone;
					break;
				}
				m_chars->get_NextChar(ub);
				m_chars->expand_Selection(ub, m_cursorPos);
				Update(FALSE);
			}
			else
			{
				m_chars->set_Selection(m_cursorPos, m_cursorPos);
				Update(FALSE);
			}
			break;
		case TTimerDirectionWest:
			if ( m_scrollPos.x == 0 )
			{
				_StopTimer();
				m_timerDirection = TTimerDirectionNone;
				break;
			}
			if ( m_chars->has_Selection() )
			{
				POINT lb = (m_cursorPos == m_chars->get_UpperBound())?(m_chars->get_LowerBound()):(m_chars->get_UpperBound());
				POINT null = { 0, 0 };

				if ( null < lb )
				{
					m_chars->get_PrevChar(lb);
					m_chars->expand_Selection(lb, m_cursorPos);
					Update(FALSE);
				}
			}
			else
			{
				m_chars->set_Selection(m_cursorPos, m_cursorPos);
				Update(FALSE);
			}
			break;
		default:
			_StopTimer();
			m_timerDirection = TTimerDirectionNone;
			break;
		}
	}
	return 0;
}

LRESULT CTextView::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	int fwKeys = GET_KEYSTATE_WPARAM(wParam);
	int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	bool bDone = false;

	if ( (!fwKeys) && zDelta )
	{
		SIZE sz = m_chars->get_fontsize();

		if ( get_VScrollVisible() )
		{
			int pos = get_VScrollOffset();

			pos -= zDelta;
			set_VScrollOffset(pos);
			m_scrollPos.y = get_VScrollOffset() / sz.cy;
			Update(FALSE);
			bDone = true;
		}
		else if ( get_HScrollVisible() )
		{
			int pos = get_HScrollOffset();

			pos -= zDelta;
			set_HScrollOffset(pos);
			m_scrollPos.x = get_HScrollOffset() / sz.cx;
			Update(FALSE);
			bDone = true;
		}
	}
	if ( !bDone )
		return DefaultWindowProc(WM_MOUSEWHEEL, wParam, lParam);
	return 0;
}

LRESULT CTextView::OnHScroll(WPARAM wParam, LPARAM lParam)
{
	if ( CControl::OnHScroll(wParam, lParam) )
	{
		SIZE sz = m_chars->get_fontsize();

		m_scrollPos.x = get_HScrollOffset() / sz.cx;
		Update(FALSE);
	}
	return 0;
}

LRESULT CTextView::OnVScroll(WPARAM wParam, LPARAM lParam)
{
	if ( CControl::OnVScroll(wParam, lParam) )
	{          
		SIZE sz = m_chars->get_fontsize();

		m_scrollPos.y = get_VScrollOffset() / sz.cy;
		Update(FALSE);
	}
	return 0;
}

LRESULT CTextView::OnSize(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CTextView::OnEditUndoCommand(WPARAM wParam, LPARAM lParam)
{
	bool b = m_undostack->get_CurrentPos() > 0;

	if ( HIWORD(wParam) > 1 )
		return b?TRUE:FALSE;
	if ( !b )
		return 0;
	m_undostack->undo();

	CTextViewUndoStruct undoStruct = m_undostack->current();
	POINT null = { 0, 0 };

	if ( null <= undoStruct.insertPosStart )
	{
		m_chars->Remove(undoStruct.insertPosStart, undoStruct.insertPosEnde);
		m_cursorPos = undoStruct.insertPosStart;
	}
	if ( null <= undoStruct.deletePosStart )
		m_cursorPos = m_chars->SetText(undoStruct.deletePosStart, undoStruct.deleteChars.GetString(), undoStruct.deleteChars.GetLength());
	Update(TRUE);
	return 0;
}

LRESULT CTextView::OnEditRedoCommand(WPARAM wParam, LPARAM lParam)
{
	bool b = m_undostack->get_CurrentPos() < m_undostack->size();

	if ( HIWORD(wParam) > 1 )
		return b?TRUE:FALSE;
	if ( !b )
		return 0;

	CTextViewUndoStruct undoStruct = m_undostack->current();
	POINT null = { 0, 0 };

	if ( null <= undoStruct.deletePosStart )
	{
		m_chars->Remove(undoStruct.deletePosStart, undoStruct.deletePosEnde);
		m_cursorPos = undoStruct.deletePosStart;
	}
	if ( null <= undoStruct.insertPosStart )
		m_cursorPos = m_chars->SetText(undoStruct.insertPosStart, undoStruct.insertChars.GetString(), undoStruct.insertChars.GetLength());
	m_undostack->redo();
	Update(TRUE);
	return 0;
}

LRESULT CTextView::OnEditCutCommand(WPARAM wParam, LPARAM lParam)
{
	int len = m_chars->get_LineCount();
	bool b = ((len > 0) && (m_chars->has_Selection()));

	if ( HIWORD(wParam) > 1 )
		return b?TRUE:FALSE;
	if ( !b )
		return 0;
	if ( !_EditCopyCommand() )
		return 0;
	_EditDeleteCommand();
	return 0;
}

LRESULT CTextView::OnEditCopyCommand(WPARAM wParam, LPARAM lParam)
{
	int len = m_chars->get_LineCount();
	bool b = ((len > 0) && (m_chars->has_Selection()));

	if ( HIWORD(wParam) > 1 )
		return b?TRUE:FALSE;
	if ( !b )
		return 0;
	_EditCopyCommand();
	return 0;
}

LRESULT CTextView::OnEditPasteCommand(WPARAM wParam, LPARAM lParam)
{
	BOOL b = ::IsClipboardFormatAvailable(CF_UNICODETEXT);

	if ( HIWORD(wParam) > 1 )
		return b;
	if ( !b )
		return 0;
	if ( !::OpenClipboard(theGuiApp->get_MainWnd()->get_handle()) ) 
        return 0; 
 
	CTextViewUndoStruct undoStruct;
    HGLOBAL hglb = ::GetClipboardData(CF_UNICODETEXT);

    if ( !hglb )
		goto _exit0;
	
	SIZE_T hglbSize = ::GlobalSize(hglb);

	if ( !hglbSize )
		goto _exit0;

    LPTSTR lptstr = (LPTSTR)(::GlobalLock(hglb));

    if ( !lptstr ) 
		goto _exit0;

	dword len0 = s_strlen(lptstr, UINT_MAX);

	_Edit_Delete_Selection(undoStruct);
	_Edit_Insert_Chars(undoStruct, lptstr, len0);
	m_undostack->push(undoStruct);
	Update(TRUE);
     GlobalUnlock(hglb); 
_exit0:
    CloseClipboard(); 
	return 0;
}

LRESULT CTextView::OnEditDeleteCommand(WPARAM wParam, LPARAM lParam)
{
	int len = m_chars->get_LineCount();
	bool b = ((len > 0) && (m_chars->has_Selection()));

	if ( HIWORD(wParam) > 1 )
		return b?TRUE:FALSE;
	if ( !b )
		return 0;
	_EditDeleteCommand();
	return 0;
}

bool CTextView::_EditCopyCommand()
{
	bool result = false;

	if ( !::OpenClipboard(theGuiApp->get_MainWnd()->get_handle()) )
        return false; 
    ::EmptyClipboard();

	POINT ub = m_chars->get_UpperBound();

	m_chars->get_NextChar(ub);
	CStringBuffer tmp = m_chars->GetText(m_chars->get_LowerBound(), ub);
	int cch = tmp.GetLength();
	HGLOBAL hglbCopy = ::GlobalAlloc(GMEM_MOVEABLE, (cch + 1) * sizeof(TCHAR));

	if ( !hglbCopy ) 
		goto _exit0;

	LPTSTR lptstrCopy = (LPTSTR)(::GlobalLock(hglbCopy));
	::MoveMemory(lptstrCopy, tmp.GetString(), cch * sizeof(TCHAR));
	lptstrCopy[cch] = (TCHAR)0;
	::GlobalUnlock(hglbCopy);
 
	::SetClipboardData(CF_UNICODETEXT, hglbCopy);

	SendNotifyMessage(NM_EDITORSELCHANGE, (LPARAM) this);
	SendNotifyMessage(NM_SELECTIONCHANGED);

	result = true;

_exit0:
	CloseClipboard(); 
	return result;
}

bool CTextView::_EditDeleteCommand()
{
	CTextViewUndoStruct undoStruct;

	_Edit_Delete_Selection(undoStruct);
	m_undostack->push(undoStruct);
	Update(TRUE);
	return true;
}

void CTextView::_Edit_Delete_Selection(CTextViewUndoStruct& undoStruct)
{
	int len = m_chars->get_LineCount();

	if ( (len > 0) && (m_chars->has_Selection()) )
	{
		POINT lb = m_chars->get_LowerBound();
		POINT ub = m_chars->get_UpperBound();

		m_chars->Normalize(lb);
		m_chars->Normalize(ub);
		undoStruct.deletePosStart = lb;
		m_chars->get_NextChar(ub);
		undoStruct.deletePosEnde = ub;
		undoStruct.deleteChars.SetString(__FILE__LINE__ m_chars->GetText(lb, ub));
		m_chars->Remove(lb, ub);
		m_cursorPos = lb;
		m_chars->reset_Selection();
	}
}

void CTextView::_Edit_Delete_Char(CTextViewUndoStruct& undoStruct)
{
	int len = m_chars->get_LineCount();

	if ( (len > 0) && (!(m_chars->has_Selection())) )
	{
		m_chars->Normalize(m_cursorPos);

		POINT start = m_cursorPos;
		POINT ende = m_cursorPos;

		m_chars->get_NextChar(ende);
		if ( start == ende )
			return;
		undoStruct.deletePosStart = start;
		undoStruct.deletePosEnde = ende;
		undoStruct.deleteChars.SetString(__FILE__LINE__ m_chars->GetText(start, ende));
		m_chars->Remove(start, ende);
	}
}

void CTextView::_Edit_Insert_Char(CTextViewUndoStruct& undoStruct, TCHAR keyChar)
{
	TCHAR buf[2];

	buf[0] = keyChar;
	buf[1] = 0;
	m_chars->Normalize(m_cursorPos);
	undoStruct.insertPosStart = m_cursorPos;
	undoStruct.insertChars.SetString(__FILE__LINE__ buf, 1);
	m_cursorPos = m_chars->SetText(m_cursorPos, buf, 1);
	undoStruct.insertPosEnde = m_cursorPos;
}

void CTextView::_Edit_Insert_Chars(CTextViewUndoStruct& undoStruct, LPCTSTR lptstr, dword len0)
{
	if ( len0 > 0 )
	{
		m_chars->Normalize(m_cursorPos);
		undoStruct.insertPosStart = m_cursorPos;
		undoStruct.insertChars.SetString(__FILE__LINE__ lptstr, len0);
		m_cursorPos = m_chars->SetText(m_cursorPos, lptstr, len0);
		undoStruct.insertPosEnde = m_cursorPos;
	}
}

BOOL CTextView::_StartTimer()
{
	if ( m_timerEnabled )
		return TRUE;
	if ( StartTimer(1000, 200) )
	{
		m_timerEnabled = TRUE;
		return TRUE;
	}
	return FALSE;
}

BOOL CTextView::_StopTimer()
{
	if ( m_timerEnabled )
	{
		if ( StopTimer(1000) )
		{
			m_timerEnabled = FALSE;
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}