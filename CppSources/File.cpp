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
#include "File.h"

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#ifdef OK_SYS_WINDOWS
	#include <share.h>
#endif
#ifdef OK_SYS_UNIX
	#include <unistd.h>
	#define _write write
	#define _read read
	#define _close close
    #define _dup dup
#endif
#include <sys/types.h>
#include <sys/stat.h>

IMPL_EXCEPTION(CFileException, CBaseException)

//***************** CFile ****************************************************
CFile::CFile(void):
    m_path(), m_readOnly(false)
{
}

CFile::CFile(ConstRef(CFilePath) _path, WBool _readOnly, WBool _textMode, TEncoding _encoding) :
m_path(_path), m_readOnly(_readOnly), m_textMode(_textMode), m_encoding(_encoding)
{
}

CFile::CFile(ConstRef(CFile) _file):
m_path(_file.m_path), m_readOnly(_file.m_readOnly), m_textMode(_file.m_textMode), m_encoding(_file.m_encoding)
{
}

CFile::~CFile(void)
{
	Close();
}

void CFile::ScanForEncoding()
{
	CByteBuffer bBuf;
	BPointer bP;

	bBuf.set_BufferSize(__FILE__LINE__ 2048);
	Read(bBuf);
	if (bBuf.get_BufferSize() == 0)
	{
		m_encoding = ISO_8859_1_Encoding;
		return;
	}
	bP = bBuf.get_Buffer();
	if (bBuf.get_BufferSize() < 2)
	{
		if (*bP > 0x7f)
			m_encoding = ISO_8859_1_Encoding;
		else
			m_encoding = US_ASCII_Encoding;
		SetFilePos(0);
		return;
	}
	if (bBuf.get_BufferSize() < 3)
	{
		if ((*bP == 0xFF) && (*(bP + 1) == 0xFE))
		{
			m_encoding = UTF_16LE_Encoding;
			return;
		}
		if ((*bP == 0xFE) && (*(bP + 1) == 0xFF))
		{
			m_encoding = UTF_16BE_Encoding;
			return;
		}
		if ((*bP != 0) && (*(bP + 1) == 0))
		{
			m_encoding = UTF_16LE_Encoding;
			SetFilePos(0);
			return;
		}
		if ((*bP == 0) && (*(bP + 1) != 0))
		{
			m_encoding = UTF_16BE_Encoding;
			SetFilePos(0);
			return;
		}
		if ((*bP > 0x7F) || (*(bP + 1) > 0x7F))
		{
			m_encoding = ISO_8859_1_Encoding;
			SetFilePos(0);
			return;
		}
		m_encoding = US_ASCII_Encoding;
		SetFilePos(0);
		return;
	}
	if (bBuf.get_BufferSize() < 4)
	{
		if ((*bP == 0xEF) && (*(bP + 1) == 0xBB) && (*(bP + 2) == 0xBF))
		{
			m_encoding = UTF_8_Encoding;
			return;
		}
		if ((*bP > 0x7F) || (*(bP + 1) > 0x7F) || (*(bP + 2) > 0x7F))
		{
			m_encoding = ISO_8859_1_Encoding;
			SetFilePos(0);
			return;
		}
		m_encoding = US_ASCII_Encoding;
		SetFilePos(0);
		return;
	}
	if ((*bP == 0xEF) && (*(bP + 1) == 0xBB) && (*(bP + 2) == 0xBF))
	{
		m_encoding = UTF_8_Encoding;
		SetFilePos(3);
		return;
	}
	if ((*bP == 0xFF) && (*(bP + 1) == 0xFE))
	{
		m_encoding = UTF_16LE_Encoding;
		SetFilePos(2);
		return;
	}
	if ((*bP == 0xFE) && (*(bP + 1) == 0xFF))
	{
		m_encoding = UTF_16BE_Encoding;
		SetFilePos(2);
		return;
	}

	if ((bBuf.get_BufferSize() % 2) == 0)
	{
		dword evenNull = 0;
		dword evenNotNull = 0;
		dword oddNull = 0;
		dword oddNotNull = 0;

		for (dword i = 0; i < bBuf.get_BufferSize(); ++i)
		{
			if ((i % 2) == 0) // even
			{
				if (*bP == 0)
					++evenNull;
				else
					++evenNotNull;
			}
			else // odd
			{
				if (*bP == 0)
					++oddNull;
				else
					++oddNotNull;
			}
		}
		if (((evenNull / 5) > evenNotNull) && ((oddNotNull / 5) > oddNull))
		{
			m_encoding = UTF_16BE_Encoding;
			SetFilePos(0);
			return;
		}
		if (((evenNotNull / 5) > evenNull) && ((oddNull / 5) > oddNotNull))
		{
			m_encoding = UTF_16LE_Encoding;
			SetFilePos(0);
			return;
		}
	}
	for (dword i = 0; i < bBuf.get_BufferSize(); ++i)
	{
		if (*bP++ > 0x7f)
		{
			m_encoding = ISO_8859_1_Encoding;
			SetFilePos(0);
			return;
		}
	}
	m_encoding = US_ASCII_Encoding;
	SetFilePos(0);
}

void CFile::WriteBOM()
{
	CByteBuffer bBuf;
	BPointer pB;

	switch (m_encoding)
	{
	case BinaryFile_NoEncoding:
	case ISO_8859_1_Encoding:
	case US_ASCII_Encoding:
		break;
	case UTF_8_Encoding:
		bBuf.set_BufferSize(__FILE__LINE__ 3);
		pB = bBuf.get_Buffer();
		*pB++ = 0xEF;
		*pB++ = 0xBB;
		*pB++ = 0xBF;
		break;
	case UTF_16_Encoding:
	case UTF_16LE_Encoding:
		bBuf.set_BufferSize(__FILE__LINE__ 2);
		pB = bBuf.get_Buffer();
		*pB++ = 0xFF;
		*pB++ = 0xFE;
		break;
	case UTF_16BE_Encoding:
		bBuf.set_BufferSize(__FILE__LINE__ 2);
		pB = bBuf.get_Buffer();
		*pB++ = 0xFE;
		*pB++ = 0xFF;
		break;
	default:
		break;
	}
	if (bBuf.get_BufferSize() > 0)
		Write(bBuf);
}

void CFile::operator = (ConstRef(CFile) _file)
{
	m_path = _file.m_path;
	m_readOnly = _file.m_readOnly;
	m_textMode = _file.m_textMode;
	m_encoding = _file.m_encoding;
}

void CFile::Open(ConstRef(CFilePath) _path, WBool _readOnly, WBool _textMode, TEncoding _encoding)
{
	m_path = _path;
	m_readOnly = _readOnly;
	m_textMode = _textMode;
	m_encoding = _encoding;
}

void CFile::Create(ConstRef(CFilePath) _path, WBool _textMode, TEncoding _encoding)
{
	m_path = _path;
	m_readOnly = false;
	m_textMode = _textMode;
	m_encoding = _encoding;
}

void CFile::Close()
{
}

WBool CFile::IsEOF()
{
	TFileOffset pos = GetFilePos();

	if ( pos < 0 )
		return true;

	TFileSize len = GetSize();

	if ( len == 0 )
		return true;

	return (Cast(TFileSize, pos) >= len);
}

void CFile::Read(Ref(CStringBuffer) _buffer, WULong _bufferSize)
{
	CByteBuffer bBuf;
	bool bLittleEndian;

	bBuf.set_BufferSize(__FILE__LINE__ _bufferSize);
	Read(bBuf);
	switch (m_encoding)
	{
	case BinaryFile_NoEncoding:
	case ISO_8859_1_Encoding:
	case US_ASCII_Encoding:
		_buffer.convertFromByteBuffer(bBuf);
		break;
	case UTF_8_Encoding:
		_buffer.convertFromUTF8(bBuf, false);
		break;
	case UTF_16_Encoding:
	case UTF_16LE_Encoding:
		bLittleEndian = true;
		_buffer.convertFromUTF16(bBuf, &bLittleEndian, false);
		break;
	case UTF_16BE_Encoding:
		bLittleEndian = false;
		_buffer.convertFromUTF16(bBuf, &bLittleEndian, false);
		break;
	default:
		break;
	}
}

void CFile::Write(ConstRef(CStringBuffer) _buffer)
{
	CByteBuffer bBuf;

	switch (m_encoding)
	{
	case BinaryFile_NoEncoding:
	case ISO_8859_1_Encoding:
	case US_ASCII_Encoding:
		_buffer.convertToByteBuffer(bBuf);
		break;
	case UTF_8_Encoding:
		_buffer.convertToUTF8(bBuf, false);
		break;
	case UTF_16_Encoding:
	case UTF_16LE_Encoding:
		_buffer.convertToUTF16(bBuf, true, false);
		break;
	case UTF_16BE_Encoding:
		_buffer.convertToUTF16(bBuf, false, false);
		break;
	default:
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
			_T("CFile::Write"), EINVAL);
		break;
	}
	Write(bBuf);
}

void CFile::Read(CConstPointer _format, ...)
{
	CStringBuffer sBuf;
	va_list args;
	TFileOffset off;
	sdword strL;
	sdword strL1;

	off = GetFilePos();

	CByteBuffer bBuf;
	bool bLittleEndian;
	BPointer mapP = NULL;
	BPointer mapP1 = NULL;

	bBuf.set_BufferSize(__FILE__LINE__ 2048);
	Read(bBuf);
	switch (m_encoding)
	{
	case BinaryFile_NoEncoding:
	case ISO_8859_1_Encoding:
	case US_ASCII_Encoding:
		sBuf.convertFromByteBuffer(bBuf);
		break;
	case UTF_8_Encoding:
		sBuf.convertFromUTF8(bBuf, false, &mapP);
		break;
	case UTF_16_Encoding:
	case UTF_16LE_Encoding:
		bLittleEndian = true;
		sBuf.convertFromUTF16(bBuf, &bLittleEndian, false);
		break;
	case UTF_16BE_Encoding:
		bLittleEndian = false;
		sBuf.convertFromUTF16(bBuf, &bLittleEndian, false);
		break;
	default:
		break;
	}
	va_start(args, _format);
	strL = sBuf.ScanString(_format, args);
	va_end(args);
	if (0 > strL)
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
			_T("CFile::Read"), errno);
	if (mapP)
	{
		strL1 = strL;
		mapP1 = mapP;
		while (strL1)
		{
			if (*mapP1 > 1)
				strL += *mapP1 - 1;
			++mapP1;
			--strL1;
		}
		TFfree(mapP);
	}
	SetFilePos(off + strL);
}

void CFile::Write(CConstPointer _format, ...)
{
	CStringBuffer sBuf;
	va_list args;

	va_start(args, _format);
	sBuf.FormatString(__FILE__LINE__ _format, args);
	va_end(args);
	Write(sBuf);
}


//***************** CDiskFile ****************************************************
CDiskFile::CDiskFile(void) :
    CFile(), m_fd(-1)
{
}

CDiskFile::CDiskFile(ConstRef(CFilePath) _path, WBool _readOnly, WBool _textMode, TEncoding _encoding) :
    CFile(_path, _readOnly, _textMode, _encoding), m_fd(-1)
{
	Open(_path, _readOnly, _textMode, _encoding);
}

CDiskFile::CDiskFile(ConstRef(CDiskFile) diskfile):
    CFile(diskfile), m_fd(-1)
{
	if ( diskfile.m_fd < 0 )
		return;
	m_fd = _dup(diskfile.m_fd);
	if ( m_fd < 0 )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
		    _T("CDiskFile::CDiskFile"), errno);
}

CDiskFile::~CDiskFile(void)
{
}

void  CDiskFile::operator = (ConstRef(CDiskFile) diskfile)
{
	if ( diskfile.m_fd < 0 )
		return;
	CFile::operator=(diskfile);
	Close();
	m_fd = _dup(diskfile.m_fd);
	if ( m_fd < 0 )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
		    _T("CDiskFile::operator="), errno);
}

void CDiskFile::Open(ConstRef(CFilePath) _path, WBool _readOnly, WBool _textMode, TEncoding _encoding)
{
	Close();
	CFile::Open(_path, _readOnly, _textMode, _encoding);
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	m_fd = _tsopen(_path.get_Path().GetString(), (_textMode ? _O_TEXT : _O_BINARY) | (_readOnly ? _O_RDONLY : _O_RDWR), _SH_DENYWR, _S_IREAD | _S_IWRITE);
    if ( m_fd < 0 )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException, path = '%s'"), 
		    _T("CDiskFile::Open"), _path.get_Path().GetString(), errno);
#endif
#ifdef OK_SYS_UNIX
	m_fd = open(_path.get_Path().GetString(), (_textMode ? _O_TEXT : _O_BINARY) | (_readOnly?_O_RDONLY:_O_RDWR));
    if ( m_fd < 0 )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException, path = '%s'"), 
		    _T("CDiskFile::Open"), _path.get_Path().GetString(), errno);
#endif
#endif
#ifdef OK_COMP_MSC
	errno_t err;

	if (0 != (err = _tsopen_s(&m_fd, _path.get_Path().GetString(), (_textMode ? _O_TEXT : _O_BINARY) | (_readOnly ? _O_RDONLY : _O_RDWR), _SH_DENYWR, _S_IREAD | _S_IWRITE)))
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException, path = '%s'"), 
		    _T("CDiskFile::Open"), _path.get_Path().GetString(), err);
#endif
	if (_encoding == ScanFor_Encoding)
		ScanForEncoding();
}

void CDiskFile::Create(ConstRef(CFilePath) _path, WBool _textMode, TEncoding _encoding)
{
	Close();
	CFile::Create(_path, _textMode, _encoding);
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	m_fd = _tsopen(_path.get_Path().GetString(), (_textMode ? _O_TEXT : _O_BINARY) | _O_CREAT | _O_TRUNC | _O_RDWR, _SH_DENYWR, _S_IREAD | _S_IWRITE);
    if ( m_fd < 0 )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException, path = '%s'"), 
		    _T("CDiskFile::Create"), _path.get_Path().GetString(), errno);
#endif
#ifdef OK_SYS_UNIX
    m_fd = creat(_path.get_Path().GetString(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if ( m_fd < 0 )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException, path = '%s'"), 
		    _T("CDiskFile::Create"), _path.get_Path().GetString(), errno);
#endif
#endif
#ifdef OK_COMP_MSC
	errno_t err;

	if (0 != (err = _tsopen_s(&m_fd, _path.get_Path().GetString(), (_textMode ? _O_TEXT : _O_BINARY) | _O_CREAT | _O_TRUNC | _O_RDWR, _SH_DENYWR, _S_IREAD | _S_IWRITE)))
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException, path = '%s'"), 
		    _T("CDiskFile::Create"), _path.get_Path().GetString(), err);
#endif
	WriteBOM();
}

void CDiskFile::Close()
{
	if ( m_fd < 0 )
		return;
	_close( m_fd );
	m_fd = -1;
}

CFile::TFileSize CDiskFile::GetSize()
{
	TFileSize res = 0;

	if ( m_fd < 0 )
		return res;

#ifdef OK_SYS_WINDOWS
	long long len;

	len = _filelengthi64( m_fd );
	
	if ( len < 0 )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
		    _T("CDiskFile::GetSize"), errno);
	res = Cast(TFileSize, len);
#endif
#ifdef OK_SYS_UNIX
	off_t pos = lseek(m_fd, 0, SEEK_CUR);
	
	if ( pos < 0 )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
		    _T("CDiskFile::GetSize"), errno);
	
	off_t end = lseek(m_fd, 0, SEEK_END);
	
	if ( end < 0 )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
		    _T("CDiskFile::GetSize"), errno);
		
	if ( lseek(m_fd, pos, SEEK_SET) < 0 )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
		    _T("CDiskFile::GetSize"), errno);
	res = Cast(TFileSize, end);
#endif
	return res;
}

void CDiskFile::SetSize(TFileSize size)
{
	if ( m_fd < 0 )
		return;

#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	if ( _chsize(m_fd, size) < 0 )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
		    _T("CDiskFile::SetSize"), errno);
#endif
#ifdef OK_SYS_UNIX
	off_t pos = lseek(m_fd, 0, SEEK_CUR);
	
	if ( pos < 0 )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
		    _T("CDiskFile::SetSize"), errno);
	
	off_t end = lseek(m_fd, 0, SEEK_END);
	
	if ( end < 0 )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
		    _T("CDiskFile::SetSize"), errno);
	
	TFileOffset cnt = size - end;
	
	if ( cnt < 0 ) // FIXME: truncate, use tmp file
		return;
	
	char buf[4096] = { 0 };
	
	while ( cnt > 0 )
	{
		dword sz = (cnt > 4096) ? 4096 : cnt;

		if ( write(m_fd, buf, sz) != Cast(int,sz) )
			throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
			    _T("CDiskFile::SetSize"), errno);
		cnt -= sz;
	}
	if ( lseek(m_fd, pos, SEEK_SET) < 0 )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
		    _T("CDiskFile::SetSize"), errno);
		
#endif
#endif
#ifdef OK_COMP_MSC
	errno_t res = _chsize_s(m_fd, size);

	if ( res )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
		    _T("CDiskFile::SetSize"), res);
#endif
}

CFile::TFileOffset CDiskFile::GetFilePos()
{
	TFileOffset res = -1;

	if ( m_fd < 0 )
		return res;

#ifdef OK_SYS_WINDOWS
	res = _telli64(m_fd);
#endif
#ifdef OK_SYS_UNIX
	res = lseek(m_fd, 0, SEEK_CUR);
#endif
	if ( res < 0 )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
		    _T("CDiskFile::GetFilePos"), errno);
	return res;
}

void CDiskFile::SetFilePos(TFileOffset pos)
{
	if ( m_fd < 0 )
		return;
#ifdef OK_SYS_WINDOWS
	if (0 > _lseeki64(m_fd, pos, SEEK_SET))
#endif
#ifdef OK_SYS_UNIX
	if (0 > lseek(m_fd, pos, SEEK_SET))
#endif
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
		    _T("CDiskFile::SetFilePos"), errno);
}

WBool CDiskFile::IsEOF()
{
	if ( m_fd < 0 )
		return true;

#ifdef OK_SYS_WINDOWS
	int res = _eof(m_fd);

	if ( res < 0 )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
		    _T("CDiskFile::IsEOF"), errno);
	return (res > 0); // res == 1: current file pos is eof
#endif
#ifdef OK_SYS_UNIX
	return CFile::IsEOF();
#endif
}

void CDiskFile::Read(Ref(CByteBuffer) _buffer)
{
	if ( m_fd < 0 )
	{
		_buffer.set_BufferSize(__FILE__LINE__ 0);
		return;
	}

	int cnt = _read( m_fd, _buffer.get_Buffer(), _buffer.get_BufferSize() );

	if ( 0 > cnt )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
		    _T("CDiskFile::Read"), errno);
	_buffer.set_BufferSize(__FILE__LINE__ cnt);
}

void CDiskFile::Read(Ref(CByteLinkedBuffer) _buffer)
{
	dword ix = 0;
	dword max = _buffer.GetBufferItemCount();

	if ( m_fd >= 0 )
	{
		for ( ; ix < max; ++ix )
		{
			dword sz;
			BPointer p = _buffer.GetBufferItem(ix, &sz);
			int cnt = _read( m_fd, p, sz );

			if ( 0 > cnt )
				throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
					_T("CDiskFile::Read"), errno);
			if ( cnt == 0 )
				break;
			_buffer.SetBufferItemSize(ix, cnt);
		}
	}
	for ( ; ix < max; ++ix )
		_buffer.SetBufferItemSize(ix, 0);
}

void CDiskFile::Write(ConstRef(CByteBuffer) _buffer)
{
	if ( m_fd < 0 )
		return;

	dword sz = _buffer.get_BufferSize();
	BPointer p = _buffer.get_Buffer();

	if ( 0 > _write( m_fd, p, sz ) )
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
		    _T("CDiskFile::Write"), errno);
}

void CDiskFile::Write(ConstRef(CByteLinkedBuffer) _buffer)
{
	if ( m_fd < 0 )
		return;

	for ( dword ix = 0; ix < _buffer.GetBufferItemCount(); ++ix )
	{
		dword sz;
		BPointer p = _buffer.GetBufferItem(ix, &sz);

		if ( 0 > _write( m_fd, p, sz ) )
			throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"), 
		        _T("CDiskFile::Write"), errno);
	}
}

//***************** CStreamFile ****************************************************
CStreamFile::CStreamFile(void) :
CFile(), m_file(NULL)
{
}

CStreamFile::CStreamFile(ConstRef(CFilePath) _path, WBool _readOnly, WBool _textMode, TEncoding _encoding) :
CFile(_path, _readOnly, _textMode, _encoding), m_file(NULL)
{
	Open(_path, _readOnly, _textMode, _encoding);
}

CStreamFile::CStreamFile(ConstRef(CStreamFile) diskfile) :
CFile(diskfile), m_file(NULL)
{
}

CStreamFile::~CStreamFile(void)
{
}

void  CStreamFile::operator = (ConstRef(CStreamFile) diskfile)
{
}

void CStreamFile::Open(ConstRef(CFilePath) _path, WBool _readOnly, WBool _textMode, TEncoding _encoding)
{
	CStringBuffer vMode;

	Close();
	CFile::Open(_path, _readOnly, _textMode, _encoding);

	if (m_readOnly)
		vMode.SetString(__FILE__LINE__ _T("r"));
	else
		vMode.SetString(__FILE__LINE__ _T("rw"));
	if (m_textMode)
		vMode.AppendString(_T("t"));
	else
		vMode.AppendString(_T("b"));
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	m_file = _tfopen(m_path.GetString(), vMode.GetString());
	if (PtrCheck(m_file))
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException, path = '%s'"),
			_T("CStreamFile::Open"), m_path.GetString(), errno);
#endif
#ifdef OK_SYS_UNIX
	m_file = fopen(m_path.GetString(), vMode.GetString());
	if (PtrCheck(m_file))
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException, path = '%s'"),
			_T("CStreamFile::Open"), m_path.GetString(), errno);
#endif
#endif
#ifdef OK_COMP_MSC
	errno_t err = _tfopen_s(CastAnyPtr(FILE*, &m_file), m_path.GetString(), vMode.GetString());
	if (err)
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException, path = '%s'"),
			_T("CStreamFile::Open"), m_path.GetString(), err);
#endif
	if (_encoding == ScanFor_Encoding)
		ScanForEncoding();
}

void CStreamFile::ReOpen(ConstRef(CFilePath) _path, Pointer stdfile, WBool _textMode, TEncoding _encoding)
{
	CStringBuffer vMode;
	bool vreadonly = false;

	if (stdfile == stdin)
	{
		vMode.SetString(__FILE__LINE__ _T("r"));
		vreadonly = true;
	}
	else if ((stdfile == stdout) || (stdfile == stderr))
		vMode.SetString(__FILE__LINE__ _T("w"));
	else
		vMode.SetString(__FILE__LINE__ _T("rw"));
	if (m_textMode)
		vMode.AppendString(_T("t"));
	else
		vMode.AppendString(_T("b"));
	Close();
	CFile::Open(_path, vreadonly, _textMode, _encoding);
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	m_file = _tfreopen(_path.GetString(), vMode.GetString(), CastAnyPtr(FILE, stdfile));
	if (PtrCheck(m_file))
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException, path = '%s'"),
			_T("CStreamFile::ReOpen"), _path.GetString(), errno);
#endif
#ifdef OK_SYS_UNIX
	m_file = freopen(_path.GetString(), vMode.GetString(), CastAnyPtr(FILE, stdfile));
	if (PtrCheck(m_file))
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException, path = '%s'"),
			_T("CStreamFile::ReOpen"), _path.GetString(), errno);
#endif
#endif
#ifdef OK_COMP_MSC
	errno_t err = _tfreopen_s(CastAnyPtr(FILE*, &m_file), _path.GetString(), vMode.GetString(), CastAnyPtr(FILE, stdfile));
	if (err)
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException, path = '%s'"),
			_T("CStreamFile::ReOpen"), _path.GetString(), err);
#endif
}

void CStreamFile::Create(ConstRef(CFilePath) _path, WBool _textMode, TEncoding _encoding)
{
	CStringBuffer vMode;

	Close();
	CFile::Create(_path, _textMode, _encoding);

	vMode.SetString(__FILE__LINE__ _T("w+"));
	if (m_textMode)
		vMode.AppendString(_T("t"));
	else
		vMode.AppendString(_T("b"));
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	m_file = _tfopen(m_path.GetString(), vMode.GetString());
	if (PtrCheck(m_file))
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException, path = '%s'"),
			_T("CStreamFile::Create"), m_path.GetString(), errno);
#endif
#ifdef OK_SYS_UNIX
	m_file = fopen(m_path.GetString(), vMode.GetString());
	if (PtrCheck(m_file))
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException, path = '%s'"),
			_T("CStreamFile::Create"), m_path.GetString(), errno);
#endif
#endif
#ifdef OK_COMP_MSC
	errno_t err = _tfopen_s(CastAnyPtr(FILE*, &m_file), m_path.GetString(), vMode.GetString());
	if (err)
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException, path = '%s'"),
			_T("CStreamFile::Create"), m_path.GetString(), err);
#endif
	WriteBOM();
}

void CStreamFile::Close()
{
	if (PtrCheck(m_file))
		return;
	fclose(CastAnyPtr(FILE,m_file));
	m_file = NULL;
}

CFile::TFileSize CStreamFile::GetSize()
{
	TFileSize res = 0;

	if (PtrCheck(m_file) || ferror(CastAnyPtr(FILE, m_file)))
		return res;
	
#ifdef OK_SYS_WINDOWS
	fpos_t curpos;

	if (fgetpos(CastAnyPtr(FILE, m_file), &curpos))
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::GetSize"), errno);

	if (_fseeki64(CastAnyPtr(FILE, m_file), 0, SEEK_END))
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::GetSize"), errno);

	__int64 len = _ftelli64(CastAnyPtr(FILE, m_file));

	if (len < 0)
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::GetSize"), errno);

	if (fsetpos(CastAnyPtr(FILE, m_file), &curpos))
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::GetSize"), errno);

	res = Cast(TFileSize, len);
#endif
#ifdef OK_SYS_UNIX
	fpos_t pos;

	if (fgetpos(CastAnyPtr(FILE, m_file), &pos))
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::GetSize"), errno);

	if (fseek(CastAnyPtr(FILE, m_file), 0, SEEK_END))
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::GetSize"), errno);

	long len = ftell(CastAnyPtr(FILE, m_file));

	if (len < 0)
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::GetSize"), errno);

	if (fsetpos(CastAnyPtr(FILE, m_file), &pos))
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::GetSize"), errno);

	res = Cast(TFileSize, len);
#endif
	return res;
}

void CStreamFile::SetSize(TFileSize size)
{
	if (PtrCheck(m_file) || ferror(CastAnyPtr(FILE, m_file)))
		return;

#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	if (_chsize(_fileno(CastAnyPtr(FILE, m_file)), size) < 0)
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::SetSize"), errno);
#endif
#ifdef OK_SYS_UNIX
	fpos_t pos;

	if (fgetpos(CastAnyPtr(FILE, m_file), &pos))
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::SetSize"), errno);

	if (fseek(CastAnyPtr(FILE, m_file), 0, SEEK_END))
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::SetSize"), errno);

	long end = ftell(CastAnyPtr(FILE, m_file));

	if (end < 0)
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::SetSize"), errno);

	TFileOffset cnt = size - end;

	if (cnt < 0) // FIXME: truncate, use tmp file
		return;

	char buf[4096] = { 0 };

	while (cnt > 0)
	{
		size_t sz = (cnt > 4096) ? 4096 : cnt;

		if (fwrite(buf, 1, sz, CastAnyPtr(FILE, m_file)) != Cast(size_t, sz))
			throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
			_T("CStreamFile::SetSize"), errno);
		cnt -= sz;
	}
	if (fsetpos(CastAnyPtr(FILE, m_file), &pos))
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::SetSize"), errno);
#endif
#endif
#ifdef OK_COMP_MSC
	errno_t res = _chsize_s(_fileno(CastAnyPtr(FILE, m_file)), size);

	if (res)
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::SetSize"), res);
#endif
}

CFile::TFileOffset CStreamFile::GetFilePos()
{
	TFileOffset res = -1;

	if (PtrCheck(m_file) || ferror(CastAnyPtr(FILE, m_file)))
		return res;

#ifdef OK_SYS_WINDOWS
	__int64 len = _ftelli64(CastAnyPtr(FILE, m_file));

	if (len < 0)
#endif
#ifdef OK_SYS_UNIX
		long len = ftell(CastAnyPtr(FILE, m_file));

	if (len < 0)
#endif
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::GetFilePos"), errno);
	return len;
}

void CStreamFile::SetFilePos(TFileOffset pos)
{
	if (PtrCheck(m_file) || ferror(CastAnyPtr(FILE, m_file)))
		return;
#ifdef OK_SYS_WINDOWS
	if (_fseeki64(CastAnyPtr(FILE, m_file), pos, SEEK_SET))
#endif
#ifdef OK_SYS_UNIX
	if (fseek(CastAnyPtr(FILE, m_file), pos, SEEK_SET))
#endif
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::SetFilePos"), errno);
}

WBool CStreamFile::IsEOF()
{
	if (PtrCheck(m_file) || ferror(CastAnyPtr(FILE, m_file)))
		return true;

	errno = 0;

	int res = feof(CastAnyPtr(FILE, m_file));

	if (errno)
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::IsEOF"), errno);
	return (res > 0);
}

void CStreamFile::Read(Ref(CByteBuffer) _buffer)
{
	if (PtrCheck(m_file) || ferror(CastAnyPtr(FILE, m_file)))
	{
		_buffer.set_BufferSize(__FILE__LINE__ 0);
		return;
	}

	size_t cnt = fread(_buffer.get_Buffer(), 1, _buffer.get_BufferSize(), CastAnyPtr(FILE, m_file));

	if (ferror(CastAnyPtr(FILE, m_file)))
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::Read"), errno);
	_buffer.set_BufferSize(__FILE__LINE__ Cast(dword, cnt));
}

void CStreamFile::Read(Ref(CByteLinkedBuffer) _buffer)
{
	dword ix = 0;
	dword max = _buffer.GetBufferItemCount();

	if (!(PtrCheck(m_file) || ferror(CastAnyPtr(FILE, m_file))))
	{
		for (; ix < max; ++ix)
		{
			dword sz;
			BPointer p = _buffer.GetBufferItem(ix, &sz);
			size_t cnt = fread(p, 1, sz, CastAnyPtr(FILE, m_file));

			if (ferror(CastAnyPtr(FILE, m_file)))
				throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
				_T("CStreamFile::Read"), errno);
			if (cnt == 0)
				break;
			_buffer.SetBufferItemSize(ix, Cast(dword, cnt));
		}
	}
	for (; ix < max; ++ix)
		_buffer.SetBufferItemSize(ix, 0);
}

void CStreamFile::Write(ConstRef(CByteBuffer) _buffer)
{
	if (PtrCheck(m_file) || ferror(CastAnyPtr(FILE, m_file)))
		return;

	size_t sz = _buffer.get_BufferSize();
	BPointer p = _buffer.get_Buffer();

	if (fwrite(p, 1, sz, CastAnyPtr(FILE, m_file)) < sz)
		throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
		_T("CStreamFile::Write"), errno);
}

void CStreamFile::Write(ConstRef(CByteLinkedBuffer) _buffer)
{
	if (PtrCheck(m_file) || ferror(CastAnyPtr(FILE, m_file)))
		return;

	for (dword ix = 0; ix < _buffer.GetBufferItemCount(); ++ix)
	{
		dword sz;
		BPointer p = _buffer.GetBufferItem(ix, &sz);

		if (fwrite(p, 1, sz, CastAnyPtr(FILE, m_file)) < sz)
			throw OK_NEW_OPERATOR CFileException(__FILE__LINE__ _T("in %s CFileException"),
			_T("CStreamFile::Write"), errno);
	}
}
