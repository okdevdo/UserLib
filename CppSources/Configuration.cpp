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
#include "Application.h"
#include "Configuration.h"
#include "DirectoryIterator.h"
#include "File.h"

/* CAbstractConfiguration */
CAbstractConfiguration::CAbstractConfiguration(void):
    m_writable(false),
	m_modified(false)
{
}

CAbstractConfiguration::CAbstractConfiguration(ConstRef(CStringLiteral) _prefix, WBool _iswritable):
    m_prefix(__FILE__LINE__ _prefix),
    m_writable(_iswritable),
	m_modified(false)
{
}

CAbstractConfiguration::~CAbstractConfiguration(void)
{
}

WBool CAbstractConfiguration::HasValue(ConstRef(CStringLiteral) _name)
{
	CStringBuffer tmp;

	if ( GetRawValue(_name, tmp) )
		return true;
	return false;
}

WBool CAbstractConfiguration::HasUserValue(ConstRef(CStringLiteral) _trailer)
{
	CStringBuffer appname(theApp->config()->GetValue(_T("Application.Name")));

	if (appname.IsEmpty())
		return false;

	CStringBuffer userappname(appname);

	userappname.AppendString(_T("User"));

	CStringBuffer tmpname;
	CStringBuffer tmpresult;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), userappname.GetString(), _trailer.GetString());

	if (GetRawValue(tmpname, tmpresult))
		return true;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), appname.GetString(), _trailer.GetString());

	if (GetRawValue(tmpname, tmpresult))
		return true;
	return false;
}

WBool CAbstractConfiguration::HasUserValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName)
{
	CStringBuffer appname(theApp->config()->GetValue(_T("Application.Name"), _defaultAppName));

	if (appname.IsEmpty())
		return false;

	CStringBuffer userappname(appname);

	userappname.AppendString(_T("User"));

	CStringBuffer tmpname;
	CStringBuffer tmpresult;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), userappname.GetString(), _trailer.GetString());

	if (GetRawValue(tmpname, tmpresult))
		return true;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), appname.GetString(), _trailer.GetString());

	if (GetRawValue(tmpname, tmpresult))
		return true;
	return false;
}

CStringBuffer CAbstractConfiguration::GetValue(ConstRef(CStringLiteral) _name)
{
	CStringBuffer tmp;

	GetRawValue(_name, tmp);
	return tmp;
}

CStringBuffer CAbstractConfiguration::GetValue(ConstRef(CStringLiteral) _name, ConstRef(CStringLiteral) _default)
{
	CStringBuffer tmp;

	if ( GetRawValue(_name, tmp) )
		return tmp;
	tmp.SetString(__FILE__LINE__ _default);
	return tmp;
}

CStringBuffer CAbstractConfiguration::GetUserValue(ConstRef(CStringLiteral) _trailer)
{
	CStringBuffer tmpresult;
	CStringBuffer appname(theApp->config()->GetValue(_T("Application.Name")));

	if (appname.IsEmpty())
		return tmpresult;

	CStringBuffer userappname(appname);

	userappname.AppendString(_T("User"));

	CStringBuffer tmpname;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), userappname.GetString(), _trailer.GetString());

	if (GetRawValue(tmpname, tmpresult))
		return tmpresult;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), appname.GetString(), _trailer.GetString());

	if (GetRawValue(tmpname, tmpresult))
		return tmpresult;
	return tmpresult;
}

CStringBuffer CAbstractConfiguration::GetUserValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName)
{
	CStringBuffer tmpresult;
	CStringBuffer appname(theApp->config()->GetValue(_T("Application.Name"), _defaultAppName));

	if (appname.IsEmpty())
		return tmpresult;

	CStringBuffer userappname(appname);

	userappname.AppendString(_T("User"));

	CStringBuffer tmpname;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), userappname.GetString(), _trailer.GetString());

	if (GetRawValue(tmpname, tmpresult))
		return tmpresult;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), appname.GetString(), _trailer.GetString());

	if (GetRawValue(tmpname, tmpresult))
		return tmpresult;
	return tmpresult;
}

CStringBuffer CAbstractConfiguration::GetUserValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName, ConstRef(CStringLiteral) _default)
{
	CStringBuffer tmpresult;
	CStringBuffer appname(theApp->config()->GetValue(_T("Application.Name"), _defaultAppName));

	if (appname.IsEmpty())
		return tmpresult;

	CStringBuffer userappname(appname);

	userappname.AppendString(_T("User"));

	CStringBuffer tmpname;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), userappname.GetString(), _trailer.GetString());

	if (GetRawValue(tmpname, tmpresult))
		return tmpresult;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), appname.GetString(), _trailer.GetString());

	if (GetRawValue(tmpname, tmpresult))
		return tmpresult;
	tmpresult.SetString(__FILE__LINE__ _default);
	return tmpresult;
}

void CAbstractConfiguration::SetValue(ConstRef(CStringLiteral) _name, ConstRef(CStringLiteral) _value)
{
	if ( !m_writable )
		return;

	CStringBuffer tmp(__FILE__LINE__ _value);

	SetRawValue(_name, tmp);
	m_modified = true;
}

void CAbstractConfiguration::SetValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value)
{
	if ( !m_writable )
		return;

	SetRawValue(_name, _value);
	m_modified = true;
}

dword CAbstractConfiguration::GetValues(ConstRef(CStringLiteral) _name, Ref(Values) _values)
{
	CStringBuffer tmp;

	if (!GetRawValue(_name, tmp))
		return 0;
	return _values.Split(tmp, _T(", "));
}

dword CAbstractConfiguration::GetUserValues(ConstRef(CStringLiteral) _trailer, Ref(Values) _values)
{
	CStringBuffer tmp(GetUserValue(_trailer));

	return _values.Split(tmp, _T(", "));
}

dword CAbstractConfiguration::GetUserValues(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName, Ref(Values) _values)
{
	CStringBuffer tmp(GetUserValue(_trailer, _defaultAppName));

	return _values.Split(tmp, _T(", "));
}

void CAbstractConfiguration::SetValues(ConstRef(CStringLiteral) _name, ConstRef(Values) _values)
{
	if ( !m_writable )
		return;
	CStringBuffer tmp = _values.Join(_T(", "));

	SetRawValue(_name, tmp);
	m_modified = true;
}

WBool CAbstractConfiguration::GetBoolValue(ConstRef(CStringLiteral) _name)
{
	CStringBuffer tmp;
	WBool res = false;

	if ( GetRawValue(_name, tmp) )
	{
		if ( (s_stricmp(tmp.GetString(), _T("True")) == 0)
			|| (s_stricmp(tmp.GetString(), _T("On")) == 0) )
			res = true;
		else if ( (s_stricmp(tmp.GetString(), _T("False")) == 0)
			|| (s_stricmp(tmp.GetString(), _T("Off")) == 0) )
			res = false;
	}
	return res;
}

WBool CAbstractConfiguration::GetBoolValue(ConstRef(CStringLiteral) _name, WBool _default)
{
	CStringBuffer tmp;
	WBool res = false;

	if ( GetRawValue(_name, tmp) )
	{
		if ( (s_stricmp(tmp.GetString(), _T("True")) == 0)
			|| (s_stricmp(tmp.GetString(), _T("On")) == 0) )
			res = true;
		else if ( (s_stricmp(tmp.GetString(), _T("False")) == 0)
			|| (s_stricmp(tmp.GetString(), _T("Off")) == 0) )
			res = false;
		else
			res = _default;
		return res;
	}
	return _default;
}

WBool CAbstractConfiguration::GetUserBoolValue(ConstRef(CStringLiteral) _trailer)
{
	CStringBuffer appname(theApp->config()->GetValue(_T("Application.Name")));

	if (appname.IsEmpty())
		return false;

	CStringBuffer userappname(appname);

	userappname.AppendString(_T("User"));

	CStringBuffer tmpname;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), userappname.GetString(), _trailer.GetString());

	if (GetBoolValue(tmpname))
		return true;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), appname.GetString(), _trailer.GetString());

	if (GetBoolValue(tmpname))
		return true;
	return false;
}

WBool CAbstractConfiguration::GetUserBoolValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName)
{
	CStringBuffer appname(theApp->config()->GetValue(_T("Application.Name"), _defaultAppName));

	if (appname.IsEmpty())
		return false;

	CStringBuffer userappname(appname);

	userappname.AppendString(_T("User"));

	CStringBuffer tmpname;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), userappname.GetString(), _trailer.GetString());

	if (GetBoolValue(tmpname))
		return true;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), appname.GetString(), _trailer.GetString());

	if (GetBoolValue(tmpname))
		return true;
	return false;
}

WBool CAbstractConfiguration::GetUserBoolValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName, WBool _default)
{
	CStringBuffer appname(theApp->config()->GetValue(_T("Application.Name"), _defaultAppName));

	if (appname.IsEmpty())
		return false;

	CStringBuffer userappname(appname);

	userappname.AppendString(_T("User"));

	CStringBuffer tmpname;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), userappname.GetString(), _trailer.GetString());

	if (GetBoolValue(tmpname))
		return true;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), appname.GetString(), _trailer.GetString());

	if (GetBoolValue(tmpname))
		return true;
	return _default;
}

void CAbstractConfiguration::SetBoolValue(ConstRef(CStringLiteral) _name, WBool _value)
{
	if ( !m_writable )
		return;

	CStringBuffer tmp;

	if ( _value )
		tmp.SetString(__FILE__LINE__ _T("True"));
	else
		tmp.SetString(__FILE__LINE__ _T("False"));
	SetRawValue(_name, tmp);
	m_modified = true;
}

WLong CAbstractConfiguration::GetLongValue(ConstRef(CStringLiteral) _name)
{
	CStringBuffer tmp;
	WLong res = -1L;

	if ( GetRawValue(_name, tmp) )
		tmp.ScanString(_T("%ld"), &res);
	return res;
}

WLong CAbstractConfiguration::GetLongValue(ConstRef(CStringLiteral) _name, WLong _default)
{
	CStringBuffer tmp;
	WLong res;

	if ( GetRawValue(_name, tmp) )
	{
		if ( tmp.ScanString(_T("%ld"), &res) > 0)
			return res;
	}
	return _default;
}

WLong CAbstractConfiguration::GetUserLongValue(ConstRef(CStringLiteral) _trailer)
{
	CStringBuffer appname(theApp->config()->GetValue(_T("Application.Name")));
	WLong result;

	if (appname.IsEmpty())
		return -1L;

	CStringBuffer userappname(appname);

	userappname.AppendString(_T("User"));

	CStringBuffer tmpname;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), userappname.GetString(), _trailer.GetString());

	result = GetLongValue(tmpname);
	if (result >= 0)
		return result;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), appname.GetString(), _trailer.GetString());

	result = GetLongValue(tmpname);
	if (result >= 0)
		return result;
	return -1L;
}

WLong CAbstractConfiguration::GetUserLongValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName)
{
	CStringBuffer appname(theApp->config()->GetValue(_T("Application.Name"), _defaultAppName));
	WLong result;

	if (appname.IsEmpty())
		return -1L;

	CStringBuffer userappname(appname);

	userappname.AppendString(_T("User"));

	CStringBuffer tmpname;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), userappname.GetString(), _trailer.GetString());

	result = GetLongValue(tmpname);
	if (result >= 0)
		return result;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), appname.GetString(), _trailer.GetString());

	result = GetLongValue(tmpname);
	if (result >= 0)
		return result;
	return -1L;
}

WLong CAbstractConfiguration::GetUserLongValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName, WLong _default)
{
	CStringBuffer appname(theApp->config()->GetValue(_T("Application.Name"), _defaultAppName));
	WLong result;

	if (appname.IsEmpty())
		return -1L;

	CStringBuffer userappname(appname);

	userappname.AppendString(_T("User"));

	CStringBuffer tmpname;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), userappname.GetString(), _trailer.GetString());

	result = GetLongValue(tmpname);
	if (result >= 0)
		return result;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), appname.GetString(), _trailer.GetString());

	result = GetLongValue(tmpname);
	if (result >= 0)
		return result;
	return _default;
}

void CAbstractConfiguration::SetLongValue(ConstRef(CStringLiteral) _name, WLong _value)
{
	if ( !m_writable )
		return;

	CStringBuffer tmp;

	tmp.FormatString(__FILE__LINE__ _T("%ld"), _value);
	SetRawValue(_name, tmp);
	m_modified = true;
}

void CAbstractConfiguration::GetLongValues(ConstRef(CStringLiteral) _name, WLong _cnt, ...)
{
    va_list argList;

	va_start(argList, _cnt);
	GetLongValues(_name, _cnt, argList);
	va_end(argList);
}

WLong CAbstractConfiguration::GetLongValues(ConstRef(CStringLiteral) _name, WLong _cnt, va_list argList)
{
	Values values(__FILE__LINE__ 16, 16);
	CStringBuffer tmp;
	WLong valcnt;

	valcnt = GetValues(_name, values);
	if (valcnt == 0)
		return 0;
	for ( WLong ix = 0; ix < Max(valcnt, _cnt); ++ix )
	{
		if ( (ix < _cnt) && (ix < valcnt) )
		{
			tmp = *(values.Index(ix));
			tmp.ScanString(_T("%ld"), va_arg(argList, WLong*));
		}
		else if ( ix < _cnt )
			*(va_arg(argList, WLong*)) = 0;
		else
			return ix;
	}
	return 0;
}

void CAbstractConfiguration::GetUserLongValues(ConstRef(CStringLiteral) _trailer, WLong _cnt, ...)
{
	CStringBuffer appname(theApp->config()->GetValue(_T("Application.Name")));
	WLong result;
    va_list argList;

	if (appname.IsEmpty())
		return;
	va_start(argList, _cnt);

	CStringBuffer userappname(appname);

	userappname.AppendString(_T("User"));

	CStringBuffer tmpname;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), userappname.GetString(), _trailer.GetString());

	result = GetLongValues(tmpname, _cnt, argList);
	if (result > 0)
	{
		va_end(argList);
		return;
	}

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), appname.GetString(), _trailer.GetString());

	result = GetLongValues(tmpname, _cnt, argList);
	if (result > 0)
	{
		va_end(argList);
		return;
	}
	for (WLong ix = 0; ix < _cnt; ++ix)
		*(va_arg(argList, WLong*)) = 0;
	va_end(argList);
}

void CAbstractConfiguration::GetUserLongValues(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName, WLong _cnt, ...)
{
	CStringBuffer appname(theApp->config()->GetValue(_T("Application.Name"), _defaultAppName));
	WLong result;
	va_list argList;

	if (appname.IsEmpty())
		return;
	va_start(argList, _cnt);

	CStringBuffer userappname(appname);

	userappname.AppendString(_T("User"));

	CStringBuffer tmpname;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), userappname.GetString(), _trailer.GetString());

	result = GetLongValues(tmpname, _cnt, argList);
	if (result > 0)
	{
		va_end(argList);
		return;
	}

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), appname.GetString(), _trailer.GetString());

	result = GetLongValues(tmpname, _cnt, argList);
	if (result > 0)
	{
		va_end(argList);
		return;
	}
	for (WLong ix = 0; ix < _cnt; ++ix)
		*(va_arg(argList, WLong*)) = 0;
	va_end(argList);
}

void CAbstractConfiguration::SetLongValues(ConstRef(CStringLiteral) _name, WLong _cnt, ...)
{
	if ( !m_writable )
		return;

	CStringBuffer tmp;
	CStringBuffer tmp1;
	bool bFirst = true;
    va_list argList;

	va_start(argList, _cnt);
	while ( _cnt > 0 )
	{
		if ( bFirst )
			bFirst = false;
		else
			tmp += _T(", ");
		tmp1.FormatString(__FILE__LINE__ _T("%ld"), va_arg(argList, WLong));
		tmp += tmp1;
		--_cnt;
	}
	va_end(argList);
	SetRawValue(_name, tmp);
	m_modified = true;
}

WDouble CAbstractConfiguration::GetDoubleValue(ConstRef(CStringLiteral) _name)
{
	CStringBuffer tmp;
	WDouble res = -1.0;

	if ( GetRawValue(_name, tmp) )
		tmp.ScanString(_T("%lf"), &res);
	return res;
}

WDouble CAbstractConfiguration::GetDoubleValue(ConstRef(CStringLiteral) _name, WDouble _default)
{
	CStringBuffer tmp;
	WDouble res;

	if ( GetRawValue(_name, tmp) )
	{
		if ( tmp.ScanString(_T("%lf"), &res) > 0 )
			return res;
	}
	return _default;
}

WDouble CAbstractConfiguration::GetUserDoubleValue(ConstRef(CStringLiteral) _trailer)
{
	CStringBuffer appname(theApp->config()->GetValue(_T("Application.Name")));
	WDouble result;

	if (appname.IsEmpty())
		return -1.0;

	CStringBuffer userappname(appname);

	userappname.AppendString(_T("User"));

	CStringBuffer tmpname;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), userappname.GetString(), _trailer.GetString());

	result = GetDoubleValue(tmpname);
	if (result >= 0.0)
		return result;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), appname.GetString(), _trailer.GetString());

	result = GetDoubleValue(tmpname);
	if (result >= 0.0)
		return result;
	return -1.0;
}

WDouble CAbstractConfiguration::GetUserDoubleValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName)
{
	CStringBuffer appname(theApp->config()->GetValue(_T("Application.Name"), _defaultAppName));
	WDouble result;

	if (appname.IsEmpty())
		return -1.0;

	CStringBuffer userappname(appname);

	userappname.AppendString(_T("User"));

	CStringBuffer tmpname;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), userappname.GetString(), _trailer.GetString());

	result = GetDoubleValue(tmpname);
	if (result >= 0.0)
		return result;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), appname.GetString(), _trailer.GetString());

	result = GetDoubleValue(tmpname);
	if (result >= 0.0)
		return result;
	return -1.0;
}

WDouble CAbstractConfiguration::GetUserDoubleValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName, WDouble _default)
{
	CStringBuffer appname(theApp->config()->GetValue(_T("Application.Name"), _defaultAppName));
	WDouble result;

	if (appname.IsEmpty())
		return -1.0;

	CStringBuffer userappname(appname);

	userappname.AppendString(_T("User"));

	CStringBuffer tmpname;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), userappname.GetString(), _trailer.GetString());

	result = GetDoubleValue(tmpname);
	if (result >= 0.0)
		return result;

	tmpname.FormatString(__FILE__LINE__ _T("%s.%s"), appname.GetString(), _trailer.GetString());

	result = GetDoubleValue(tmpname);
	if (result >= 0.0)
		return result;
	return _default;
}

void CAbstractConfiguration::SetDoubleValue(ConstRef(CStringLiteral) _name, WDouble _value)
{
	if ( !m_writable )
		return;

	CStringBuffer tmp;

	tmp.FormatString(__FILE__LINE__ _T("%lf"), _value);
	SetRawValue(_name, tmp);
	m_modified = true;
}

WBool CAbstractConfiguration::GetRawValue(ConstRef(CStringLiteral) _name, Ref(CStringBuffer) _value)
{
	return false;
}

WBool CAbstractConfiguration::SetRawValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value)
{
	return false;
}

WBool CAbstractConfiguration::Enumerate(ConstRef(CStringLiteral) _prefix, Ref(Keys) _keys)
{
	return false;
}

/* CMapConfiguration */
CMapConfiguration::CMapConfiguration(DECL_FILE_LINE0):
	m_mapItems(ARGS_FILE_LINE0)
{
}

CMapConfiguration::CMapConfiguration(DECL_FILE_LINE ConstRef(CStringLiteral) _prefix, WBool _iswritable):
    CAbstractConfiguration(_prefix, _iswritable),
	m_mapItems(ARGS_FILE_LINE0)
{
}

CMapConfiguration::~CMapConfiguration(void)
{
}

WBool CMapConfiguration::GetRawValue(ConstRef(CStringLiteral) _name, Ref(CStringBuffer) _value)
{
	MapItem item(CStringBuffer(__FILE__LINE__ _name));
	MapItems::Iterator it = m_mapItems.FindSorted(&item);

	if (!(m_mapItems.MatchSorted(it, &item)))
		return false;
	_value = (*it)->get_Value();
	return true;
}

WBool CMapConfiguration::SetRawValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value)
{
	if ( !m_writable )
		return false;

	Ptr(MapItem) item = OK_NEW_OPERATOR MapItem(CStringBuffer(__FILE__LINE__ _name), _value);
	MapItems::Iterator it = m_mapItems.FindSorted(item);

	if (!(m_mapItems.MatchSorted(it, item)))
	{
		if (_value.IsEmpty())
		{
			item->release();
			return true;
		}
		m_mapItems.InsertSorted(item);
		return true;
	}
	if ( _value.IsEmpty() )
	{
		m_mapItems.RemoveSorted(item);
		return true;
	}
	m_mapItems.SetData(it, item);
	return true;
}

WBool CMapConfiguration::Enumerate(ConstRef(CStringLiteral) _prefix, Ref(Keys) _keys)
{
	if ( _prefix.Compare(m_prefix, 0, CStringLiteral::cIgnoreCase) != 0 )
		return false;
	MapItems::Iterator it = m_mapItems.Begin();

	while ( it )
	{
		_keys.Append((*it)->get_Name());
		++it;
	}
	return (_keys.Count() > 0);
}

WBool CMapConfiguration::AddValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value)
{
	CStringBuffer tmp(CStringBuffer(__FILE__LINE__ _name));

	if ( !(m_prefix.IsEmpty()) )
	{
		tmp.PrependString(_T("."));
		tmp.PrependString(m_prefix);
	}

	Ptr(MapItem) item = OK_NEW_OPERATOR MapItem(tmp, _value);
	MapItems::Iterator it = m_mapItems.FindSorted(item);

	if (!(m_mapItems.MatchSorted(it, item)))
	{
		m_mapItems.InsertSorted(item);
		return true;
	}
	m_mapItems.SetData(it, item);
	return true;
}

/* CFileConfiguration */
CFileConfiguration::CFileConfiguration(DECL_FILE_LINE0):
    CMapConfiguration(ARGS_FILE_LINE0),
	m_path()
{
}

CFileConfiguration::CFileConfiguration(DECL_FILE_LINE ConstRef(CFilePath) _path, WBool bWritable):
    CMapConfiguration(ARGS_FILE_LINE _path.get_PureBasename(), bWritable),
	m_path(_path)
{
	LoadConfig();
}

CFileConfiguration::~CFileConfiguration(void)
{
	SaveConfig();
}

WBool CFileConfiguration::LoadConfig()
{
	bool fExist = false;

	try
	{
		fExist = CDirectoryIterator::FileExists(m_path);
	}
	catch ( CDirectoryIteratorException* )
	{
		fExist = false;
	}
	if ( !fExist )
		return false;

	CStringBuffer sBuf;
	CDiskFile file;

	try
	{
		CByteBuffer filebuf;
		sqword filesize;

		file.Open(m_path);
		filesize = file.GetSize();
		if ( filesize > 0LL )
		{
			filebuf.set_BufferSize(__FILE__LINE__ Cast(dword,filesize));
			file.Read(filebuf);
		}
		else
			fExist = false;
		file.Close();

		sBuf.convertFromUTF8(filebuf);
	}
	catch ( CFileException* )
	{
		file.Close();
		fExist = false;
	}
	if ( !fExist )
		return false;

	CStringConstIterator it(sBuf);
	CStringBuffer name;
	CStringBuffer value;
	WInt state = 0;

	while ( !(it.IsEnd()) )
	{
		switch ( it[0] )
		{
		case _T('#'):
			switch (state)
			{
			case 0:
				name.Clear();
				break;
			case 1:
				name.Trim();
				value.Trim();
				CMapConfiguration::AddValue(name, value);
				name.Clear();
				value.Clear();
				break;
			default:
				break;
			}
			state = 2;
			break;
		case _T(':'):
		case _T('='):
			switch (state)
			{
			case 0:
				state = 1;
				break;
			case 1:
				value += it[0];
				break;
			default:
				break;
			}
			break;
		case _T('\n'):
		case _T('\r'):
			switch (state)
			{
			case 0:
				name.Clear();
				break;
			case 1:
				name.Trim();
				value.Trim();
				CMapConfiguration::AddValue(name, value);
				name.Clear();
				value.Clear();
				break;
			default:
				break;
			}
			state = 0;
			break;
		default:
			switch ( state )
			{
			case 0:
				name += it[0];
				break;
			case 1:
				value += it[0];
				break;
			default:
				break;
			}
			break;
		}
		++it;
	}
	if ( state == 1 )
	{
		name.Trim();
		value.Trim();
		CMapConfiguration::AddValue(name, value);
	}
	return true;
}

WBool CFileConfiguration::SaveConfig()
{
	if ( (!m_writable) || (!m_modified) )
		return false;

	CStringBuffer buf;
	CStringBuffer name;
	MapItems::Iterator it = m_mapItems.Begin();
	dword plen = m_prefix.GetLength();

	if ( plen )
		++plen;
	while ( it )
	{
		name = (*it)->get_Name();
		if ( plen )
			name.DeleteString(0, plen);
		buf += name;
		buf += _T(" = ");
		buf += (*it)->get_Value();
		buf += _T("\r\n");
		++it;
	}
	if ( buf.IsEmpty() )
		return false;

	CDiskFile file;

	try
	{
		CByteBuffer filebuf;

		buf.convertToUTF8(filebuf);
		file.Create(m_path, true, CFile::UTF_8_Encoding);
		file.Write(filebuf);
		file.Close();
	}
	catch ( CFileException* )
	{
		file.Close();
		return false;
	}
	return true;
}

/* CSystemConfiguration */
CSystemConfiguration::CSystemConfiguration(void):
    CAbstractConfiguration(_T("System"))
{
}

CSystemConfiguration::~CSystemConfiguration(void)
{
}

WBool CSystemConfiguration::GetRawValue(ConstRef(CStringLiteral) _name, Ref(CStringBuffer) _value)
{
	if ( _name.Compare(_T("System.Environment."), 19, CStringLiteral::cIgnoreCase + CStringLiteral::cLimited) == 0 )
	{
		CStringBuffer name(__FILE__LINE__ _name);

		name.DeleteString(0, 19);
#ifdef __MINGW32_MAJOR_VERSION
        CPointer buf = _tgetenv(name);
		dword bufsz;

        if ( PtrCheck(buf) )
            return false;
		bufsz = s_strlen(buf, 32768) + 1;
        _value.SetSize(__FILE__LINE__ bufsz);
        s_strcpy(CastMutable(CPointer, _value.GetString()), bufsz, buf);
#elif _MSC_VER
		size_t res;
        
		if ( _tgetenv_s(&res, NULL, 0, name) )
			return false;
		if ( res == 0 )
			return false;
		_value.SetSize(__FILE__LINE__ Castdword(res + 1));
		if ( _tgetenv_s(&res, CastMutable(CPointer, _value.GetString()), res, name) )
			return false;
#else
        CPointer buf = getenv(name);
		dword bufsz;

        if ( PtrCheck(buf) )
            return false;
		bufsz = s_strlen(buf, 32768) + 1;
        _value.SetSize(__FILE__LINE__ bufsz);
        s_strcpy(CastMutable(CPointer, _value.GetString()), bufsz, buf);
#endif
		return true;
	}
	return false;
}

WBool CSystemConfiguration::SetRawValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value)
{
	if ( _name.Compare(_T("System.Environment."), 19, CStringLiteral::cIgnoreCase + CStringLiteral::cLimited) == 0 )
	{
		CStringBuffer name(__FILE__LINE__ _name);

		name.DeleteString(0, 19);
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
		name.AppendString(_T("="));
        name.AppendString(_value.GetString());
        if ( _tputenv(name.GetString()) < 0 )
#endif
#ifdef OK_SYS_UNIX
        name.AppendString(_T("="));
        name.AppendString(_value.GetString());
		
		char * buf = strdup(name.GetString());
		
		if ( PtrCheck(buf) )
			return false;
        if ( putenv(buf) )
#endif
#endif
#ifdef OK_COMP_MSC
		if (_tputenv_s(name, _value))
#endif
			return false;
		return true;
	}
	return false;
}

WBool CSystemConfiguration::Enumerate(ConstRef(CStringLiteral) _prefix, Ref(Keys) _keys)
{
	if ( _prefix.Compare(m_prefix, 0, CStringLiteral::cIgnoreCase) != 0 )
		return false;
#ifdef OK_SYS_WINDOWS
	_keys.Append(CStringBuffer(__FILE__LINE__ _T("System.Environment.ALLUSERSPROFILE")));
	_keys.Append(CStringBuffer(__FILE__LINE__ _T("System.Environment.APPDATA")));
	_keys.Append(CStringBuffer(__FILE__LINE__ _T("System.Environment.INCLUDE")));
	_keys.Append(CStringBuffer(__FILE__LINE__ _T("System.Environment.LIB")));
	_keys.Append(CStringBuffer(__FILE__LINE__ _T("System.Environment.LOCALAPPDATA")));
	_keys.Append(CStringBuffer(__FILE__LINE__ _T("System.Environment.PATH")));
	_keys.Append(CStringBuffer(__FILE__LINE__ _T("System.Environment.PROGRAMDATA")));
	_keys.Append(CStringBuffer(__FILE__LINE__ _T("System.Environment.USERPROFILE")));
	_keys.Append(CStringBuffer(__FILE__LINE__ _T("System.Environment.TMP")));
	_keys.Append(CStringBuffer(__FILE__LINE__ _T("System.Environment.TEMP")));
#endif
#ifdef OK_SYS_UNIX
	_keys.Append(CStringBuffer(__FILE__LINE__ _T("System.Environment.HOME")));
	_keys.Append(CStringBuffer(__FILE__LINE__ _T("System.Environment.PATH")));
	_keys.Append(CStringBuffer(__FILE__LINE__ _T("System.Environment.TMP")));
	_keys.Append(CStringBuffer(__FILE__LINE__ _T("System.Environment.TEMP")));
#endif
	return true;
}

/* CCApplicationConfiguration */
CApplicationConfiguration::CApplicationConfiguration(void):
    CAbstractConfiguration(),
	m_path()
{
}

CApplicationConfiguration::CApplicationConfiguration(CConstPointer _path):
    CAbstractConfiguration(_T("Application")),
	m_path(__FILE__LINE__ _path)
{
}

CApplicationConfiguration::CApplicationConfiguration(ConstRef(CFilePath) _path):
    CAbstractConfiguration(_T("Application")),
	m_path(_path)
{
}

CApplicationConfiguration::~CApplicationConfiguration(void)
{
}

WBool CApplicationConfiguration::GetRawValue(ConstRef(CStringLiteral) _name, Ref(CStringBuffer) _value)
{
	if ( _name.Compare(_T("Application.Directory"), 0, CStringLiteral::cIgnoreCase) == 0 )
	{
		_value = m_path.get_Root();
		_value += m_path.get_Directory();
		return true;
	}
	if ( _name.Compare(_T("Application.Name"), 0, CStringLiteral::cIgnoreCase) == 0 )
	{
		_value = m_path.get_PureBasename();
		return true;
	}
	return false;
}

WBool CApplicationConfiguration::SetRawValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value)
{
	return false;
}

WBool CApplicationConfiguration::Enumerate(ConstRef(CStringLiteral) _prefix, Ref(Keys) _keys)
{
	if ( _prefix.Compare(m_prefix, 0, CStringLiteral::cIgnoreCase) != 0 )
		return false;
	_keys.Append(CStringBuffer(__FILE__LINE__ _T("Application.Directory")));
	_keys.Append(CStringBuffer(__FILE__LINE__ _T("Application.Name")));
	return true;
}

/* CConfigurationList */
static void __stdcall CConfigurationListDeleteFunc(ConstPointer ptr, Pointer context)
{
	CAbstractConfiguration* pConfig = CastAnyPtr(CAbstractConfiguration, CastMutable(Pointer, ptr));

	pConfig->release();
}

CConfigurationList::CConfigurationList(DECL_FILE_LINE0):
    CAbstractConfiguration(),
	m_configList(ARGS_FILE_LINE0)
{
	m_writable = true;
}

CConfigurationList::~CConfigurationList(void)
{
}

void CConfigurationList::Remove(CConfigurationList::Iterator it)
{ 
	m_configList.Remove(it);
}

WBool CConfigurationList::GetRawValue(ConstRef(CStringLiteral) _name, Ref(CStringBuffer) _value)
{
	ConfigList::Iterator it = m_configList.Begin();

	while ( it )
	{
		if ( (*it)->GetRawValue(_name, _value) )
			return true;
		++it;
	}
	return false;
}

WBool CConfigurationList::SetRawValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value)
{
	ConfigList::Iterator it = m_configList.Begin();

	while ( it )
	{
		if ( (*it)->HasValue(_name) )
			return (*it)->SetRawValue(_name, _value);
		++it;
	}

	CStringBuffer name(__FILE__LINE__ _name);
	CStringBuffer name2;
	CPointer splitname[64];
	dword splitsize;
	dword i = 0;
	bool bFirst = true;

	name.SplitAny(_T("."), splitname, 32, &splitsize);
	while ( i < splitsize )
	{
		if ( bFirst )
			bFirst = false;
		else
			name2 += _T(".");
		name2 += splitname[i];
		it = m_configList.Begin();
		while ( it )
		{
			if ( name2.Compare((*it)->GetPrefix(), 0, CStringLiteral::cIgnoreCase) == 0 )
				return (*it)->SetRawValue(_name, _value);
			++it;
		}
		++i;
	}
	return false;
}

WBool CConfigurationList::Enumerate(ConstRef(CStringLiteral) _prefix, Ref(Keys) _keys)
{
	ConfigList::Iterator it = m_configList.Begin();

	while ( it )
	{
		CAbstractConfiguration* pConfiguration = *it;

		if ( _prefix.IsEmpty() )
			_keys.Append(pConfiguration->GetPrefix());
		else if ( _prefix.Compare(pConfiguration->GetPrefix(), 0, CStringLiteral::cIgnoreCase) == 0 )
			return pConfiguration->Enumerate(_prefix, _keys);
		++it;
	}
	if ( _prefix.IsEmpty() )
		return (_keys.Count() > 0);
	return false;
}

