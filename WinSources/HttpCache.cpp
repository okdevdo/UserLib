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
#include "HttpCache.h"
#include "HttpDate.h"
#include "File.h"
#include "DirectoryIterator.h"
#include "DataVector.h"

CHttpCache::CHttpCache(void):
	m_RootDir(),
	m_MainCachePath(),
	m_MainCache(),
	m_DocumentCachePath(),
	m_DocumentCache(),
	m_ImageCachePath(),
	m_ImageCache(),
	m_CookieCachePath(),
	m_CookieCache()
{
}

CHttpCache::CHttpCache(ConstRef(CFilePath) rootDir):
	m_RootDir(),
	m_MainCachePath(),
	m_MainCache(),
	m_DocumentCachePath(),
	m_DocumentCache(),
	m_ImageCachePath(),
	m_ImageCache(),
	m_CookieCachePath(),
	m_CookieCache()
{
	Open(rootDir);
}

CHttpCache::~CHttpCache(void)
{
	Close();
}

void CHttpCache::Open(ConstRef(CFilePath) rootDir)
{
	CStringBuffer sBuffer;
	CStringBuffer sBuffer1;

	m_RootDir = rootDir;
	CreateRootDir();

	sBuffer = rootDir.get_Directory(-1);
	if ( sBuffer.IsEmpty() )
		return;

	m_MainCachePath = rootDir;
	m_MainCachePath.set_Basename(sBuffer);
	m_MainCachePath.set_Extension(_T(".dbf"));
	CreateMainCache();

	sBuffer1 = sBuffer;
	sBuffer1.AppendString(_T("_Documents"));

	m_DocumentCachePath = rootDir;
	m_DocumentCachePath.set_Basename(sBuffer1);
	m_DocumentCachePath.set_Extension(_T(".dbf"));
	CreateDocumentCache();

	sBuffer1 = sBuffer;
	sBuffer1.AppendString(_T("_Images"));

	m_ImageCachePath = rootDir;
	m_ImageCachePath.set_Basename(sBuffer1);
	m_ImageCachePath.set_Extension(_T(".dbf"));
	CreateImageCache();

	sBuffer1 = sBuffer;
	sBuffer1.AppendString(_T("_Cookies"));

	m_CookieCachePath = rootDir;
	m_CookieCachePath.set_Basename(sBuffer1);
	m_CookieCachePath.set_Extension(_T(".dbf"));
	CreateCookieCache();
}

void CHttpCache::Close()
{
	m_MainCache.Close();
	m_DocumentCache.Close();
	m_ImageCache.Close();
	m_CookieCache.Close();
}

void CHttpCache::Import(ConstRef(CFilePath) path)
{
	CDiskFile impfile;
	CByteBuffer impfcontent;

	try
	{
		impfile.Open(path, true, false);
		impfcontent.set_BufferSize(__FILE__LINE__ Cast(dword, impfile.GetSize()));
		impfile.Read(impfcontent);
		impfile.Close();
	}
	catch ( CFileException* )
	{
		impfile.Close();
		return;
	}

	CStringBuffer sImpfcontent;

	sImpfcontent.convertFromUTF8(impfcontent);

	CStringConstIterator it(sImpfcontent.GetString());
	dword cnt = 1;

	it.Find(_T("\r\n"));
	while ( !(it.IsEnd()) )
	{
		++cnt;
		++it;
		it.Find(_T("\r\n"));
	}

	CArray sImpfcSplitArray = Cast(CArray, TFalloc(cnt * szPointer));

	sImpfcontent.Split(_T("\r\n"), sImpfcSplitArray, cnt, &cnt);
	for ( dword ix = 0; ix < cnt; ++ix )
	{
		if ( (*(sImpfcSplitArray[ix]) != 0) && (*(sImpfcSplitArray[ix]) != _T('#')) )
		{
			CUrl path(sImpfcSplitArray[ix]);

			LoadUrl(path);
		}
	}
	TFfree(sImpfcSplitArray);
}

void CHttpCache::CreateRootDir()
{
	try
	{
		CDirectoryIterator::MakeDirectory(m_RootDir);
		CDirectoryIterator::SetCurrentDirectory(m_RootDir);
	}
	catch ( CDirectoryIteratorException* )
	{
	}
}

void CHttpCache::CreateMainCache()
{
	bool bOpen = false;

	try
	{
		m_MainCache.Open(m_MainCachePath);
		bOpen = true;
	}
	catch ( CDBaseException* )
	{
		m_MainCache.Close();
	}
	if ( !bOpen )
	{
		try
		{
			m_MainCache.Init(3, 70);
			m_MainCache.InitField(0, _T("Identity"), 'I', 10, 0);
			m_MainCache.InitField(1, _T("Server"), 'C', 50, 0);
			m_MainCache.InitField(2, _T("DefaultURL"), 'M', 10, 0);
			m_MainCache.InitIndex(2);
			m_MainCache.InitIndexFields(0, 1);
			m_MainCache.InitIndexField(0, 0, 0, 10); // Identity
			m_MainCache.InitIndexFields(1, 1);
			m_MainCache.InitIndexField(1, 0, 1, 50); // Server

			m_MainCache.Create(m_MainCachePath);
		}
		catch ( CDBaseException* )
		{
			m_MainCache.Close();
		}
	}
}

void CHttpCache::CreateDocumentCache()
{
	bool bOpen = false;

	try
	{
		m_DocumentCache.Open(m_DocumentCachePath);
		bOpen = true;
	}
	catch (CDBaseException*)
	{
		m_DocumentCache.Close();
	}
	if (!bOpen)
	{
		try
		{
			m_DocumentCache.Init(10, 250);
			m_DocumentCache.InitField(0, _T("Identity"), 'I', 10, 0);
			m_DocumentCache.InitField(1, _T("ServerId"), 'N', 10, 0);
			m_DocumentCache.InitField(2, _T("ETag"), 'C', 50, 0);
			m_DocumentCache.InitField(3, _T("MimeType"), 'C', 50, 0);
			m_DocumentCache.InitField(4, _T("Charset"), 'C', 20, 0);
			m_DocumentCache.InitField(5, _T("Date"), 'C', 30, 0);
			m_DocumentCache.InitField(6, _T("Expires"), 'C', 30, 0);
			m_DocumentCache.InitField(7, _T("Modified"), 'C', 30, 0);
			m_DocumentCache.InitField(8, _T("URL"), 'M', 10, 0);
			m_DocumentCache.InitField(9, _T("Path"), 'M', 10, 0);
			m_DocumentCache.InitIndex(2);
			m_DocumentCache.InitIndexFields(0, 1);
			m_DocumentCache.InitIndexField(0, 0, 0, 10); // Identity
			m_DocumentCache.InitIndexFields(1, 1);
			m_DocumentCache.InitIndexField(1, 0, 1, 10); // ServerId

			m_DocumentCache.Create(m_DocumentCachePath);
		}
		catch (CDBaseException*)
		{
			m_DocumentCache.Close();
		}
	}
}

void CHttpCache::CreateImageCache()
{
	bool bOpen = false;

	try
	{
		m_ImageCache.Open(m_ImageCachePath);
		bOpen = true;
	}
	catch (CDBaseException*)
	{
		m_ImageCache.Close();
	}
	if (!bOpen)
	{
		try
		{
			m_ImageCache.Init(4, 40);
			m_ImageCache.InitField(0, _T("Identity"), 'I', 10, 0);
			m_ImageCache.InitField(1, _T("DocuId"), 'N', 10, 0);
			m_ImageCache.InitField(2, _T("URL"), 'M', 10, 0);
			m_ImageCache.InitField(3, _T("Path"), 'M', 10, 0);
			m_ImageCache.InitIndex(2);
			m_ImageCache.InitIndexFields(0, 1);
			m_ImageCache.InitIndexField(0, 0, 0, 10); // Identity
			m_ImageCache.InitIndexFields(1, 1);
			m_ImageCache.InitIndexField(1, 0, 1, 10); // DocuId

			m_ImageCache.Create(m_ImageCachePath);
		}
		catch (CDBaseException*)
		{
			m_ImageCache.Close();
		}
	}
}

void CHttpCache::CreateCookieCache()
{
	bool bOpen = false;

	try
	{
		m_CookieCache.Open(m_CookieCachePath);
		bOpen = true;
	}
	catch ( CDBaseException* )
	{
		m_CookieCache.Close();
	}
	if ( !bOpen )
	{
		try
		{
			m_CookieCache.Init(8, 142);
			m_CookieCache.InitField(0, _T("Identity"), 'I', 10, 0);
			m_CookieCache.InitField(1, _T("ServerId"), 'N', 10, 0);
			m_CookieCache.InitField(2, _T("Domain"), 'C', 30, 0);
			m_CookieCache.InitField(3, _T("Path"), 'C', 50, 0);
			m_CookieCache.InitField(4, _T("Expires"), 'C', 30, 0);
			m_CookieCache.InitField(5, _T("Secure"), 'L', 1, 0);
			m_CookieCache.InitField(6, _T("HttpOnly"), 'L', 1, 0);
			m_CookieCache.InitField(7, _T("Content"), 'M', 10, 0);
			m_CookieCache.InitIndex(2);
			m_CookieCache.InitIndexFields(0, 1);
			m_CookieCache.InitIndexField(0, 0, 0, 10); // Identity
			m_CookieCache.InitIndexFields(1, 1);
			m_CookieCache.InitIndexField(1, 0, 1, 10); // ServerId

			m_CookieCache.Create(m_CookieCachePath);
		}
		catch ( CDBaseException* )
		{
			m_CookieCache.Close();
		}
	}
}

void CHttpCache::CheckClient(Ref(CHttpClient) client)
{
	double id;
	CStringBuffer sBuf;
	CStringBuffer sBuf1;

	try
	{
		m_MainCache.ClearData();
		m_MainCache.SetTextField(1, client.get_ServerName());
		if ( !(m_MainCache.ReadIndex(1)) )
			return;
		if ( client.get_DefaultURL() )
		{
			sBuf = m_MainCache.GetTextField(2);

			CStringConstIterator it(sBuf.GetString());

			it.Find(_T("http:"), CStringConstIterator::cIgnoreCase);
			if ( it.IsBegin() )
			{
				CUrl url(sBuf.GetString());

				client.set_ServerBackupName(client.get_ServerName());
				client.set_ServerName(url.get_Server());
				client.set_ResourceBackupString(client.get_ResourceString());
				client.set_ResourceString(url.get_Resource());
			}
			else
			{
				client.set_ServerBackupName(client.get_ServerName());
				client.set_ResourceBackupString(client.get_ResourceString());
				client.set_ResourceString(sBuf);
			}
		}
		id = m_MainCache.GetNumericField(0);

		m_DocumentCache.ClearData();
		m_DocumentCache.SetNumericField(1, id);
		if ( m_DocumentCache.ReadIndex(1) )
		{
			do
			{
				if ( m_DocumentCache.GetNumericField(1) != id )
					break;
				sBuf = m_DocumentCache.GetTextField(8);
				if ( sBuf == client.get_ResourceString() )
				{
					sBuf = m_DocumentCache.GetTextField(2);
					if ( !(sBuf.IsEmpty()) )
						client.set_RequestData(CStringBuffer(__FILE__LINE__ _T("If-None-Match")), sBuf);
					break;
				}
			} while ( m_DocumentCache.NextIndex(1) );
		}

		m_CookieCache.ClearData();
		m_CookieCache.SetNumericField(1, id);
		if ( m_CookieCache.ReadIndex(1) )
		{
			do
			{
				if ( m_CookieCache.GetNumericField(1) != id )
					break;
				sBuf = m_CookieCache.GetTextField(7);
				if ( !(sBuf.IsEmpty()) )
				{
					CStringBuffer name(__FILE__LINE__ _T("Cookie"));

					sBuf1 = client.get_RequestData(name);
					if ( sBuf1.IsEmpty() )
						client.set_RequestData(name, sBuf);
					else
					{
						sBuf1.AppendString(_T(";"));
						sBuf1.AppendString(sBuf.GetString());
						client.remove_RequestData(name);
						client.set_RequestData(name, sBuf1);
					}
				}
			} while ( m_CookieCache.NextIndex(1) );
		}
	}
	catch ( CDBaseException* )
	{
	}
}

void CHttpCache::UpdateDocumentCache(Ref(CHttpClient) client)
{
	CStringBuffer sBuf;
	CHttpDate date;

	sBuf = client.get_ResponseData(CStringBuffer(__FILE__LINE__ _T("ETag")));
	if ( !(sBuf.IsEmpty()) )
		m_DocumentCache.SetTextField(2, sBuf);
	sBuf = client.get_ResponseData(CStringBuffer(__FILE__LINE__ _T("Content-Type")));
	if ( !(sBuf.IsEmpty()) )
	{
		CPointer splitArray[3];
		dword splitCnt;

		sBuf.Split(_T(";"), splitArray, 3, &splitCnt);
		if ( splitCnt == 2 )
		{
			m_DocumentCache.SetTextField(3, CStringBuffer(__FILE__LINE__ splitArray[0]));
			sBuf.SetString(__FILE__LINE__ splitArray[1]);
			sBuf.Split(_T("="), splitArray, 3, &splitCnt);
			if ( splitCnt == 2 )
				m_DocumentCache.SetTextField(4, CStringBuffer(__FILE__LINE__ splitArray[1]));
		}
	}
	sBuf.SetString(__FILE__LINE__ client.get_ResponseData(CStringBuffer(__FILE__LINE__ _T("Date"))));
	if ( !(sBuf.IsEmpty()) )
	{
		date.SetDate(sBuf.GetString());
		m_DocumentCache.SetTextField(5, date.GetDate());
	}
	sBuf.SetString(__FILE__LINE__ client.get_ResponseData(CStringBuffer(__FILE__LINE__ _T("Expires"))));
	if ( !(sBuf.IsEmpty()) )
	{
		if ( (sBuf == CStringLiteral(_T("0"))) || (sBuf == CStringLiteral(_T("-1"))) )
			date.Now();
		else
			date.SetDate(sBuf.GetString());
		m_DocumentCache.SetTextField(6, date.GetDate());
	}
	sBuf.SetString(__FILE__LINE__ client.get_ResponseData(CStringBuffer(__FILE__LINE__ _T("Last-Modified"))));
	if ( !(sBuf.IsEmpty()) )
	{
		date.SetDate(sBuf.GetString());
		m_DocumentCache.SetTextField(7, date.GetDate());
	}
}

void CHttpCache::UpdateDocumentCache(Ref(CHttpClient) client, double id)
{
	CStringBuffer sBuf;
	bool bNewDocument = true;

	m_DocumentCache.ClearData();
	m_DocumentCache.SetNumericField(1, id);
	if ( m_DocumentCache.ReadIndex(1) )
	{
		do
		{
			if ( m_DocumentCache.GetNumericField(1) != id )
				break;
			sBuf = m_DocumentCache.GetTextField(8);
			if ( sBuf == client.get_ResourceString() )
			{
				UpdateDocumentCache(client);
				m_DocumentCache.Write(m_DocumentCache.GetIndexRecno(1));
				bNewDocument = false;
				break;
			}
		} while ( m_DocumentCache.NextIndex(1) );
	}
	if ( bNewDocument )
	{
		m_DocumentCache.ClearData();
		m_DocumentCache.SetNumericField(1, id);
		UpdateDocumentCache(client);
		m_DocumentCache.SetTextField(8, client.get_ResourceString());
		m_DocumentCache.Append();
	}
}

void CHttpCache::UpdateImageCache(Ref(CHtmlPage) page, double id)
{
	CStringBuffer sBuf;
	CFilePath vPath;
	CFilePath vPath1;
	double vId1;

	m_ImageCache.ClearData();
	m_ImageCache.SetNumericField(1, id);
	if (m_ImageCache.ReadIndex(1))
	{
		do
		{
			if (m_ImageCache.GetNumericField(1) != id)
				break;
			sBuf = m_ImageCache.GetTextField(2); // url
			if (page.GetAssocPath(sBuf, vPath))
			{
				vId1 = m_ImageCache.GetNumericField(0);
				vPath1 = m_ImageCache.GetTextField(3); // path
				if (CDirectoryIterator::FileExists(vPath1))
					CDirectoryIterator::RemoveFile(vPath1);
				m_ImageCache.SetTextField(3, vPath.get_Path());
				m_ImageCache.Write(m_ImageCache.GetIndexRecno(1));

				m_ImageCache.ClearData();
				m_ImageCache.SetNumericField(1, id);
				if (m_ImageCache.ReadIndex(1))
				{
					do
					{
						if (m_ImageCache.GetNumericField(1) != id)
							break;
						if (m_ImageCache.GetNumericField(0) == vId1)
							break;
					} while (m_ImageCache.NextIndex(1));
				}
				else
					break;
			}
		} while (m_ImageCache.NextIndex(1));
	}

	CHtmlPage::ServerItems::Iterator itS = page.GetServerItems().Begin();

	while (itS)
	{
		CHtmlPage::ResourceItems::Iterator itR = (*itS)->_items.Begin();

		while (itR)
		{
			if (!((*itR)->_updated))
			{
				m_ImageCache.ClearData();
				m_ImageCache.SetNumericField(1, id);
				sBuf.SetString(__FILE__LINE__ _T("http://"));
				sBuf.AppendString((*itS)->_server);
				sBuf.AppendString((*itR)->_resource);
				m_ImageCache.SetTextField(2, sBuf);
				m_ImageCache.SetTextField(3, (*itR)->_filePath.get_Path());
				m_ImageCache.Append();
			}
			(*itR)->_updated = false;
			++itR;
		}
		++itS;
	}
}

void CHttpCache::UpdateCookieCache(Ref(CHttpClient) client, double id)
{
	CStringBuffer sBuf;
	CStringBuffer sBuf1;
	CStringBuffer sBuf2;
	CHttpDate date1; // Now
	CHttpDate date2;
	bool bContinue = true;

	while ( bContinue )
	{
		m_CookieCache.ClearData();
		m_CookieCache.SetNumericField(1, id);
		if ( m_CookieCache.ReadIndex(1) )
		{
			do
			{
				date2.SetDate(m_CookieCache.GetTextField(4));
				if ( date2 < date1 )
				{
					m_CookieCache.Delete(m_CookieCache.GetIndexRecno(1));
					bContinue = true;
					break;
				}
				bContinue = false;
			}
			while ( m_CookieCache.NextIndex(1) );
		}
		else
			bContinue = false;
	}

	CHttpClient::ResponseDataList::iterator rit(client.get_ResponseDataBegin());

	while ( *rit )
	{
		sBuf = (*rit)->item.Key;
		if ( sBuf == CStringLiteral(_T("Set-Cookie")) )
		{
			m_CookieCache.ClearData();
			m_CookieCache.SetNumericField(1, id);
			sBuf = (*rit)->item.Value;

			CStringConstIterator sit(sBuf.GetString());
			dword cnt = 1;

			sit.Find(_T(";"));
			while ( !(sit.IsEnd()) )
			{
				++cnt;
				++sit;
				sit.Find(_T(";"));
			}

			CArray splitArray1 = CastAny(CArray, TFalloc(cnt * szPointer));

			sBuf.Split(_T(";"), splitArray1, cnt, &cnt);
			if ( cnt > 0 )
				m_CookieCache.SetTextField(7, CStringBuffer(__FILE__LINE__ splitArray1[0]));
			for ( dword ix = 1; ix < cnt; ++ix )
			{
				sBuf1.SetString(__FILE__LINE__ splitArray1[ix]);
				sit = sBuf1.GetString();
				sit.Find(_T("="));
				if ( sit.IsEnd() )
				{
					sBuf1.Trim();
					sBuf1.ToLowerCase();
					if ( sBuf1 == CStringLiteral(_T("secure")) )
						m_CookieCache.SetLogicalField(5, 1);
					else if ( sBuf1 == CStringLiteral(_T("httponly")) )
						m_CookieCache.SetLogicalField(6, 1);
				}
				else
				{
					CPointer splitArray2[3];
					dword cnt2;

					sBuf1.Split(_T("="), splitArray2, 3, &cnt2);
					if ( cnt2 == 2 )
					{
						sBuf2.SetString(__FILE__LINE__ splitArray2[0]);

						sBuf2.Trim();
						sBuf2.ToLowerCase();
						if ( sBuf2 == CStringLiteral(_T("domain")) )
						{
							sBuf2.SetString(__FILE__LINE__ splitArray2[1]);
							sBuf2.Trim();
							m_CookieCache.SetTextField(2, sBuf2);
						}
						else if ( sBuf2 == CStringLiteral(_T("path")) )
						{
							sBuf2.SetString(__FILE__LINE__ splitArray2[1]);
							sBuf2.Trim();
							m_CookieCache.SetTextField(3, sBuf2);
						}
						else if ( sBuf2 == CStringLiteral(_T("expires")) )
						{
							sBuf2.SetString(__FILE__LINE__ splitArray2[1]);
							sBuf2.Trim();
							m_CookieCache.SetTextField(4, sBuf2);
						}
					}
				}
			}
			TFfree(splitArray1);
			m_CookieCache.Append();
		}
		++rit;
	}
}

void CHttpCache::Update1(Ref(CHttpClient) client)
{
	double id;
	CStringBuffer sBuf;

	m_MainCache.ClearData();
	m_MainCache.SetTextField(1, client.get_ServerName());
	if ( !(m_MainCache.ReadIndex(1)) )
		m_MainCache.Append();
	id = m_MainCache.GetNumericField(0);

	UpdateDocumentCache(client, id);
	UpdateCookieCache(client, id);
}

void CHttpCache::Update2(Ref(CHttpClient) client)
{
	double id;
	CStringBuffer sBuf;
	bool bServerNameDiffers = false;

	m_MainCache.ClearData();
	m_MainCache.SetTextField(1, client.get_ServerBackupName());
	if ( !(m_MainCache.ReadIndex(1)) )
	{
		if ( client.get_ServerBackupName() != client.get_ServerName() )
		{
			sBuf = client.get_ServerName();
			bServerNameDiffers = true;
		}
		sBuf.AppendString(client.get_ResourceString());
		m_MainCache.SetTextField(2, sBuf);
		m_MainCache.Append();
	}
	if ( bServerNameDiffers )
	{
		m_MainCache.ClearData();
		m_MainCache.SetTextField(1, client.get_ServerName());
		if ( !(m_MainCache.ReadIndex(1)) )
		{
			m_MainCache.SetTextField(2, client.get_ResourceString());
			m_MainCache.Append();
		}
	}
	id = m_MainCache.GetNumericField(0);

	UpdateDocumentCache(client, id);
	UpdateCookieCache(client, id);
}

void CHttpCache::Update3(Ref(CHttpClient) client, ConstRef(CFilePath) filePath)
{
	double id;
	CStringBuffer sBuf;
	bool bNewDocument = true;

	try
	{
		m_MainCache.ClearData();
		m_MainCache.SetTextField(1, client.get_ServerName());
		if ( !(m_MainCache.ReadIndex(1)) )
			m_MainCache.Append();
		id = m_MainCache.GetNumericField(0);

		m_DocumentCache.ClearData();
		m_DocumentCache.SetNumericField(1, id);
		if ( m_DocumentCache.ReadIndex(1) )
		{
			do
			{
				if ( m_DocumentCache.GetNumericField(1) != id )
					break;
				sBuf = m_DocumentCache.GetTextField(8);
				if ( sBuf == client.get_ResourceString() )
				{
					m_DocumentCache.SetTextField(9, filePath.get_Path());
					m_DocumentCache.Write(m_DocumentCache.GetIndexRecno(1));
					bNewDocument = false;
					break;
				}
			} while ( m_DocumentCache.NextIndex(1) );
		}
		if ( bNewDocument )
		{
			UpdateDocumentCache(client);
			m_DocumentCache.SetTextField(8, client.get_ResourceString());
			m_DocumentCache.SetTextField(9, filePath.get_Path());
			m_DocumentCache.Append();
		}
	}
	catch ( CDBaseException* )
	{
	}
}

void CHttpCache::GetDocumentPath(Ref(CHttpClient) client, Ref(CHtmlPage) page, Ref(CFilePath) filePath)
{
	double id;
	double vId1;
	CStringBuffer sBuf;

	try
	{
		filePath.set_Path(__FILE__LINE__ _T(""));
		m_MainCache.ClearData();
		m_MainCache.SetTextField(1, client.get_ServerName());
		if ( !(m_MainCache.ReadIndex(1)) )
			m_MainCache.Append();
		id = m_MainCache.GetNumericField(0);

		m_DocumentCache.ClearData();
		m_DocumentCache.SetNumericField(1, id);
		if ( m_DocumentCache.ReadIndex(1) )
		{
			do
			{
				if ( m_DocumentCache.GetNumericField(1) != id )
					break;
				sBuf = m_DocumentCache.GetTextField(8);
				if ( sBuf == client.get_ResourceString() )
				{
					vId1 = m_DocumentCache.GetNumericField(0);
					sBuf = m_DocumentCache.GetTextField(9);
					filePath = sBuf;
					UpdateImageCache(page, vId1);
					break;
				}
			} while ( m_DocumentCache.NextIndex(1) );
		}
	}
	catch ( CDBaseException* )
	{
	}
}

void CHttpCache::UpdateDispatchClient(Ref(CHttpClient) client)
{
	try
	{
		if ( client.get_DefaultURL() )
		{
			switch ( client.get_BackupReason() )
			{
			case 0:
				Update1(client);
				break;
			case 301:
			case 302:
				Update2(client);
				break;
			default:
				break;
			}
		}
		else
		{
			switch ( client.get_BackupReason() )
			{
			case 0:
				Update1(client);
				break;
			case 301:
			case 302:
				Update2(client);
				break;
			default:
				break;
			}
		}
	}
	catch ( CDBaseException* )
	{
	}
}

void CHttpCache::LoadUrl(ConstRef(CUrl) path)
{
	CHttpClient client(path);
	CStringBuffer sBuf;
	CStringConstIterator sIt;
	CUrl uHelper;
	int resultCode;
	bool bContinue = true;
	bool bOk = false;

	CheckClient(client);
	while ( bContinue )
	{
		client.ClearResponse();
		if ( !(client.Load()) )
			return;
		client.get_ResponseTypeNum().ScanString(_T("%d"), &resultCode);
		switch ( resultCode )
		{
		case 100:
		case 101:
		case 204:
		case 304:
			bContinue = false;
			break;
		case 200: // OK
			UpdateDispatchClient(client);
			bContinue = false;
			bOk = true;
			break;
		case 301: // Moved Permanently
			if ( client.get_ResourceBackupString().IsEmpty() )
				client.set_ResourceBackupString(client.get_ResourceString());
			if ( client.get_ServerBackupName().IsEmpty() )
				client.set_ServerBackupName(client.get_ServerName());
			client.set_BackupReason(301);
			sBuf = client.get_ResponseData(CStringBuffer(__FILE__LINE__ _T("Location")));
			if ( sBuf.IsEmpty() )
			{
				bContinue = false;
				break;
			}
			sIt = sBuf.GetString();
			sIt.Find(_T("http:"), CStringConstIterator::cIgnoreCase);
			if ( sIt.IsBegin() )
			{
				uHelper.set_Url(sBuf);
				sBuf = uHelper.get_Server();
				client.set_ServerName(sBuf);
				sBuf = uHelper.get_Resource();
			}
			client.set_ResourceString(sBuf);
			break;
		case 302: // Found
			if ( client.get_ResourceBackupString().IsEmpty() )
				client.set_ResourceBackupString(client.get_ResourceString());
			if ( client.get_ServerBackupName().IsEmpty() )
				client.set_ServerBackupName(client.get_ServerName());
			client.set_BackupReason(302);
			sBuf = client.get_ResponseData(CStringBuffer(__FILE__LINE__ _T("Location")));
			if ( sBuf.IsEmpty() )
			{
				bContinue = false;
				break;
			}
			sIt = sBuf.GetString();
			sIt.Find(_T("http:"), CStringConstIterator::cIgnoreCase);
			if ( sIt.IsBegin() )
			{
				uHelper.set_Url(sBuf);
				sBuf = uHelper.get_Server();
				client.set_ServerName(sBuf);
				sBuf = uHelper.get_Resource();
			}
			client.set_ResourceString(sBuf);
			break;
		default:
			bContinue = false;
			break;
		}
	}	

	if ( bOk )
	{
		CHtmlPage page(m_RootDir, client.get_ServerName(), client.get_ResponseContent());
		CByteBuffer buffer;
		CDiskFile file;
		CDirectoryIterator dirIt;
		CFilePath path(m_RootDir);
		CFilePath path2;
		dword num = 0;
		dword num1;
		bool bUpdate = false;

		page.ReplaceImages();
		page.GetDocumentBuffer().GetBuffer(buffer);
		GetDocumentPath(client, page, path2);
		if ( path2.get_Path().IsEmpty() )
		{
			try
			{
				path.set_Filename(_T("index*.htm"));
				dirIt.Open(path);
				while ( dirIt )
				{
					path2 = dirIt.get_Name();
					sBuf = path2.get_Basename();
					if ( sBuf.ScanString(_T("index%u"), &num1) > 0 )
					{
						if ( num1 > num )
							num = num1;
					}
					++dirIt;
				}
				++num;
				sBuf.FormatString(__FILE__LINE__ _T("index%06u.htm"), num);
				path.set_Filename(sBuf);
				path2 = path;
			}
			catch ( CDirectoryIteratorException* )
			{
				dirIt.Close();
			}
			bUpdate = true;
		}
		try
		{
			file.Create(path2);
			file.Write(buffer);
			file.Close();
			if ( bUpdate )
				Update3(client, path2);
		}
		catch ( CFileException* )
		{
			file.Close();
		}
	}
}

