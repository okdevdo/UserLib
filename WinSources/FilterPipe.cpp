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
#include "WS_PCH.H"
#include "FilterPipe.h"
#include "ScopedLock.h"

#ifdef OK_SYS_UNIX
#include <sys/select.h>
#endif

CFilterPipeConnector::CFilterPipeConnector() {}
CFilterPipeConnector::~CFilterPipeConnector() {}

void CFilterPipeConnector::open()
{
}

CFile::TFileSize CFilterPipeConnector::size()
{
	return 0;
}

void CFilterPipeConnector::read(Ref(CByteBuffer) inputbuf)
{
	if (m_buffer.GetBufferItemCount() > 0)
	{
		dword sz = 0;
		BPointer pB = m_buffer.GetBufferItem(0, &sz);

		inputbuf.set_Buffer(__FILE__LINE__ pB, sz);
		m_buffer.RemoveBufferItem(0);
	}
	else
		inputbuf.set_BufferSize(__FILE__LINE__ 0);
}

void CFilterPipeConnector::write(Ref(CByteBuffer) outputbuf)
{
	s_memmove_s(m_buffer.AddBufferItem(outputbuf.get_BufferSize()), outputbuf.get_BufferSize(), outputbuf.get_Buffer(), outputbuf.get_BufferSize());
}

void CFilterPipeConnector::close()
{
}

CFilterThreadedPipeConnector::CFilterThreadedPipeConnector() {}
CFilterThreadedPipeConnector::~CFilterThreadedPipeConnector() {}

void CFilterThreadedPipeConnector::open()
{
}

CFile::TFileSize CFilterThreadedPipeConnector::size()
{
	return 0;
}

void CFilterThreadedPipeConnector::read(Ref(CByteBuffer) inputbuf)
{
	CScopedLock lock;
	dword bufCnt = m_buffer.GetBufferItemCount();

	if (bufCnt == 0)
	{
		lock.unlock();
#ifdef OK_SYS_WINDOWS
		Sleep(200);
#endif
#ifdef OK_SYS_UNIX
		{
			struct timeval tv;

			tv.tv_sec = 0;
			tv.tv_usec = 200000;
			select(0, NULL, NULL, NULL, &tv);
		}
#endif
		lock.lock();
	}
	if (m_buffer.GetBufferItemCount() > 0)
	{
		dword sz = 0;
		BPointer pB = m_buffer.GetBufferItem(0, &sz);

		inputbuf.set_Buffer(__FILE__LINE__ pB, sz);
		m_buffer.RemoveBufferItem(0);
	}
	else
		inputbuf.set_BufferSize(__FILE__LINE__ 0);
}

void CFilterThreadedPipeConnector::write(Ref(CByteBuffer) outputbuf)
{
	CScopedLock lock;

	s_memmove_s(m_buffer.AddBufferItem(outputbuf.get_BufferSize()), outputbuf.get_BufferSize(), outputbuf.get_Buffer(), outputbuf.get_BufferSize());
}

void CFilterThreadedPipeConnector::close()
{
}

static void __stdcall CFilterVectorDeleteFunc(ConstPointer data, Pointer context)
{
	Ptr(CFilter) pFilter = CastAnyPtr(CFilter, CastMutable(Pointer, data));

	pFilter->release();
}

CFilterVector::CFilterVector() : CDataVectorT<CFilter>(__FILE__LINE__ 32, 32) {}
CFilterVector::~CFilterVector() {}

CFilterPipe::CFilterPipe(Ptr(CFilter) _input, Ptr(CFilter) _output)
{
	m_Pipe.Append(_input);
	m_Pipe.Append(_output);
}

CFilterPipe::CFilterPipe(Ptr(CFilter) _input, Ptr(CFilter) _medium, Ptr(CFilter) _output)
{
	m_Pipe.Append(_input);
	m_Pipe.Append(_medium);
	m_Pipe.Append(_output);
}

CFilterPipe::~CFilterPipe(void) {}

void CFilterPipe::do_pipe()
{
	CFilterVector::Iterator it = m_Pipe.Begin();

	while (it)
	{
		(*it)->open();
		++it;
	}
	it = m_Pipe.Begin();
	while (it)
	{
		(*it)->do_filter();
		++it;
	}
	it = m_Pipe.Begin();
	while (it)
	{
		(*it)->close();
		++it;
	}
}

CFilterThreadedPipe::CFilterThreadedPipe(Ptr(CFilter) _input, Ptr(CFilter) _output):
	CFilterPipe(_input, _output),
	m_Pool(__FILE__LINE__ 5, 5, INT_MAX, CThreadPool::QuickResponse)
{
}

CFilterThreadedPipe::CFilterThreadedPipe(Ptr(CFilter) _input, Ptr(CFilter) _medium, Ptr(CFilter) _output):
	CFilterPipe(_input, _medium, _output),
	m_Pool(__FILE__LINE__ 5, 5, INT_MAX, CThreadPool::QuickResponse)
{
}

CFilterThreadedPipe::~CFilterThreadedPipe(void) {}

void CFilterThreadedPipe::do_pipe()
{
	CFilterVector::Iterator it = m_Pipe.Begin();

	while (it)
	{
		(*it)->open();
		++it;
	}
	CScopedLock lock;

	it = m_Pipe.Begin();
	while (it)
	{
		CThreadCallback<CFilter>* pThreadCallback = OK_NEW_OPERATOR CThreadCallback<CFilter>(*it, &CFilter::do_filter);

		m_Pool.AddTask(pThreadCallback);
		++it;
	}
	lock.unlock();
	m_Pool.WaitForComplete();
	m_Pool.StopAll();
	m_Pool.JoinAll();

	it = m_Pipe.Begin();
	while (it)
	{
		(*it)->close();
		++it;
	}
}

