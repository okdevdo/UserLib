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
#include "FilePath.h"
#include "DirectoryIterator.h"

CConstPointer illegal_filename_chars =
        _T("\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10")
        _T("\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F")
        _T("<>:\"/\\|?*");
CConstPointer illegal_filenames[] = {
        _T("CON"), _T("PRN"), _T("AUX"), _T("NUL"),
        _T("COM1"), _T("COM2"), _T("COM3"), _T("COM4"), _T("COM5"), _T("COM6"), _T("COM7"), _T("COM8"), _T("COM9"),
        _T("LPT1"), _T("LPT2"), _T("LPT3"), _T("LPT4"), _T("LPT5"), _T("LPT6"), _T("LPT7"), _T("LPT8"), _T("LPT9") };

bool CFilePath::CheckFilename(CStringLiteral filename)
{
    CStringConstIterator it(filename);
    dword sz;

    while ( !(it.IsEnd()) )
    {
        if ( NotPtrCheck(s_strchr(CastMutablePtr(mbchar, illegal_filename_chars), it[0])) )
            return false;
        ++it;
    }
    it = filename;
    it.Find(_T('.'));
    if ( it.IsEnd() )
        sz = filename.GetLength();
    else
        sz = Castdword(it.GetCurrent() - it.GetOrigin());
    if ( sz > 0 )
        for ( dword ix = 0; ix < (sizeof(illegal_filenames) / sizeof(CConstPointer)); ++ix )
            if ( s_strnicmp(illegal_filenames[ix], filename.GetString(), sz) == 0 )
                return false;
    return true;
}

CStringBuffer CFilePath::TrimFilename(CStringLiteral filename)
{
    CStringBuffer tmp(__FILE__LINE__ filename);
    CStringConstIterator it(filename.End());
    bool bOk = true;
    dword sz;

    while ( !(it.IsBegin()) )
    {
        --it;
        if ( NotPtrCheck(s_strchr(CastMutablePtr(mbchar, illegal_filename_chars), it[0])) )
            tmp.DeleteString(Castdword(it.GetCurrent() - it.GetOrigin()));
    }
    while ( bOk )
    {
        it = tmp.Begin();
        it.Find(_T('.'));
        if ( it.IsEnd() )
            sz = tmp.GetLength();
        else
            sz = Castdword(it.GetCurrent() - it.GetOrigin());
        if ( sz == 0 )
            break;
        bOk = false;
        for ( dword ix = 0; ix < (sizeof(illegal_filenames) / sizeof(CConstPointer)); ++ix )
            if ( s_strnicmp(illegal_filenames[ix], tmp.GetString(), sz) == 0 )
            {
                if ( it.IsEnd() )
                {
                    tmp.Clear();
                    break;
                }
                tmp.DeleteString(0, sz + 1);
                bOk = true;
                break;
            }
    }
    return tmp;
}

CConstPointer CFilePath::_sWinPathSeparator = _T("\\");
CConstPointer CFilePath::_sUnixPathSeparator = _T("/");

CFilePath::CFilePath() :
_path(), _sPathSeparator(PATH_SEPDEFARG)
{
}

CFilePath::CFilePath(DECL_FILE_LINE CConstPointer path, int len, CConstPointer sep) :
_path(ARGS_FILE_LINE path, len), _sPathSeparator(sep)
{
}

CFilePath::CFilePath(DECL_FILE_LINE CStringLiteral path, CConstPointer sep) :
	_path(ARGS_FILE_LINE path.GetString()), _sPathSeparator(sep)
{
}

CFilePath::CFilePath(ConstRef(CStringBuffer) path, CConstPointer sep) :
    _path(path), _sPathSeparator(sep)
{
}

#ifdef QT_VERSION
CFilePath::CFilePath( ConstRef(QString) _str, CConstPointer sep):
    _path(_str), _sPathSeparator(sep)
{
}
#endif

CFilePath::CFilePath(ConstRef(CFilePath) path):
    _path(path._path), _sPathSeparator(path._sPathSeparator)
{
}

CFilePath::~CFilePath(void)
{
}

CFilePath& CFilePath::operator=(CConstPointer path)
{
    _path.SetString(__FILE__LINE__ path);
    return *this;
}

CFilePath& CFilePath::operator=(CStringLiteral path)
{
    _path.SetString(__FILE__LINE__ path);
    return *this;
}

CFilePath& CFilePath::operator=(ConstRef(CStringBuffer) path)
{
    _path.SetString(path);
    return *this;
}

#ifdef QT_VERSION
CFilePath& CFilePath::operator=(ConstRef(QString) path)
{
    _path.SetString(path);
    return *this;
}
#endif

CFilePath& CFilePath::operator=(ConstRef(CFilePath) path)
{
	_path.SetString(path._path);
	_sPathSeparator = path._sPathSeparator;
    return *this;
}

CStringBuffer CFilePath::get_Extension() const
{
	CStringBuffer tmp;

	if ( _path.IsEmpty() )
		return tmp;
	CStringConstIterator it1(_path.GetString());

	it1.FindReverse(_T('.'));
	if ( it1.IsEnd() )
		return tmp;

	CStringConstIterator it2(_path.GetString());

	it2.FindReverse(*get_PathSeparator());
	if ( it2.IsEnd() )
	{
		++it1;
		tmp.SetString(__FILE__LINE__ it1);
		return tmp;
	}
	if ( it1.GetCurrent() < it2.GetCurrent() )
		return tmp;
	++it1;
	tmp.SetString(__FILE__LINE__ it1);
	return tmp;
}

CStringBuffer CFilePath::get_FullExtension() const
{
	CStringBuffer tmp;

	if ( _path.IsEmpty() )
		return tmp;

	CStringConstIterator it1(_path.GetString());

	it1.FindReverse(*get_PathSeparator());
	if ( it1.IsEnd() )
		it1 = _path.GetString();

	CStringConstIterator it2(it1);

	it2.Find(_T('.'));
	if ( it2.IsEnd() )
		return tmp;
	++it2;
	tmp.SetString(__FILE__LINE__ it2);
	return tmp;
}

CStringBuffer CFilePath::get_Basename() const
{
	CStringBuffer tmp;

	if ( _path.IsEmpty() )
		return tmp;
	CStringConstIterator it1(_path.GetString());

	it1.FindReverse(_T('.'));

	CStringConstIterator it2(_path.GetString());

	it2.FindReverse(*get_PathSeparator());
	if ( it2.IsEnd() )
	{
		if ( it1.IsEnd() )
		{
			tmp.SetString(__FILE__LINE__ _path);
			return tmp;
		}
		_path.SubString(0, Castdword(it1.GetCurrent() - it1.GetOrigin()), tmp);
		return tmp;
	}
	++it2;
	if ( it2.IsEnd() )
		return tmp;
	if ( it1.IsEnd() || (it1.GetCurrent() < it2.GetCurrent()) )
	{
		_path.SubString(Castdword(it2.GetCurrent() - it2.GetOrigin()), it2.GetLength(), tmp);
		return tmp;
	}
	_path.SubString(Castdword(it2.GetCurrent() - it2.GetOrigin()), it2.GetLength() - it1.GetLength(), tmp);
	return tmp;
}

CStringBuffer CFilePath::get_PureBasename() const
{
	CStringBuffer tmp;

	if ( _path.IsEmpty() )
		return tmp;

	CStringConstIterator it1(_path.GetString());

	it1.FindReverse(*get_PathSeparator());
	if ( it1.IsEnd() )
		it1 = _path.GetString();
	else
		++it1;

	CStringConstIterator it2(it1);

	it2.Find(_T('.'));
	if ( it2.IsEnd() )
	{
		tmp.SetString(__FILE__LINE__ it1);
		return tmp;
	}
	_path.SubString(Castdword(it1.GetCurrent() - it1.GetOrigin()), it1.GetLength() - it2.GetLength(), tmp);
	return tmp;
}

CStringBuffer CFilePath::get_Filename() const
{
	CStringBuffer tmp;

	if ( _path.IsEmpty() )
		return tmp;

	CStringConstIterator it2(_path.GetString());

	it2.FindReverse(*get_PathSeparator());
	if ( it2.IsEnd() )
	{
		tmp.SetString(__FILE__LINE__ _path);
		return tmp;
	}
	++it2;
	if ( it2.IsEnd() )
		return tmp;
	_path.SubString(Castdword(it2.GetCurrent() - it2.GetOrigin()), it2.GetLength(), tmp);
	return tmp;
}

CStringBuffer CFilePath::get_Directory() const
{
	CStringBuffer tmp;

	if ( _path.IsEmpty() )
		return tmp;

	CStringConstIterator it2(_path.GetString());

	it2.FindReverse(*get_PathSeparator());
	if ( it2.IsEnd() )
		return tmp;

	CStringConstIterator it3(_path.GetString());

	if ( *get_PathSeparator() == CDirectoryIterator::WinPathSeparatorChar() )
	{
		if ( (it3[0] == *get_PathSeparator()) && (it3[1] == *get_PathSeparator()) )
		{
			++it3; 
			++it3;
			it3.Find(*get_PathSeparator());
			if ( it3.IsEnd() || (it3.GetCurrent() == it2.GetCurrent()) )
				return tmp;
		}
		else if ( it3[1] == _T(':') )
		{
			it3.Find(*get_PathSeparator());
			if ( it3.IsEnd() || (it3.GetCurrent() == it2.GetCurrent()) )
				return tmp;
		}
		else
		{
			it3.Find(*get_PathSeparator());
			assert(!(it3.IsEnd()));
			++it2;
			_path.SubString(0, Castdword(it2.GetCurrent() - it2.GetOrigin()), tmp);
			return tmp;
		}
		++it3;
		if ( it3.IsEnd() )
			return tmp;
	}
	else if ( it3[0] == *get_PathSeparator() )
	{
		if (it3.GetCurrent() == it2.GetCurrent() )
			return tmp;
		++it3;
	}
	++it2;
	_path.SubString(Castdword(it3.GetCurrent() - it3.GetOrigin()), it3.GetLength() - it2.GetLength(), tmp);
	return tmp;
}

CStringBuffer CFilePath::get_Directory(int level) const
{
	CStringBuffer tmp(get_Directory());
	CStringBuffer tmp1;

    if ( tmp.IsEmpty() )
        return tmp1;

    CPointer tmpSplitArray[64];
    dword cnt;

    tmp.Split(get_PathSeparator(), tmpSplitArray, 64, &cnt);
	if ( *(tmpSplitArray[cnt - 1]) == 0 )
		--cnt;
	if ( level >= 0 )
	{
		if ( level >= Cast(int, cnt) )
			return tmp1;
		tmp1.SetString(__FILE__LINE__ tmpSplitArray[level]);
	}
	else
	{
		if ( (Cast(int, cnt) + level) < 0 )
			return tmp1;
		tmp1.SetString(__FILE__LINE__ tmpSplitArray[cnt + level]);
	}
	return tmp1;
}

CStringBuffer CFilePath::get_Root() const
{
	CStringBuffer tmp;

	if ( _path.IsEmpty() )
		return tmp;

	CStringConstIterator it3(_path.GetString());

	if ( *get_PathSeparator() == CDirectoryIterator::WinPathSeparatorChar() )
	{
		if ( (it3[0] == *get_PathSeparator()) && (it3[1] == *get_PathSeparator()) )
		{
			++it3; 
			++it3;
			it3.Find(*get_PathSeparator());
			if ( it3.IsEnd() )
			{
				tmp.SetString(__FILE__LINE__ _path);
				tmp += get_PathSeparator();
				return tmp;
			}
		}
		else if ( it3[1] == _T(':') )
		{
			it3.Find(*get_PathSeparator());
			if ( it3.IsEnd() )
			{
				tmp.SetString(__FILE__LINE__ _path);
				tmp += get_PathSeparator();
				return tmp;
			}
		}
		else
			return tmp;
		++it3;
		if ( it3.IsEnd() )
		{
			tmp.SetString(__FILE__LINE__ _path);
			return tmp;
		}
	}
	else if ( it3[0] == *get_PathSeparator() )
		++it3;
	else
		return tmp;
	_path.SubString(0, Castdword(it3.GetCurrent() - it3.GetOrigin()), tmp);
	return tmp;
}

void CFilePath::set_Extension(CStringLiteral extension)
{
    CStringBuffer tmp(__FILE__LINE__ extension);

	if ( (!(tmp.IsEmpty())) && (tmp[0] != _T('.')) )
         tmp.PrependString(_T("."), 1);

	if ( _path.IsEmpty() )
	{
		_path.SetString(tmp);
		return;
	}

	CStringConstIterator it1(_path.GetString());

	it1.FindReverse(_T('.'));
	if ( it1.IsEnd() )
	{
		_path += tmp;
		return;
	}

	CStringConstIterator it2(_path.GetString());

	it2.FindReverse(*get_PathSeparator());
	if ( it2.IsEnd() )
	{
		_path.DeleteString(Castdword(it1.GetCurrent() - it1.GetOrigin()), it1.GetLength());
		_path += tmp;
		return;
	}
	if ( it1.GetCurrent() < it2.GetCurrent() )
	{
		_path += tmp;
		return;
	}
	_path.DeleteString(Castdword(it1.GetCurrent() - it1.GetOrigin()), it1.GetLength());
	_path += tmp;
}

void CFilePath::set_FullExtension(CStringLiteral extension)
{
    CStringBuffer tmp(__FILE__LINE__ extension);

	if ( (!(tmp.IsEmpty())) && (tmp[0] != _T('.')) )
        tmp.PrependString(_T("."), 1);

	if ( _path.IsEmpty() )
	{
		_path.SetString(tmp);
		return;
	}

	CStringConstIterator it1(_path.GetString());

	it1.FindReverse(*get_PathSeparator());
	if ( it1.IsEnd() )
		it1 = _path.GetString();
	else
		++it1;

	CStringConstIterator it2(it1);

	it2.Find(_T('.'));
	if ( it2.IsEnd() )
	{
		_path += tmp;
		return;
	}
	_path.DeleteString(Castdword(it2.GetCurrent() - it2.GetOrigin()), it2.GetLength());
	_path += tmp;
}

void CFilePath::set_Basename(CStringLiteral basename)
{
	if ( _path.IsEmpty() )
	{
		_path.SetString(__FILE__LINE__ basename);
		return;
	}

	CStringConstIterator it1(_path.GetString());

	it1.FindReverse(_T('.'));

	CStringConstIterator it2(_path.GetString());

	it2.FindReverse(*get_PathSeparator());
	if ( it2.IsEnd() )
	{
		if ( it1.IsEnd() )
		{
			_path.SetString(__FILE__LINE__ basename);
			return;
		}
		_path.DeleteString(0, Castdword(it1.GetCurrent() - it1.GetOrigin()));
		_path.PrependString(basename);
		return;
	}
	++it2;
	if ( it2.IsEnd() )
	{
		_path += basename;
		return;
	}
	if ( it1.IsEnd() || (it1.GetCurrent() < it2.GetCurrent()) )
	{
		_path.DeleteString(Castdword(it2.GetCurrent() - it2.GetOrigin()), it2.GetLength());
		_path += basename;
		return;
	}
	_path.DeleteString(Castdword(it2.GetCurrent() - it2.GetOrigin()), it2.GetLength() - it1.GetLength());
	_path.InsertString(Castdword(it2.GetCurrent() - it2.GetOrigin()), basename);
}

void CFilePath::set_PureBasename(CStringLiteral basename)
{
	if ( _path.IsEmpty() )
	{
		_path.SetString(__FILE__LINE__ basename);
		return;
	}

	CStringConstIterator it1(_path.GetString());

	it1.FindReverse(*get_PathSeparator());
	if ( it1.IsEnd() )
		it1 = _path.GetString();
	else
		++it1;

	CStringConstIterator it2(it1);

	it2.Find(_T('.'));
	if ( it2.IsEnd() )
	{
		_path.DeleteString(Castdword(it1.GetCurrent() - it1.GetOrigin()), it1.GetLength());
		_path.AppendString(basename);
		return;
	}
	_path.DeleteString(Castdword(it1.GetCurrent() - it1.GetOrigin()), it1.GetLength() - it2.GetLength());
	_path.InsertString(Castdword(it1.GetCurrent() - it1.GetOrigin()), basename);
}

void CFilePath::set_Filename(CStringLiteral filename)
{
	if ( _path.IsEmpty() )
	{
		_path.SetString(__FILE__LINE__ filename);
		return;
	}

	CStringConstIterator it2(_path.GetString());

	it2.FindReverse(*get_PathSeparator());
	if ( it2.IsEnd() )
	{
		_path.SetString(__FILE__LINE__ filename);
		return;
	}
	++it2;
	if ( !(it2.IsEnd()) )
		_path.DeleteString(Castdword(it2.GetCurrent() - it2.GetOrigin()), it2.GetLength());
	_path += filename;
}

void CFilePath::set_Directory(CStringLiteral directory)
{
	CStringBuffer tmp(__FILE__LINE__ directory);
	CStringConstIterator it(directory);
	bool bEmpty = false;

	if ( tmp.IsEmpty() )
	{
		tmp.SetString(__FILE__LINE__ get_PathSeparator());
		bEmpty = true;
	}
	else
	{
		it.FindReverse(*get_PathSeparator());
		if ( it.IsEnd() )
			tmp += get_PathSeparator();
		else
		{
			++it;
			if ( !(it.IsEnd()) )
				tmp += get_PathSeparator();
		}
	}
	if ( _path.IsEmpty() )
	{
		if ( !bEmpty )
			_path.SetString(tmp);
		return;
	}

	CStringConstIterator it2(_path.GetString());

	it2.FindReverse(*get_PathSeparator());
	if ( it2.IsEnd() )
	{
		if ( !bEmpty )
			_path.PrependString(tmp);
		return;
	}

	CStringConstIterator it3(_path.GetString());

	if ( *get_PathSeparator() == CDirectoryIterator::WinPathSeparatorChar() )
	{
		if ( (it3[0] == *get_PathSeparator()) && (it3[1] == *get_PathSeparator()) )
		{
			++it3; 
			++it3;
			it3.Find(*get_PathSeparator());
			if ( it3.IsEnd() )
			{
				_path += get_PathSeparator();
				if ( s_strcmp(tmp.GetString(), get_PathSeparator()) != 0 )
					_path += tmp;
				return;
			}
		}
		else if ( it3[1] == _T(':') )
		{
			it3.Find(*get_PathSeparator());
			if ( it3.IsEnd() )
			{
				_path += get_PathSeparator();
				if ( s_strcmp(tmp.GetString(), get_PathSeparator()) != 0 )
					_path += tmp;
				return;
			}
		}
	}
	if ( it3.GetCurrent() == it2.GetCurrent() )
	{
		if ( s_strcmp(tmp.GetString(), get_PathSeparator()) != 0 )
		{
			++it3;
			_path.InsertString(Castdword(it3.GetCurrent() - it3.GetOrigin()), tmp);
		}
		return;
	}
	if ( it3[0] == *get_PathSeparator() )
		++it3;

	dword pos = Castdword(it3.GetCurrent() - it3.GetOrigin());

	++it2;
	_path.DeleteString(pos, it3.GetLength() - it2.GetLength());
	if ( s_strcmp(tmp.GetString(), get_PathSeparator()) != 0 )
		_path.InsertString(pos, tmp);
}

void CFilePath::set_Directory(CStringLiteral directory, int level)
{
	CStringBuffer tmp(get_Directory());
    CStringBuffer tmp1(__FILE__LINE__ directory);
	CStringBuffer tmp2;

    if ( tmp.IsEmpty() )
	{
		if ( !(tmp1.IsEmpty()) )
			set_Directory(tmp1);
        return;
	}
	if ( !(tmp1.IsEmpty()) )
	{
		CStringConstIterator it(tmp1);

		it.FindReverse(*get_PathSeparator());
		if ( !(it.IsEnd()) )
		{
			++it;
			if ( it.IsEnd() )
			{
				--it;
				tmp1.DeleteString(Castdword(it.GetCurrent() - it.GetOrigin()));
			}
		}
	}

    CPointer tmpSplitArray[64];
    dword cnt;

    tmp.Split(get_PathSeparator(), tmpSplitArray, 64, &cnt);
	if ( *(tmpSplitArray[cnt - 1]) == 0 )
		--cnt;
	if ( level >= 0 )
	{
		if ( level >= Cast(int, cnt) )
			return;
		tmpSplitArray[level] = (tmp1.IsEmpty())?NULL:CastMutable(CPointer, tmp1.GetString());
	}
	else
	{
		if ( (Cast(int, cnt) + level) < 0 )
			return;
		tmpSplitArray[cnt + level] = (tmp1.IsEmpty())?NULL:CastMutable(CPointer, tmp1.GetString());
	}
	for ( dword ix = 0; ix < cnt; ++ix )
		if ( NotPtrCheck(tmpSplitArray[ix]) )
		{
			tmp2.AppendString(tmpSplitArray[ix]);
			tmp2.AppendString(get_PathSeparator());
		}
	set_Directory(tmp2);
}

void CFilePath::append_Directory(CStringLiteral directory)
{
	CStringBuffer tmp(__FILE__LINE__ directory);
	CStringConstIterator it(directory);

	if ( tmp.IsEmpty() )
		return;
	it.FindReverse(*get_PathSeparator());
	if ( it.IsEnd() )
		tmp += get_PathSeparator();
	else
	{
		++it;
		if ( !(it.IsEnd()) )
			tmp += get_PathSeparator();
	}

	CStringBuffer tmp1(get_Directory());

	tmp1 += tmp;
	set_Directory(tmp1);
}

void CFilePath::set_Root(CStringLiteral root)
{
	CStringBuffer tmp(__FILE__LINE__ root);
	CStringConstIterator it(root);

	if ( !(tmp.IsEmpty()) )
	{
		it.FindReverse(*get_PathSeparator());
		if ( it.IsEnd() )
			tmp += get_PathSeparator();
		else
		{
			++it;
			if ( !(it.IsEnd()) )
				tmp += get_PathSeparator();
		}
	}
	if ( _path.IsEmpty() )
	{
		_path = tmp;
		return;
	}

	CStringConstIterator it3(_path.GetString());

	if ( *get_PathSeparator() == CDirectoryIterator::WinPathSeparatorChar() )
	{
		if ( (it3[0] == *get_PathSeparator()) && (it3[1] == *get_PathSeparator()) )
		{
			++it3; 
			++it3;
			it3.Find(*get_PathSeparator());
			if ( it3.IsEnd() )
			{
				_path = tmp;
				return;
			}
		}
		else if ( it3[1] == _T(':') )
		{
			it3.Find(*get_PathSeparator());
			if ( it3.IsEnd() )
			{
				_path = tmp;
				return;
			}
		}
		else
		{
			_path.PrependString(tmp);
			return;
		}
	}
	else if ( it3[0] != *get_PathSeparator() )
	{
		_path.PrependString(tmp);
		return;
	}
	++it3;
	if ( it3.IsEnd() )
	{
		_path = tmp;
		return;
	}
	_path.DeleteString(0, Castdword(it3.GetCurrent() - it3.GetOrigin()));
	_path.PrependString(tmp);
}

WBool CFilePath::is_File() const
{
	return !(is_Directory());
}

WBool CFilePath::is_Directory() const
{
	CStringConstIterator it(_path.GetString());

	it.FindReverse(*get_PathSeparator());
	if ( it.IsEnd() )
		return false;
	++it;
	return it.IsEnd();
}

WBool CFilePath::is_Relative() const
{
	return !(is_Absolute());
}

WBool CFilePath::is_Absolute() const
{
	if ( _path.IsEmpty() )
		return false;

	CStringConstIterator it3(_path.GetString());

	if ( *get_PathSeparator() == CDirectoryIterator::WinPathSeparatorChar() )
	{
		if ( (it3[0] == *get_PathSeparator()) && (it3[1] == *get_PathSeparator()) )
			return true;
		if ( (it3[1] == _T(':')) && (it3[2] == *get_PathSeparator()) )
			return true;
	}
	else if ( it3[0] == *get_PathSeparator() )
		return true;
	return false;
}

void CFilePath::MakeAbsolute()
{
    if ( is_Absolute() )
        return;

	CFilePath tmpPath;
	
	CDirectoryIterator::GetCurrentDirectory(tmpPath);
#ifdef OK_SYS_WINDOWS
	assert(*get_PathSeparator() == CDirectoryIterator::WinPathSeparatorChar());
#endif
#ifdef OK_SYS_UNIX
	assert(*get_PathSeparator() == CDirectoryIterator::UnixPathSeparatorChar());
#endif
	_path.PrependString(tmpPath.GetString());
}

void CFilePath::MakeDirectory()
{
	if ( _path.IsEmpty() )
		return;
    if ( is_Directory() )
        return;
	_path.AppendString(get_PathSeparator());
}

void CFilePath::Normalize(CConstPointer replaceRoot)
{
	if ( NotPtrCheck(replaceRoot) )
	{
		_sPathSeparator = CDirectoryIterator::DefaultPathSeparatorString();
		set_Root(replaceRoot);
	}
	if ( *get_PathSeparator() == CDirectoryIterator::WinPathSeparatorChar() )
	{
		_path.ReplaceString(_T("/"), _T("\\"), 1);

		dword sz = _path.GetSize();

		_path.ReplaceString(_T("\\\\"), _T("\\"), 1);

		dword sz1 = _path.GetSize();

		while ( sz1 < sz )
		{
			sz = sz1;
			_path.ReplaceString(_T("\\\\"), _T("\\"), 1);
			sz1 = _path.GetSize();
		}
	}
	else
	{
		_path.ReplaceString(_T("\\"), _T("/"), 1);

		dword sz = _path.GetSize();

		_path.ReplaceString(_T("//"), _T("/"), 1);

		dword sz1 = _path.GetSize();

		while ( sz1 < sz )
		{
			sz = sz1;
			_path.ReplaceString(_T("//"), _T("/"), 1);
			sz1 = _path.GetSize();
		}
	}

	CStringBuffer dir(get_Directory());
	CStringBuffer tmp;
	CPointer splitArray[64];
	bool bSplitArray[64];
	dword splitCnt = 0;
	dword splitIx;
	dword splitIx1;
	bool bAny = false;

	if ( !(dir.IsEmpty()) )
	{
		dir.Split(get_PathSeparator(), splitArray, 64, &splitCnt);
		if ( splitCnt > 1 )
		{
			for ( splitIx = 0; splitIx < splitCnt; ++splitIx )
				bSplitArray[splitIx] = true;
			if ( *(splitArray[splitCnt - 1]) == 0 )
			{
				bSplitArray[splitCnt - 1] = false;
				--splitCnt;
			}
			splitIx = 0;
			while ( splitIx < splitCnt )
			{
				if ( s_strcmp(_T(".."), splitArray[splitIx]) == 0 )
				{
					splitIx1 = splitIx;
					bSplitArray[splitIx1] = false;
					while ( (splitIx1 > 0) && (!(bSplitArray[splitIx1])) )
						--splitIx1;
					bSplitArray[splitIx1] = false;
					bAny = true;
				}
				else if ( s_strcmp(_T("."), splitArray[splitIx]) == 0 )
				{
					bSplitArray[splitIx] = false;
					bAny = true;
				}
				++splitIx;
			}
			if ( bAny )
			{
				splitIx = 0;
				while ( splitIx < splitCnt )
				{
					if ( bSplitArray[splitIx] )
					{
						tmp.AppendString(splitArray[splitIx]);
						tmp.AppendString(get_PathSeparator());
					}
					++splitIx;
				}
				set_Directory(tmp);
			}
		}
	}
}

CConstPointer CFilePath::DefaultPathSeparatorString()
{
#ifdef OK_SYS_WINDOWS
	return _sWinPathSeparator;
#endif
#ifdef OK_SYS_UNIX
	return _sUnixPathSeparator;
#endif
}

CConstPointer CFilePath::WinPathSeparatorString()
{
	return _sWinPathSeparator;
}

CConstPointer CFilePath::UnixPathSeparatorString()
{
	return _sUnixPathSeparator;
}
