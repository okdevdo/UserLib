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
#include "WS_PCH.H"
#include "URL.h"

/*  
*  RFC 1738 defines that these characters should be escaped, as well
*  any non-US-ASCII character or anything between 0x00 - 0x1F.
*/
static mbchar rfc1738_unsafe_chars[] =
{
	(mbchar) 0x3C,		/* < */
	(mbchar) 0x3E,		/* > */
	(mbchar) 0x22,		/* " */
	(mbchar) 0x23,		/* # */
#if 0				/* done in code */
	(mbchar) 0x25,		/* % */
#endif
	(mbchar) 0x7B,		/* { */
	(mbchar) 0x7D,		/* } */
	(mbchar) 0x7C,		/* | */
	(mbchar) 0x5C,		/* \ */
	(mbchar) 0x5E,		/* ^ */
	(mbchar) 0x7E,		/* ~ */
	(mbchar) 0x5B,		/* [ */
	(mbchar) 0x5D,		/* ] */
	(mbchar) 0x60,		/* ` */
	(mbchar) 0x27,		/* ' */
	(mbchar) 0x20		/* space */
};

static mbchar rfc1738_reserved_chars[] =
{
	(mbchar) 0x3b,		/* ; */
	(mbchar) 0x2f,		/* / */
	(mbchar) 0x3f,		/* ? */
	(mbchar) 0x3a,		/* : */
	(mbchar) 0x40,		/* @ */
	(mbchar) 0x3d,		/* = */
	(mbchar) 0x26		/* & */
};

CUrl::CUrl(CConstPointer url, int chLen):
_protocol(),
_server(),
_resource(),
_queries(),
_fragment()
{
	ParseUrl(url, chLen);
}

CUrl::CUrl(ConstRef(CStringBuffer) url):
_protocol(),
_server(),
_resource(),
_queries(),
_fragment()
{
	ParseUrl(url);
}

CUrl::~CUrl(void)
{
}

void CUrl::clear()
{
	_protocol.Clear();
	_server.Clear();
	_resource.Clear();
	_queries.clear();
	_fragment.Clear();
}

CStringBuffer CUrl::get_Url() const
{
	CStringBuffer sResult;

	if (!(_server.IsEmpty()))
	{
		sResult.AppendString(_protocol);
		sResult.AppendString(_T("://"));
		sResult.AppendString(_server);
	}
	sResult.AppendString(_resource);
	if (_queries.count() > 0)
	{
		QueryDataList::iterator it(_queries.begin());
		bool bFirst = true;

		sResult.AppendString(_T("?"));
		while (*it)
		{
			if (bFirst)
				bFirst = false;
			else
				sResult.AppendString(_T("&"));
			sResult.AppendString((*it)->item.Key);
			sResult.AppendString(_T("="));
			sResult.AppendString((*it)->item.Value);
			++it;
		}
	}
	if (!(_fragment.IsEmpty()))
	{
		sResult.AppendString(_T("#"));
		sResult.AppendString(_fragment);
	}
	return sResult;
}

bool CUrl::set_Url(CConstPointer url, int chLen) 
{
	clear();
	return ParseUrl(url, chLen);
}

bool CUrl::set_Url(ConstRef(CStringBuffer) url) 
{
	clear();
	return ParseUrl(url);
}

bool CUrl::ParseUrl(CConstPointer url, int chLen)
{
	CStringBuffer vUrl(__FILE__LINE__ url, chLen);

	return ParseUrl(vUrl);
}

bool CUrl::ParseUrl(ConstRef(CStringBuffer) url)
{
	if (url.IsEmpty())
		return false;
	CStringConstIterator it = url.Begin();
	CStringConstIterator it2;
	CStringConstIterator it3;
	CStringConstIterator it4;
	CStringBuffer tmp;

	it.Find(_T("://"));
	if ( it.IsEnd() ) 
	{
		_protocol.SetString(__FILE__LINE__ _T("http"));
		it = url.Begin();
	}
	else
	{
		url.SubString(0, it.GetDistance(), _protocol);
		it += 3;
	}
	it2 = it;
	it3 = it;
	it4 = it;
	it2.Find(_T('/'));
	it3.Find(_T('?'));
	it4.Find(_T('#'));
	if ( it2.IsEnd() && it3.IsEnd() && it4.IsEnd() )
	{
		_resource.SetString(__FILE__LINE__ _T("/"));
		url.SubString(it.GetDistance(), it.GetLength(), _server);
		return (!(_server.IsEmpty()));
	}
	if (it3.IsEnd() && it4.IsEnd())
	{
		url.SubString(it.GetDistance(), it.GetLength() - it2.GetLength(), _server);
		url.SubString(it2.GetDistance(), it2.GetLength(), _resource);
		return (!(_server.IsEmpty()));
	}
	if (it2.IsEnd() && it4.IsEnd())
	{
		_resource.SetString(__FILE__LINE__ _T("/"));
		url.SubString(it.GetDistance(), it.GetLength() - it3.GetLength(), _server);
		url.SubString(it3.GetDistance() + 1, it3.GetLength() - 1, tmp);
		_queries.split(tmp);
		return (!(_server.IsEmpty()));
	}
	if (it2.IsEnd() && it3.IsEnd())
	{
		_resource.SetString(__FILE__LINE__ _T("/"));
		url.SubString(it.GetDistance(), it.GetLength() - it4.GetLength(), _server);
		url.SubString(it4.GetDistance() + 1, it4.GetLength() - 1, _fragment);
		return (!(_server.IsEmpty()));
	}
	if (it2.IsEnd())
	{
		if (it3.GetCurrent() < it4.GetCurrent())
		{
			_resource.SetString(__FILE__LINE__ _T("/"));
			url.SubString(it.GetDistance(), it.GetLength() - it3.GetLength(), _server);
			url.SubString(it3.GetDistance() + 1, it3.GetLength() - it4.GetLength() - 1, tmp);
			url.SubString(it4.GetDistance() + 1, it4.GetLength() - 1, _fragment);
			_queries.split(tmp);
			return (!(_server.IsEmpty()));
		}
		return false;
	}
	if (it3.IsEnd())
	{
		if (it2.GetCurrent() < it4.GetCurrent())
		{
			url.SubString(it.GetDistance(), it.GetLength() - it2.GetLength(), _server);
			url.SubString(it2.GetDistance(), it2.GetLength() - it4.GetLength(), _resource);
			url.SubString(it4.GetDistance() + 1, it4.GetLength() - 1, _fragment);
			return (!(_server.IsEmpty()));
		}
		return false;
	}
	if (it4.IsEnd())
	{
		if (it2.GetCurrent() < it3.GetCurrent())
		{
			url.SubString(it.GetDistance(), it.GetLength() - it2.GetLength(), _server);
			url.SubString(it2.GetDistance(), it2.GetLength() - it3.GetLength(), _resource);
			url.SubString(it3.GetDistance() + 1, it3.GetLength() - 1, tmp);
			_queries.split(tmp);
			return (!(_server.IsEmpty()));
		}
		_resource.SetString(__FILE__LINE__ _T("/"));
		url.SubString(it.GetDistance(), it.GetLength() - it3.GetLength(), _server);
		url.SubString(it3.GetDistance() + 1, it3.GetLength() - it2.GetLength() - 1, tmp);
		_queries.split(tmp);
		return (!(_server.IsEmpty()));
	}
	if ((it2.GetCurrent() < it3.GetCurrent()) && (it3.GetCurrent() < it4.GetCurrent()))
	{
		url.SubString(it.GetDistance(), it.GetLength() - it2.GetLength(), _server);
		url.SubString(it2.GetDistance(), it2.GetLength() - it3.GetLength(), _resource);
		url.SubString(it3.GetDistance() + 1, it3.GetLength() - it4.GetLength() - 1, tmp);
		_queries.split(tmp);
		url.SubString(it4.GetDistance() + 1, it4.GetLength() - 1, _fragment);
		return (!(_server.IsEmpty()));
	}
	return false;
}

CStringBuffer CUrl::get_Encoded(CUrl::EncodeType encodetype)
{
	CStringBuffer _url(get_Url());
	CStringBuffer buf;
	CConstPointer p;
	CPointer q;
	bool do_escape;

	buf.SetSize(__FILE__LINE__ (_url.GetLength() * 3) + 1);
	for ( p = _url.GetString(), q = CastMutable(CPointer, buf.GetString()); *p != _T('\0'); p++, q++) 
	{
		do_escape = false;

		/* RFC 1738 defines these chars as unsafe */
		for ( word i = 0; i < sizeof(rfc1738_unsafe_chars); i++ ) 
		{
			if ( *p == rfc1738_unsafe_chars[i] ) 
			{
				do_escape = true;
				break;
			}
		}
		/* Handle % separately */
		if ( (encodetype >= 0) && (*p == _T('%')) )
			do_escape = true;

		/* RFC 1738 defines these chars as reserved */
		for ( word i = 0; (i < sizeof(rfc1738_reserved_chars)) && (encodetype > 0); i++ ) 
		{
			if ( *p == rfc1738_reserved_chars[i] ) 
			{
				do_escape = true;
				break;
			}
		}

		/* RFC 1738 says any control chars (0x00-0x1F) are encoded */
		if ( (word)(*p) <= 0x001F )
			do_escape = true;

		/* RFC 1738 says 0x7f is encoded */
		if ( (word)(*p) == 0x007F )
			do_escape = true;

		/* RFC 1738 says any non-US-ASCII are encoded */
		if ( (word)(*p) >= 0x0080 )
			do_escape = true;

		/* Do the triplet encoding, or just copy the char */
		if ( do_escape )
		{
			CStringBuffer tmp;
			CConstPointer t;

			tmp.FormatString(__FILE__LINE__ _T("%%%02x"), (word)*p);
			t = tmp.GetString();
			*q++ = *t++;
			*q++ = *t++;
			*q = *t;
		} else
			*q = *p;
	}
	*q = _T('\0');
	return (buf);
}

/*
*  rfc1738_unescape() - Converts escaped characters (%xy numbers) in 
*  given the string.  %% is a %. %ab is the 8-bit hexadecimal number "ab"
*/
void CUrl::set_Encoded(CConstPointer encodedstr)
{
	CStringBuffer _url(__FILE__LINE__ encodedstr);
	mbchar hexnum[3];
	dword x;
	CConstPointer p = encodedstr;
	CPointer q;

	q = CastMutable(CPointer, _url.GetString());
	for ( ; *p != _T('\0'); ++p, ++q ) 
	{
		*q = *p;
		if ( *p != _T('%') )
			continue;
		++p;
		if ( *p == _T('%') )
			continue;
		if ( (*p) && (*(p + 1)) ) 
		{
			if ( ((*p) == _T('0')) && ((*(p + 1)) == _T('0')) )
			{
				++p;
				continue;
			}
			hexnum[0] = *p++;
			hexnum[1] = *p;
			hexnum[2] = '\0';

			CStringBuffer tmp(__FILE__LINE__ hexnum);

			if ( tmp.ScanString(_T("%x"), &x) > 0 )
				*q = (mbchar) (0x000000ff & x);
		}
	}
	*q = _T('\0');
	ParseUrl(_url);
}
