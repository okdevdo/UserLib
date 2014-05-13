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
#include "ByteLinkedBuffer.h"
#include "FilePath.h"
#include "BaseException.h"

class CPPSOURCES_API CFile: public CCppObject
{
public:
	typedef sqword TFileOffset;
	typedef qword TFileSize;

	enum TEncoding
	{
		BinaryFile_NoEncoding,
		ISO_8859_1_Encoding,
		US_ASCII_Encoding,
		UTF_8_Encoding,
		UTF_16_Encoding,
		UTF_16LE_Encoding,
		UTF_16BE_Encoding,

		ScanFor_Encoding = 0xff
	};

	CFile(void);
	CFile(ConstRef(CFilePath) _path, WBool _readOnly = true, WBool _textMode = true, TEncoding _encoding = ScanFor_Encoding);
	CFile(ConstRef(CFile) _file);
	virtual ~CFile(void);

	void operator = (ConstRef(CFile) _file);

	virtual void Open(ConstRef(CFilePath) _path, WBool _readOnly = true, WBool _textMode = true, TEncoding _encoding = ScanFor_Encoding);
	virtual void Create(ConstRef(CFilePath) _path, WBool _textMode = true, TEncoding _encoding = ISO_8859_1_Encoding);
	virtual void Close();

	__inline ConstRef(CFilePath) GetPath() const { return m_path; }
	__inline WBool IsReadOnly() const { return m_readOnly; }
	__inline WBool GetTextMode() const { return m_textMode; }
	__inline TEncoding GetEncoding() const { return m_encoding; }

	virtual TFileSize GetSize() = 0;
	virtual void SetSize(TFileSize) = 0;
	virtual TFileOffset GetFilePos() = 0;
	virtual void SetFilePos(TFileOffset) = 0;
	virtual WBool IsEOF();

	virtual void Read(Ref(CByteBuffer) _buffer) = 0;
	virtual void Read(Ref(CByteLinkedBuffer) _buffer) = 0;
	virtual void Write(ConstRef(CByteBuffer) _buffer) = 0;
	virtual void Write(ConstRef(CByteLinkedBuffer) _buffer) = 0;

	virtual void Read(Ref(CStringBuffer) _buffer, WULong _bufferSize);
	virtual void Write(ConstRef(CStringBuffer) _buffer);

	virtual void Read(CConstPointer _format, ...);
	virtual void Write(CConstPointer _format, ...);

protected:
	void ScanForEncoding();
	void WriteBOM();

	CFilePath m_path;
	WBool m_readOnly;
	WBool m_textMode;
	TEncoding m_encoding;
};

class CPPSOURCES_API CDiskFile : public CFile
{
public:
	CDiskFile(void);
	CDiskFile(ConstRef(CFilePath) _path, WBool _readOnly = true, WBool _textMode = true, TEncoding _encoding = ScanFor_Encoding);
	CDiskFile(ConstRef(CDiskFile) diskfile);
	virtual ~CDiskFile(void);

	void operator = (ConstRef(CDiskFile) diskfile);

	virtual void Open(ConstRef(CFilePath) _path, WBool _readOnly = true, WBool _textMode = true, TEncoding _encoding = ScanFor_Encoding);
	virtual void Create(ConstRef(CFilePath) _path, WBool _textMode = true, TEncoding _encoding = ISO_8859_1_Encoding);
	virtual void Close();

	virtual TFileSize GetSize();
	virtual void SetSize(TFileSize);
	virtual TFileOffset GetFilePos();
	virtual void SetFilePos(TFileOffset);
	virtual WBool IsEOF();

	virtual void Read(Ref(CByteBuffer) _buffer);
	virtual void Read(Ref(CByteLinkedBuffer) _buffer);
	virtual void Write(ConstRef(CByteBuffer) _buffer);
	virtual void Write(ConstRef(CByteLinkedBuffer) _buffer);

protected:
	int m_fd;
};

class CPPSOURCES_API CStreamFile : public CFile
{
public:
	CStreamFile(void);
	CStreamFile(ConstRef(CFilePath) _path, WBool _readOnly = true, WBool _textMode = true, TEncoding _encoding = ScanFor_Encoding);
	virtual ~CStreamFile(void);

	virtual void Open(ConstRef(CFilePath) _path, WBool _readOnly = true, WBool _textMode = true, TEncoding _encoding = ScanFor_Encoding);
	virtual void Create(ConstRef(CFilePath) _path, WBool _textMode = true, TEncoding _encoding = ISO_8859_1_Encoding);
	void ReOpen(ConstRef(CFilePath) _path, Pointer stdfile, WBool _textMode = true, TEncoding _encoding = ISO_8859_1_Encoding);
	virtual void Close();

	virtual TFileSize GetSize();
	virtual void SetSize(TFileSize);
	virtual TFileOffset GetFilePos();
	virtual void SetFilePos(TFileOffset);
	virtual WBool IsEOF();

	virtual void Read(Ref(CByteBuffer) _buffer);
	virtual void Read(Ref(CByteLinkedBuffer) _buffer);
	virtual void Write(ConstRef(CByteBuffer) _buffer);
	virtual void Write(ConstRef(CByteLinkedBuffer) _buffer);

protected:
	Pointer m_file;

private:
	CStreamFile(ConstRef(CStreamFile) sfile);
	void operator = (ConstRef(CStreamFile) sfile);
};

DECL_EXCEPTION(CPPSOURCES_API, CFileException, CBaseException)
