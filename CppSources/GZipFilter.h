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
 *  \file GZipFilter.h
 *  \brief GZip compressor and decompressor filters
 */
#pragma once

#include "CppSources.h"
#include "ZLibFilter.h"

/**
 *  \class CGZipCompressFilter
 *  \brief GZip compressor filter
 */
class CPPSOURCES_API CGZipCompressFilter :
	public CZLibCompressFilter
{
public:
	/**
	 *  \brief Constructor
	 *  \param input any filter input
	 *  \param output any filter output
	 *  \details Initializes the object.
	 */
	CGZipCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Does nothing.
	 */
	virtual ~CGZipCompressFilter(void);

	/**
	 *  \brief Calls CFilter::open and initializes the GZip compressor.
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function has been overwritten.
	 */
	virtual void open();
	/**
	 *  \brief Frees allocated memory and calls CZLibCompressFilter::close.
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function has been overwritten.
	 */
	virtual void close();

private:
	Pointer m_head;

	CGZipCompressFilter(void);
};

/**
 *  \class CGZipDeCompressFilter
 *  \brief GZip decompressor filter
 */
class CPPSOURCES_API CGZipDeCompressFilter :
	public CZLibDeCompressFilter
{
public:
	/**
	 *  \brief Constructor
	 *  \param input any filter input
	 *  \param output any filter output
	 *  \details Initializes the object.
	 */
	CGZipDeCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Does nothing.
	 */
	virtual ~CGZipDeCompressFilter(void);

	/**
	 *  \brief Calls CFilter::open and initializes the GZip decompressor.
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function has been overwritten.
	 */
	virtual void open();

private:

	CGZipDeCompressFilter(void);
};

