/******************************************************************************
    
	This file is part of XServiceCtrlApp, which is part of UserLib.

    Copyright (C) 1995-2014  Oliver Kreis (okdev10@arcor.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by 
	the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/
#include "stdafx.h"

void okEnumAllServices()
{
	Ptr(CServiceManager) pManager = OK_NEW_OPERATOR CServiceManager;

	pManager->StartUp();
	pManager->EnumAll();
	pManager->Print();
	pManager->CleanUp();
	pManager->release();
}

void okStartServices(ConstRef(CDataVectorT<mbchar>) services)
{
	Ptr(CServiceManager) pManager = OK_NEW_OPERATOR CServiceManager;
	CDataVectorT<mbchar>::Iterator it = services.Begin();

	pManager->StartUp();
	while (it)
	{
		pManager->Start(*it);
		++it;
	}
	pManager->CleanUp();
	pManager->release();
}

void okStopServices(ConstRef(CDataVectorT<mbchar>) services)
{
	Ptr(CServiceManager) pManager = OK_NEW_OPERATOR CServiceManager;
	CDataVectorT<mbchar>::Iterator it = services.Begin();

	pManager->StartUp();
	while (it)
	{
		pManager->Stop(*it);
		++it;
	}
	pManager->CleanUp();
	pManager->release();
}

void okEnableServices(ConstRef(CDataVectorT<mbchar>) services)
{
	Ptr(CServiceManager) pManager = OK_NEW_OPERATOR CServiceManager;
	CDataVectorT<mbchar>::Iterator it = services.Begin();

	pManager->StartUp();
	while (it)
	{
		pManager->Enable(*it);
		++it;
	}
	pManager->CleanUp();
	pManager->release();
}

void okDisableServices(ConstRef(CDataVectorT<mbchar>) services)
{
	Ptr(CServiceManager) pManager = OK_NEW_OPERATOR CServiceManager;
	CDataVectorT<mbchar>::Iterator it = services.Begin();

	pManager->StartUp();
	while (it)
	{
		pManager->Disable(*it);
		++it;
	}
	pManager->CleanUp();
	pManager->release();
}

void okDeleteServices(ConstRef(CDataVectorT<mbchar>) services)
{
	Ptr(CServiceManager) pManager = OK_NEW_OPERATOR CServiceManager;
	CDataVectorT<mbchar>::Iterator it = services.Begin();

	pManager->StartUp();
	while (it)
	{
		pManager->Delete(*it);
		++it;
	}
	pManager->CleanUp();
	pManager->release();
}


