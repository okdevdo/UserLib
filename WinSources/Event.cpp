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
#include "Event.h"

IMPL_WINEXCEPTION(CEventException, CWinException)

CEvent::CEvent(void)
{
	m_Event = CreateEvent( 
        NULL,              // default security attributes
        TRUE,              // manual reset
		FALSE,             // initial state
        NULL);             // unnamed event
	if ( !m_Event )
		throw OK_NEW_OPERATOR CEventException(__FILE__LINE__ _T("in %s CEventException"), 
		    _T("CEvent::CEvent"), CWinException::WinExtError);
}

CEvent::~CEvent(void)
{
	if ( m_Event )
		CloseHandle(m_Event);
}

void CEvent::set()
{
	if ( !SetEvent(m_Event) )
		throw OK_NEW_OPERATOR CEventException(__FILE__LINE__ _T("in %s CEventException"), 
		    _T("CEvent::set"), CWinException::WinExtError);
}

void CEvent::reset()
{
	if ( !ResetEvent(m_Event) )
		throw OK_NEW_OPERATOR CEventException(__FILE__LINE__ _T("in %s CEventException"), 
		    _T("CEvent::reset"), CWinException::WinExtError);
}

bool CEvent::wait(DWORD millisec)
{
    switch ( WaitForSingleObject( 
            m_Event,    // handle to event
            millisec) ) 
    {
    // event is signalled
    case WAIT_OBJECT_0: 
        return true; 
	// timeout
	case WAIT_TIMEOUT:
		return false;
	default:
		throw OK_NEW_OPERATOR CEventException(__FILE__LINE__ _T("in %s CEventException"), 
			_T("CEvent::wait"), CWinException::WinExtError);
		break;
    }
}
