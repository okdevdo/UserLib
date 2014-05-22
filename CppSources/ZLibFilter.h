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
 *  \file ZLibFilter.h
 *  \brief ZLib compressor and decompressor filters
 */
#pragma once

#include "CppSources.h"
#include "Filter.h"

/**
 *  \class CZLibCompressFilter
 *  \brief ZLib compressor filter
 */
class CPPSOURCES_API CZLibCompressFilter :
	public CFilter
{
public:
	/**
	 *  \brief Constructor
	 *  \param input any filter input
	 *  \param output any filter output
	 *  \details Initializes the object.
	 */
	CZLibCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Does nothing.
	 */
	virtual ~CZLibCompressFilter(void);

	/**
	 *  \brief Calls CFilter::open and initializes the ZLib compressor.
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
	CZLibCompressFilter(void);
};

/**
 *  \class CZLibDeCompressFilter
 *  \brief ZLib decompressor filter
 */
class CPPSOURCES_API CZLibDeCompressFilter :
	public CFilter
{
public:
	/**
	 *  \brief Constructor
	 *  \param input any filter input
	 *  \param output any filter output
	 *  \details Initializes the object.
	 */
	CZLibDeCompressFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Does nothing.
	 */
	virtual ~CZLibDeCompressFilter(void);

	/**
	 *  \brief Calls CFilter::open and initializes the ZLib decompressor.
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
	CZLibDeCompressFilter(void);
};

