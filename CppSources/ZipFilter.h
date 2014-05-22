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
 *  \file ZipFilter.h
 *  \brief Zip compressor and decompressor filters
 */
#pragma once

#include "CppSources.h"
#include "ZLibFilter.h"

/**
 *  \class CZipCompressFilter
 *  \brief Zip compressor filter
 */
class CPPSOURCES_API CZipCompressFilter :
	public CZLibCompressFilter
{
public:
	/**
	 *  \brief Constructor
	 *  \param input any filter input
	 *  \param output any filter output
	 *  \details Initializes the object.
	 */
	CZipCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Does nothing.
	 */
	virtual ~CZipCompressFilter(void);

	/**
	 *  \brief Calls CFilter::open and initializes the Zip compressor.
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function has been overwritten.
	 */
	virtual void open();

	/**
	 *  \brief Read the input with the read function and calculate the CRC32 value.
	 *  
	 *  \param [out] inputbuf data to be read in
	 *  \return void.
	 *  
	 *  \details Virtual function has been overwritten.
	 */
	virtual void read(Ref(CByteBuffer) inputbuf);
	/**
	 *  \brief Write to the output with the write function and calculate the compressed size.
	 *  
	 *  \param [in] outputbuf data to be written
	 *  \return void.
	 *  
	 *  \details Virtual function has been overwritten.
	 */
	virtual void write(Ref(CByteBuffer) outputbuf);

	/**
	 *  \brief Retrieves the calculated CRC32 value.
	 *  
	 *  \return dword.
	 */
	__inline dword get_crc32() const { return m_crc32; }
	/**
	 *  \brief Retrieves the calculated compressed size.
	 *  
	 *  \return dword.
	 */
	__inline dword get_compressedSize() const { return m_compressedSize; }

private:
	CZipCompressFilter(void);
	dword m_crc32;
	dword m_compressedSize;
};

/**
 *  \class CZipDeCompressFilter
 *  \brief Zip decompressor filter
 */
class CPPSOURCES_API CZipDeCompressFilter :
	public CZLibDeCompressFilter
{
public:
	/**
	 *  \brief Constructor
	 *  \param input any filter input
	 *  \param output any filter output
	 *  \details Initializes the object.
	 */
	CZipDeCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Does nothing.
	 */
	virtual ~CZipDeCompressFilter(void);

	/**
	 *  \brief Calls CFilter::open and initializes the Zip decompressor.
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function has been overwritten.
	 */
	virtual void open();

	/**
	 *  \brief Write to the output with the write function and calculate CRC32 value and uncompressed size.
	 *  
	 *  \param [in] outputbuf data to be written
	 *  \return void.
	 *  
	 *  \details Virtual function has been overwritten.
	 */
	virtual void write(Ref(CByteBuffer) outputbuf);

	/**
	 *  \brief Retrieves the calculated CRC32 value.
	 *  
	 *  \return dword.
	 */
	__inline dword get_crc32() const { return m_crc32; }
	/**
	 *  \brief Retrieves the calculated uncompressed size.
	 *  
	 *  \return dword.
	 */
	__inline dword get_unCompressedSize() const { return m_unCompressedSize; }

private:
	CZipDeCompressFilter(void);
	dword m_crc32;
	dword m_unCompressedSize;
};

