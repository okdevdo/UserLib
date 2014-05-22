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
 *  \file LZMALibFilter.h
 *  \brief Bzip2 compressor and decompressor filters
 */
#pragma once

#include "CppSources.h"
#include "Filter.h"

/**
 *  \class CLZMALibCompressFilter
 *  \brief Compressor filter
 */
class CPPSOURCES_API CLZMALibCompressFilter :
	public CFilter
{
public:
	/**
	 *  \brief Constructor
	 *  \param input any filter input
	 *  \param output any filter output
	 *  \details Initializes the object.
	 */
	CLZMALibCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Does nothing.
	 */
	virtual ~CLZMALibCompressFilter();

	/**
	 *  \brief Perform compression.
	 *  
	 *  \return dword result code of the operation.
	 *  
	 *  \details Pure virtual function has been overwritten.
	 */
	virtual dword do_filter();
	/**
	 *  \brief Frees allocated memory and calls CFilter::close.
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function has been overwritten.
	 */
	virtual void close();

protected:
	Pointer m_stream;

private:
	CLZMALibCompressFilter(void);
};

/**
 *  \class CLZMALibDeCompressFilter
 *  \brief DeCompressor filter
 */
class CPPSOURCES_API CLZMALibDeCompressFilter :
	public CFilter
{
public:
	/**
	 *  \brief Constructor
	 *  \param input any filter input
	 *  \param output any filter output
	 *  \details Initializes the object.
	 */
	CLZMALibDeCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Does nothing.
	 */
	virtual ~CLZMALibDeCompressFilter();

	/**
	 *  \brief Perform compression.
	 *  
	 *  \return dword result code of the operation.
	 *  
	 *  \details Pure virtual function has been overwritten.
	 */
	virtual dword do_filter();
	/**
	 *  \brief Frees allocated memory and calls CFilter::close.
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function has been overwritten.
	 */
	virtual void close();

protected:
	Pointer m_stream;

private:
	CLZMALibDeCompressFilter(void);
};

