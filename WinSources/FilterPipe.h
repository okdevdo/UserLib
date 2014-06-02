/******************************************************************************
    
	This file is part of WinSources, which is part of UserLib.

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

#include "DataVector.h"
#include "Filter.h"
#include "WinSources.h"
#include "Thread.h"

class WINSOURCES_API CFilterPipeConnector : public CCppObject, public virtual CAbstractFilterInput, public virtual CAbstractFilterOutput
{
public:
	CFilterPipeConnector();
	virtual ~CFilterPipeConnector();

	virtual void open();
	virtual CFile::TFileSize size();
	virtual void read(Ref(CByteBuffer) inputbuf);
	virtual void write(Ref(CByteBuffer) outputbuf);
	virtual void close();

protected:
	CByteLinkedBuffer m_buffer;
};

class WINSOURCES_API CFilterThreadedPipeConnector : public CCppObject, public virtual CAbstractFilterInput, public virtual CAbstractFilterOutput
{
public:
	CFilterThreadedPipeConnector();
	virtual ~CFilterThreadedPipeConnector();

	virtual void open();
	virtual CFile::TFileSize size();
	virtual void read(Ref(CByteBuffer) inputbuf);
	virtual void write(Ref(CByteBuffer) outputbuf);
	virtual void close();

protected:
	CByteLinkedBuffer m_buffer;
};

class WINSOURCES_API CFilterVector : public CDataVectorT<CFilter, CCppObjectLessFunctor<CFilter>, CCppObjectNullFunctor<CFilter> >
{
	typedef CDataVectorT<CFilter, CCppObjectLessFunctor<CFilter>, CCppObjectNullFunctor<CFilter> > super;
public:
	CFilterVector();
	~CFilterVector();
};

class WINSOURCES_API CFilterPipe: public CCppObject
{
public:
	CFilterPipe(Ptr(CFilter) _input, Ptr(CFilter) _output);
	CFilterPipe(Ptr(CFilter) _input, Ptr(CFilter) _medium, Ptr(CFilter) _output);
	virtual ~CFilterPipe(void);

	virtual void do_pipe();

protected:
	CFilterVector m_Pipe;

	CFilterPipe(void);
};

class WINSOURCES_API CFilterThreadedPipe : public CFilterPipe
{
public:
	CFilterThreadedPipe(Ptr(CFilter) _input, Ptr(CFilter) _output);
	CFilterThreadedPipe(Ptr(CFilter) _input, Ptr(CFilter) _medium, Ptr(CFilter) _output);
	virtual ~CFilterThreadedPipe(void);

	virtual void do_pipe();

protected:
	CThreadPool m_Pool;

	CFilterThreadedPipe(void);
};

