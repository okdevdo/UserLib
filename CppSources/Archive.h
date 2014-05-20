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
 *  \file Archive.h
 *  \brief Archive management.
 */
#pragma once

#include "CppSources.h"
#include "File.h"
#include "DataHashtable.h"

class CPPSOURCES_API CArchive;
/**
 *  \class CArchiveFile
 *  \brief Overloaded version of CFile, representing a file object in the archive.
 */
class CPPSOURCES_API CArchiveFile: public CFile
{
public:
	/**
	 *  \brief Constructor
	 *  
	 *  \param [in] _archive path of the file object
	 *  \param [in] _fLength read only flag
	 *  
	 *  \details Initializes the CArchiveFile object.
	 */
	CArchiveFile(Ref(CArchive) _archive, dword _fLength);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Frees allocated memory.
	 */
	virtual ~CArchiveFile();

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
	 *  \details Virtual overload. Not implemented.
	 */
	virtual void Read(Ref(CByteLinkedBuffer) _buffer);
	/**
	 *  \brief Write buffer
	 *  
	 *  \param [in] _buffer data to be written
	 *  \return void.
	 *  
	 *  \details Virtual overload. Not implemented.
	 */
	virtual void Write(ConstRef(CByteBuffer) _buffer);
	/**
	 *  \brief Write buffer
	 *  
	 *  \param [in] _buffer data to be written
	 *  \return void.
	 *  
	 *  \details Virtual overload. Not implemented.
	 */
	virtual void Write(ConstRef(CByteLinkedBuffer) _buffer);

protected:
	Ref(CArchive) m_archive;
	dword m_length;
	dword m_offset;

private:
	CArchiveFile();
	CArchiveFile(ConstRef(CArchiveFile));
	Ref(CArchiveFile) operator=(ConstRef(CArchiveFile));
};

/**
 *  \class CArchiveProperty
 *  \brief To extract arbitrary data from an archive, this class is defined.
 */
class CPPSOURCES_API CArchiveProperty
{
public:
    /**
	 *  \brief Standard constructor
	 *  
	 *  \details Initializes the object.
	 */
	CArchiveProperty(): m_name(), m_value(0), m_isNull(true) {}
    /**
	 *  \brief Constructor
	 *  
	 *  \param [in] name name of the property
	 *  \param [in] value value of the property
	 *  
	 *  \details Initializes the object.
	 */
	CArchiveProperty(CStringLiteral name, sqword value): m_name(name), m_value(value), m_isNull(false) {}
    /**
	 *  \brief Copy constructor
	 *  
	 *  \param [in] copy object to be copied
	 *  
	 *  \details Initializes the object.
	 */
	CArchiveProperty(ConstRef(CArchiveProperty) copy): m_name(copy.m_name), m_value(copy.m_value), m_isNull(copy.m_isNull) {}

	/**
	 *  \brief Assignment operator
	 *  
	 *  \param [in] copy object to be copied
	 *  \return Ref(CArchiveProperty) *this
	 *  
	 *  \details Assignment operator.
	 */
	__inline Ref(CArchiveProperty) operator = (ConstRef(CArchiveProperty) copy) { m_name = copy.m_name; m_value = copy.m_value; m_isNull = copy.m_isNull; return *this; }

	/**
	 *  \brief Getter name of the property
	 *  
	 *  \return CStringLiteral
	 *  
	 *  \details Getter name of the property.
	 */
	__inline CStringLiteral key() const { return m_name; }
	/**
	 *  \brief Getter value of the property
	 *  
	 *  \return sqword
	 *  
	 *  \details Getter value of the property.
	 */
	__inline sqword value() const { return m_value; }
	/**
	 *  \brief Test on null
	 *  
	 *  \return bool
	 *  
	 *  \details Test on null.
	 */
	__inline bool isNull() const { return m_isNull; }

private:
	CStringLiteral m_name;
	sqword m_value;
	bool m_isNull;
};

/**
 *  \class CArchiveProperties
 *  \brief A hash list of CArchiveProperty objects.
 */
class CPPSOURCES_API CArchiveProperties
{
public:
	/**
	 *  \brief Constructor
	 *  
	 *  \param [in] maxEntries size of the hash list
	 *  
	 *  \details Initializes the object.
	 */
	CArchiveProperties(dword maxEntries = 64);

	/**
	 *  \brief Getter property
	 *  
	 *  \param [in] name name of the property
	 *  \param [out] value value of the property
	 *  \param [out] isNull test on null
	 *  \return void
	 *  
	 *  \details Getter property.
	 */
	void GetProperty(CStringLiteral name, Ref(sqword) value, Ref(bool) isNull) const;
	/**
	 *  \brief Setter property
	 *  
	 *  \param [in] name name of the property
	 *  \param [in] value value of the property
	 *  \return void
	 *  
	 *  \details Setter property.
	 */
	void SetProperty(CStringLiteral name, sqword value);
	/**
	 *  \brief Clear the list of properties
	 *  
	 *  \return void
	 *  
	 *  \details Clear the list of properties.
	 */
	void ClearProperties();

protected:
	CHashLinkedListT<CArchiveProperty, CStringLiteral, HashFunctorString> m_properties;

private:
	CArchiveProperties(ConstRef(CArchiveProperties));
	Ref(CArchiveProperties) operator = (ConstRef(CArchiveProperties) copy);
};

/**
 *  \class CArchiveIterator
 *  \brief An iterator, traversing the objects in an archive. Pure virtual base class.
 */
class CPPSOURCES_API CArchiveIterator: public CCppObject
{
public:
	typedef enum
	{
		ARCHIVE_FILE_INVALID,
		ARCHIVE_FILE_REGULAR,
		ARCHIVE_FILE_HARDLINK,
		ARCHIVE_FILE_SYMLINK,
		ARCHIVE_FILE_DIRECTORY
	}
	archive_file_t;

	/**
	 *  \brief Constructor
	 *  
	 *  \param [in] _archive the archive, which the iterator has to traverse
	 *  
	 *  \details Initializes the object.
	 */
	CArchiveIterator(Ref(CArchive) _archive);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Frees allocated memory.
	 */
	virtual ~CArchiveIterator();

	/**
	 *  \brief Getter type of current archive item
	 *  
	 *  \return archive_file_t
	 *  
	 *  \details Pure virtual overload. Getter type of current archive item.
	 */
	virtual archive_file_t GetType() const = 0;
	/**
	 *  \brief Getter file name of current archive item
	 *  
	 *  \return CStringBuffer
	 *  
	 *  \details Pure virtual overload. Getter file name of current archive item.
	 */
	virtual CStringBuffer GetFileName() const = 0;
	/**
	 *  \brief Getter link name of current archive item
	 *  
	 *  \return CStringBuffer
	 *  
	 *  \details Getter link name of current archive item, in case the arcxhive item is a link.
	 */
	virtual CStringBuffer GetLinkName() const;
	/**
	 *  \brief Getter CArchiveFile
	 *  
	 *  \return Ptr(CArchiveFile)
	 *  
	 *  \details Pure virtual overload. Returns a new CArchiveFile, which represents a file object in the archive.
	 */
	virtual Ptr(CArchiveFile) GetFile() = 0;

	/**
	 *  \brief Getter property
	 *  
	 *  \param [in] name name of the property
	 *  \param [out] value value of the property
	 *  \param [out] isNull test on null
	 *  \return void
	 *  
	 *  \details Getter property.
	 */
	void GetProperty(CStringLiteral name, Ref(sqword) value, Ref(bool) isNull);
	/**
	 *  \brief Setter property
	 *  
	 *  \param [in] name name of the property
	 *  \param [in] value value of the property
	 *  \return void
	 *  
	 *  \details Setter property.
	 */
	void SetProperty(CStringLiteral name, sqword value);
	/**
	 *  \brief Clear the list of properties
	 *  
	 *  \return void
	 *  
	 *  \details Clear the list of properties.
	 */
	void ClearProperties();

	/**
	 *  \brief Advances the iterator
	 *  
	 *  \return bool whether the operation was successful
	 *  
	 *  \details Pure virtual overload. Advances the iterator to the next object in the archive, if any.
	 */
	virtual bool Next() = 0;
	/**
	 *  \brief Skip the current object in the archive
	 *  
	 *  \return void
	 *  
	 *  \details Pure virtual overload. Skip the current object in the archive.
	 */
	virtual void Skip() = 0;

protected:
	Ref(CArchive) m_archive;
	CArchiveProperties m_properties;

private:
	CArchiveIterator();
	CArchiveIterator(ConstRef(CArchiveIterator));
	Ref(CArchiveIterator) operator=(ConstRef(CArchiveIterator));
};

/**
 *  \class CArchive
 *  \brief The pure virtual base class of all archives.
 */
class CPPSOURCES_API CArchive: public CCppObject
{
public:
	/**
	 *  \brief Constructor
	 *  
	 *  \param [in] _file the archive as a disk file
	 *  
	 *  \details Initializes the object.
	 */
	CArchive(Ptr(CFile) _file);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Frees allocated memory.
	 */
	virtual ~CArchive();

	/**
	 *  \brief Getter archive file
	 *  
	 *  \return Ptr(CFile)
	 *  
	 *  \details Getter archive file.
	 */
	__inline Ptr(CFile) GetFile() { return m_file; }
	/**
	 *  \brief Setter archive file
	 *  
	 *  \param [in] _file new file
	 *  \return void
	 *  
	 *  \details Setter archive file.
	 */
	__inline void SetFile(Ptr(CFile) _file) { m_file = _file; }

	/**
	 *  \brief Returns a new archive iterator
	 *  
	 *  \return Ptr(CArchiveIterator)
	 *  
	 *  \details Pure virtual overload. Returns a new archive iterator.
	 */
	virtual Ptr(CArchiveIterator) begin() = 0;

	/**
	 *  \brief Open the archive for manipulation
	 *  
	 *  \return void
	 *  
	 *  \details Virtual overload. Open the archive to add or remove archive items.
	 */
	virtual void AddOpen();
	/**
	 *  \brief Add file object to archive
	 *  
	 *  \param [in] fname path to new file object
	 *  \param [in] props properties associated with the file object
	 *  \return void
	 *  
	 *  \details Virtual overload. Add file object to archive.
	 */
	virtual void AddFile(ConstRef(CFilePath) fname, ConstRef(CArchiveProperties) props);
	/**
	 *  \brief Delete file object from archive
	 *  
	 *  \param [in] fname path to archive object to be deleted
	 *  \return void
	 *  
	 *  \details Virtual overload. Delete file object from archive.
	 */
	virtual void AddDelete(ConstRef(CFilePath) fname);
	/**
	 *  \brief Add directory object to archive
	 *  
	 *  \param [in] fname path of new directory object
	 *  \param [in] props properties associated with the directory object
	 *  \return void
	 *  
	 *  \details Virtual overload. Add directory object to archive.
	 */
	virtual void AddDirectory(ConstRef(CFilePath) fname, ConstRef(CArchiveProperties) props);
	/**
	 *  \brief Add a link object to the archive
	 *  
	 *  \param [in] fname path of the new link object
	 *  \param [in] lname link path
	 *  \param [in] props properties associated with the link object
	 *  \param [in] bHardLink
	 *  \return void
	 *  
	 *  \details Virtual overload. Add a link object to the archive.
	 */
	virtual void AddLink(ConstRef(CFilePath) fname, ConstRef(CFilePath) lname, ConstRef(CArchiveProperties) props, bool bHardLink = false);
	/**
	 *  \brief Close the archive after manipulation
	 *  
	 *  \return void
	 *  
	 *  \details Virtual overload. Close the archive after manipulation.
	 */
	virtual void AddClose();

protected:
	Ptr(CFile) m_file;

private:
	CArchive();
	CArchive(ConstRef(CArchive));
	Ref(CArchive) operator=(ConstRef(CArchive));
};

DECL_EXCEPTION(CPPSOURCES_API, CArchiveException, CBaseException)
