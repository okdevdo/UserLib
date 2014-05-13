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

#include "WinSources.h"
#include "FilePath.h"
#include "DataBase.h"
#include "URL.h"
#include "HTTPClient.h"
#include "HtmlPage.h"

class WINSOURCES_API CHttpCache: public CCppObject
{
public:
	CHttpCache(void);
	CHttpCache(ConstRef(CFilePath) rootDir);
	virtual ~CHttpCache(void);

	void Open(ConstRef(CFilePath) rootDir);
	void Close();
	void Import(ConstRef(CFilePath) path);

protected:
	void CreateRootDir();
	void CreateMainCache();
	void CreateDocumentCache();
	void CreateImageCache();
	void CreateCookieCache();
	void CheckClient(Ref(CHttpClient) client);
	void UpdateDispatchClient(Ref(CHttpClient) client);
	void Update1(Ref(CHttpClient) client);
	void Update2(Ref(CHttpClient) client);
	void Update3(Ref(CHttpClient) client, ConstRef(CFilePath) filePath);
	void GetDocumentPath(Ref(CHttpClient) client, Ref(CHtmlPage) page, Ref(CFilePath) filePath);
	void UpdateDocumentCache(Ref(CHttpClient) client);
	void UpdateDocumentCache(Ref(CHttpClient) client, double id);
	void UpdateImageCache(Ref(CHtmlPage) page, double id);
	void UpdateCookieCache(Ref(CHttpClient) client, double id);
	void LoadUrl(ConstRef(CUrl) path);

	CFilePath m_RootDir;
	CFilePath m_MainCachePath;
	CDBase m_MainCache;
	CFilePath m_DocumentCachePath;
	CDBase m_DocumentCache;
	CFilePath m_ImageCachePath;
	CDBase m_ImageCache;
	CFilePath m_CookieCachePath;
	CDBase m_CookieCache;
};

