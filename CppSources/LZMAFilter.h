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
 *  \file LZMAFilter.h
 *  \brief LZMA compressor and decompressor filters.
 */
#pragma once

#include "CppSources.h"
#include "LZMALibFilter.h"

/**
 *  \class CLZMACompressFilter
 *  \brief LZMA compressor filter
 */
class CPPSOURCES_API CLZMACompressFilter :
	public CLZMALibCompressFilter
{
public:
	/**
	 *  \brief Constructor
	 *  \param input any filter input
	 *  \param output any filter output
	 *  \details Initializes the object.
	 */
	CLZMACompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Does nothing.
	 */
	virtual ~CLZMACompressFilter(void);

	/**
	 *  \brief Calls CFilter::open and initializes the LZMA compressor.
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function has been overwritten.
	 */
	virtual void open();
	/**
	 *  \brief Perform compression.
	 *  
	 *  \return dword result code of the operation.
	 *  
	 *  \details Pure virtual function has been overwritten.
	 */
	virtual dword do_filter();

private:
	CLZMACompressFilter(void);
};

/**
 *  \class CLZMADeCompressFilter
 *  \brief LZMA decompressor filter
 */
class CPPSOURCES_API CLZMADeCompressFilter :
	public CLZMALibDeCompressFilter
{
public:
	/**
	 *  \brief Constructor
	 *  \param input any filter input
	 *  \param output any filter output
	 *  \details Initializes the object.
	 */
	CLZMADeCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Does nothing.
	 */
	virtual ~CLZMADeCompressFilter(void);

	/**
	 *  \brief Calls CFilter::open and initializes the LZMA decompressor.
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function has been overwritten.
	 */
	virtual void open();
	/**
	 *  \brief Perform compression.
	 *  
	 *  \return dword result code of the operation.
	 *  
	 *  \details Pure virtual function has been overwritten.
	 */
	virtual dword do_filter();

private:
	CLZMADeCompressFilter(void);
};

