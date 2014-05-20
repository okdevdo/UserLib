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
 *  \file File.h
 *  \brief Disk-IO Low-Level Management
 */
#pragma once

#include "CppSources.h"
#include "ByteLinkedBuffer.h"
#include "FilePath.h"
#include "BaseException.h"

/**
 *  \class CFile
 *  \brief Pure virtual base class for disk file management
 */
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

	/**
	 *  \brief Standard constructor
	 *  
	 *  \details Initializes the CFile object.
	 */
	CFile(void);
	/**
	 *  \brief Constructor
	 *  
	 *  \param [in] _path path of the file object
	 *  \param [in] _readOnly read only flag
	 *  \param [in] _textMode controls the automatic transformation of line endings
	 *  \param [in] _encoding 
	 *  
	 *  \details Initializes the CFile object. Calls Open.
	 */
	CFile(ConstRef(CFilePath) _path, WBool _readOnly = true, WBool _textMode = true, TEncoding _encoding = ScanFor_Encoding);
	/**
	 *  \brief Copy constructor
	 *  
	 *  \param [in] _file object to be copied
	 *  
	 *  \details Initializes the CFile object.
	 */
	CFile(ConstRef(CFile) _file);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Frees allocated memory.
	 */
	virtual ~CFile(void);

	/**
	 *  \brief Assignment operator
	 *  
	 *  \param [in] _file object to be copied
	 *  \return void.
	 *  
	 *  \details Assignment operator.
	 */
	void operator = (ConstRef(CFile) _file);

	/**
	 *  \brief Opens the file object
	 *  
	 *  \param [in] _path path of the file object
	 *  \param [in] _readOnly read only flag
	 *  \param [in] _textMode controls the automatic transformation of line endings
	 *  \param [in] _encoding 
	 *  \return void.
	 *  
	 *  \details Virtual overload. Opens the file object.
	 */
	virtual void Open(ConstRef(CFilePath) _path, WBool _readOnly = true, WBool _textMode = true, TEncoding _encoding = ScanFor_Encoding);
	/**
	 *  \brief Creates a new file object
	 *  
	 *  \param [in] _path path of the file object
	 *  \param [in] _textMode controls the automatic transformation of line endings
	 *  \param [in] _encoding 
	 *  \return void.
	 *  
	 *  \details Virtual overload. Creates a new file object.
	 */
	virtual void Create(ConstRef(CFilePath) _path, WBool _textMode = true, TEncoding _encoding = ISO_8859_1_Encoding);
	/**
	 *  \brief Closes the file object
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual overload. Closes the file object.
	 */
	virtual void Close();

	/**
	 *  \brief Getter file path
	 *  
	 *  \return ConstRef(CFilePath)
	 *  
	 *  \details Getter file path.
	 */
	__inline ConstRef(CFilePath) GetPath() const { return m_path; }
	/**
	 *  \brief Getter read only flag
	 *  
	 *  \return WBool
	 *  
	 *  \details Getter read only flag.
	 */
	__inline WBool IsReadOnly() const { return m_readOnly; }
	/**
	 *  \brief Getter text mode
	 *  
	 *  \return WBool
	 *  
	 *  \details Getter text mode.
	 */
	__inline WBool GetTextMode() const { return m_textMode; }
	/**
	 *  \brief Getter encoding
	 *  
	 *  \return TEncoding
	 *  
	 *  \details Getter encoding.
	 */
	__inline TEncoding GetEncoding() const { return m_encoding; }

	/**
	 *  \brief Getter file size
	 *  
	 *  \return TFileSize.
	 *  
	 *  \details Pure virtual overload. Getter file size.
	 */	
	virtual TFileSize GetSize() = 0;
	/**
	 *  \brief Setter file size
	 *  
	 *  \param [in] sz new size of the file object
	 *  \return void.
	 *  
	 *  \details Pure virtual overload. Setter file size.
	 */
	virtual void SetSize(TFileSize sz) = 0;
	/**
	 *  \brief Getter file read and write position
	 *  
	 *  \return TFileOffset.
	 *  
	 *  \details Pure virtual overload. Getter file read and write position.
	 */
	virtual TFileOffset GetFilePos() = 0;
	/**
	 *  \brief Setter file read and write position
	 *  
	 *  \param [in] off new file position
	 *  \return void.
	 *  
	 *  \details Pure virtual overload. Setter file read and write position.
	 */
	virtual void SetFilePos(TFileOffset off) = 0;
	/**
	 *  \brief Test end of file
	 *  
	 *  \return WBool
	 *  
	 *  \details Test end of file.
	 */
	virtual WBool IsEOF();

	/**
	 *  \brief Read buffer
	 *  
	 *  \param [out] _buffer holds data to be read in
	 *  \return void.
	 *  
	 *  \details Pure virtual overload. Read buffer. Set buffer size before calling \c Read. After the call the buffer size holds the number of bytes read in.
	 */
	virtual void Read(Ref(CByteBuffer) _buffer) = 0;
	/**
	 *  \brief Read buffer
	 *  
	 *  \param [out] _buffer holds data to be read in
	 *  \return void.
	 *  
	 *  \details Pure virtual overload. Read buffer. Set buffer size before calling \c Read. After the call the buffer size holds the number of bytes read in.
	 */
	virtual void Read(Ref(CByteLinkedBuffer) _buffer) = 0;
	/**
	 *  \brief Write buffer
	 *  
	 *  \param [in] _buffer data to be written
	 *  \return void.
	 *  
	 *  \details Pure virtual overload. Write buffer.
	 */
	virtual void Write(ConstRef(CByteBuffer) _buffer) = 0;
	/**
	 *  \brief Write buffer
	 *  
	 *  \param [in] _buffer data to be written
	 *  \return void.
	 *  
	 *  \details Pure virtual overload. Write buffer.
	 */
	virtual void Write(ConstRef(CByteLinkedBuffer) _buffer) = 0;

	/**
	 *  \brief Read buffer
	 *  
	 *  \param [out] _buffer holds data to be read in
	 *  \param [in] _bufferSize number of bytes to be read in
	 *  \return void.
	 *  
	 *  \details Virtual overload. Read buffer. \c _bufferSize bytes will be read in. Then this data will be transformed in respect to the encoding settings and written to \c _buffer.
	 */
	virtual void Read(Ref(CStringBuffer) _buffer, WULong _bufferSize);
	/**
	 *  \brief Write buffer
	 *  
	 *  \param [in] _buffer data to be written
	 *  \return void.
	 *  
	 *  \details Virtual overload. Write buffer. In respect to the encoding settings the input data will be transformed to byte data, which will be written to disk.
	 */
	virtual void Write(ConstRef(CStringBuffer) _buffer);

	/**
	 *  \brief Read buffer
	 *  
	 *  \param [in] _format determines data to read in
	 *  \return void.
	 *  
	 *  \details Virtual overload. Read buffer. 
	 */
	virtual void Read(CConstPointer _format, ...);
	/**
	 *  \brief Write buffer
	 *  
	 *  \param [in] _format determines data to written out
	 *  \return void.
	 *  
	 *  \details Virtual overload. Write buffer. 
	 */
	virtual void Write(CConstPointer _format, ...);

protected:
	void ScanForEncoding();
	void WriteBOM();

	CFilePath m_path;
	WBool m_readOnly;
	WBool m_textMode;
	TEncoding m_encoding;
};

/**
 *  \class CDiskFile
 *  \brief Overloaded version of CFile, holding an integer file descriptor.
 */
class CPPSOURCES_API CDiskFile : public CFile
{
public:
	/**
	 *  \brief Standard constructor
	 *  
	 *  \details Initializes the CDiskFile object.
	 */
	CDiskFile(void);
	/**
	 *  \brief Constructor
	 *  
	 *  \param [in] _path path of the file object
	 *  \param [in] _readOnly read only flag
	 *  \param [in] _textMode controls the automatic transformation of line endings
	 *  \param [in] _encoding 
	 *  
	 *  \details Initializes the CDiskFile object. Calls Open.
	 */
	CDiskFile(ConstRef(CFilePath) _path, WBool _readOnly = true, WBool _textMode = true, TEncoding _encoding = ScanFor_Encoding);
	/**
	 *  \brief Copy constructor
	 *  
	 *  \param [in] diskfile object to be copied
	 *  
	 *  \details Initializes the CDiskFile object.
	 */
	CDiskFile(ConstRef(CDiskFile) diskfile);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Frees allocated memory.
	 */
	virtual ~CDiskFile(void);

	/**
	 *  \brief Assignment operator
	 *  
	 *  \param [in] diskfile object to be copied
	 *  \return void.
	 *  
	 *  \details Assignment operator.
	 */
	void operator = (ConstRef(CDiskFile) diskfile);

	/**
	 *  \brief Opens the file object
	 *  
	 *  \param [in] _path path of the file object
	 *  \param [in] _readOnly read only flag
	 *  \param [in] _textMode controls the automatic transformation of line endings
	 *  \param [in] _encoding 
	 *  \return void.
	 *  
	 *  \details Virtual overload. Opens the file object.
	 */
	virtual void Open(ConstRef(CFilePath) _path, WBool _readOnly = true, WBool _textMode = true, TEncoding _encoding = ScanFor_Encoding);
	/**
	 *  \brief Creates a new file object
	 *  
	 *  \param [in] _path path of the file object
	 *  \param [in] _textMode controls the automatic transformation of line endings
	 *  \param [in] _encoding 
	 *  \return void.
	 *  
	 *  \details Virtual overload. Creates a new file object.
	 */
	virtual void Create(ConstRef(CFilePath) _path, WBool _textMode = true, TEncoding _encoding = ISO_8859_1_Encoding);
	/**
	 *  \brief Closes the file object
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual overload. Closes the file object.
	 */
	virtual void Close();

	/**
	 *  \brief Getter file size
	 *  
	 *  \return TFileSize.
	 *  
	 *  \details Virtual overload. Getter file size.
	 */	
	virtual TFileSize GetSize();
	/**
	 *  \brief Setter file size
	 *  
	 *  \param [in] sz new size of the file object
	 *  \return void.
	 *  
	 *  \details Virtual overload. Setter file size.
	 */
	virtual void SetSize(TFileSize sz);
	/**
	 *  \brief Getter file read and write position
	 *  
	 *  \return TFileOffset.
	 *  
	 *  \details Virtual overload. Getter file read and write position.
	 */
	virtual TFileOffset GetFilePos();
	/**
	 *  \brief Setter file read and write position
	 *  
	 *  \param [in] off new file position
	 *  \return void.
	 *  
	 *  \details Virtual overload. Setter file read and write position.
	 */
	virtual void SetFilePos(TFileOffset off);
	/**
	 *  \brief Test end of file
	 *  
	 *  \return WBool
	 *  
	 *  \details Test end of file.
	 */
	virtual WBool IsEOF();

	/**
	 *  \brief Read buffer
	 *  
	 *  \param [out] _buffer holds data to be read in
	 *  \return void.
	 *  
	 *  \details Virtual overload. Read buffer. Set buffer size before calling \c Read. After the call the buffer size holds the number of bytes read in.
	 */
	virtual void Read(Ref(CByteBuffer) _buffer);
	/**
	 *  \brief Read buffer
	 *  
	 *  \param [out] _buffer holds data to be read in
	 *  \return void.
	 *  
	 *  \details Virtual overload. Read buffer. Set buffer size before calling \c Read. After the call the buffer size holds the number of bytes read in.
	 */
	virtual void Read(Ref(CByteLinkedBuffer) _buffer);
	/**
	 *  \brief Write buffer
	 *  
	 *  \param [in] _buffer data to be written
	 *  \return void.
	 *  
	 *  \details Virtual overload. Write buffer.
	 */
	virtual void Write(ConstRef(CByteBuffer) _buffer);
	/**
	 *  \brief Write buffer
	 *  
	 *  \param [in] _buffer data to be written
	 *  \return void.
	 *  
	 *  \details Virtual overload. Write buffer.
	 */
	virtual void Write(ConstRef(CByteLinkedBuffer) _buffer);

protected:
	int m_fd;
};

/**
 *  \class CStreamFile
 *  \brief Overloaded version of CFile, holding a \c FILE object.
 */
class CPPSOURCES_API CStreamFile : public CFile
{
public:
	/**
	 *  \brief Standard constructor
	 *  
	 *  \details Initializes the CStreamFile object.
	 */
	CStreamFile(void);
	/**
	 *  \brief Constructor
	 *  
	 *  \param [in] _path path of the file object
	 *  \param [in] _readOnly read only flag
	 *  \param [in] _textMode controls the automatic transformation of line endings
	 *  \param [in] _encoding 
	 *  
	 *  \details Initializes the CStreamFile object. Calls Open.
	 */
	CStreamFile(ConstRef(CFilePath) _path, WBool _readOnly = true, WBool _textMode = true, TEncoding _encoding = ScanFor_Encoding);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Frees allocated memory.
	 */
	virtual ~CStreamFile(void);

	/**
	 *  \brief Opens the file object
	 *  
	 *  \param [in] _path path of the file object
	 *  \param [in] _readOnly read only flag
	 *  \param [in] _textMode controls the automatic transformation of line endings
	 *  \param [in] _encoding 
	 *  \return void.
	 *  
	 *  \details Virtual overload. Opens the file object.
	 */
	virtual void Open(ConstRef(CFilePath) _path, WBool _readOnly = true, WBool _textMode = true, TEncoding _encoding = ScanFor_Encoding);
	/**
	 *  \brief Creates a new file object
	 *  
	 *  \param [in] _path path of the file object
	 *  \param [in] _textMode controls the automatic transformation of line endings
	 *  \param [in] _encoding 
	 *  \return void.
	 *  
	 *  \details Virtual overload. Creates a new file object.
	 */
	virtual void Create(ConstRef(CFilePath) _path, WBool _textMode = true, TEncoding _encoding = ISO_8859_1_Encoding);
	/**
	 *  \brief Reopen a \c FILE object
	 *  
	 *  \param [in] _path path of the file object
	 *  \param [in] stdfile an already defined \c FILE object
	 *  \param [in] _textMode controls the automatic transformation of line endings
	 *  \param [in] _encoding 
	 *  \return Return_Description
	 *  
	 *  \details Reopen a \c FILE object.
	 */
	void ReOpen(ConstRef(CFilePath) _path, Pointer stdfile, WBool _textMode = true, TEncoding _encoding = ISO_8859_1_Encoding);
	/**
	 *  \brief Closes the file object
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual overload. Closes the file object.
	 */
	virtual void Close();

	/**
	 *  \brief Getter file size
	 *  
	 *  \return TFileSize.
	 *  
	 *  \details Virtual overload. Getter file size.
	 */	
	virtual TFileSize GetSize();
	/**
	 *  \brief Setter file size
	 *  
	 *  \param [in] sz new size of the file object
	 *  \return void.
	 *  
	 *  \details Virtual overload. Setter file size.
	 */
	virtual void SetSize(TFileSizes sz);
	/**
	 *  \brief Getter file read and write position
	 *  
	 *  \return TFileOffset.
	 *  
	 *  \details Virtual overload. Getter file read and write position.
	 */
	virtual TFileOffset GetFilePos();
	/**
	 *  \brief Setter file read and write position
	 *  
	 *  \param [in] off new file position
	 *  \return void.
	 *  
	 *  \details Virtual overload. Setter file read and write position.
	 */
	virtual void SetFilePos(TFileOffset off);
	/**
	 *  \brief Test end of file
	 *  
	 *  \return WBool
	 *  
	 *  \details Test end of file.
	 */
	virtual WBool IsEOF();

	/**
	 *  \brief Read buffer
	 *  
	 *  \param [out] _buffer holds data to be read in
	 *  \return void.
	 *  
	 *  \details Virtual overload. Read buffer. Set buffer size before calling \c Read. After the call the buffer size holds the number of bytes read in.
	 */
	virtual void Read(Ref(CByteBuffer) _buffer);
	/**
	 *  \brief Read buffer
	 *  
	 *  \param [out] _buffer holds data to be read in
	 *  \return void.
	 *  
	 *  \details Virtual overload. Read buffer. Set buffer size before calling \c Read. After the call the buffer size holds the number of bytes read in.
	 */
	virtual void Read(Ref(CByteLinkedBuffer) _buffer);
	/**
	 *  \brief Write buffer
	 *  
	 *  \param [in] _buffer data to be written
	 *  \return void.
	 *  
	 *  \details Virtual overload. Write buffer.
	 */
	virtual void Write(ConstRef(CByteBuffer) _buffer);
	/**
	 *  \brief Write buffer
	 *  
	 *  \param [in] _buffer data to be written
	 *  \return void.
	 *  
	 *  \details Virtual overload. Write buffer.
	 */
	virtual void Write(ConstRef(CByteLinkedBuffer) _buffer);

protected:
	Pointer m_file;

private:
	CStreamFile(ConstRef(CStreamFile) sfile);
	void operator = (ConstRef(CStreamFile) sfile);
};

DECL_EXCEPTION(CPPSOURCES_API, CFileException, CBaseException)
