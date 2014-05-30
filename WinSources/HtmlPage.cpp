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
#include "HtmlPage.h"
#include "HTTPClient.h"
#include "File.h"
#include "DirectoryIterator.h"
#include "Url.h"

CHtmlPage::ReplacePointer::ReplacePointer(ConstRef(CByteLinkedBuffer::Iterator) begin, ConstRef(CByteLinkedBuffer::Iterator) end, ConstRef(CFilePath) path) :
	_begin(begin),
	_end(end),
	_filePath(path)
{
}

CHtmlPage::ReplacePointer::~ReplacePointer()
{
}

void __stdcall ReplacePointers_DeleteFunc( ConstPointer data, Pointer context )
{
	CHtmlPage::ReplacePointer* p = CastAnyPtr(CHtmlPage::ReplacePointer, CastMutable(Pointer, data));

	p->release();
}

sword __stdcall ReplacePointers_SearchAndSortFunc( ConstPointer pa, ConstPointer pb )
{
	CHtmlPage::ReplacePointer* ppa = CastAnyPtr(CHtmlPage::ReplacePointer, CastMutable(Pointer, pa));
	CHtmlPage::ReplacePointer* ppb = CastAnyPtr(CHtmlPage::ReplacePointer, CastMutable(Pointer, pb));

	if ( ppa->_begin < ppb->_begin )
		return -1;
	if ( ppa->_begin == ppb->_begin )
		return 0;
	return 1;
}

CHtmlPage::ContentPointer::ContentPointer(ConstRef(CByteLinkedBuffer::Iterator) begin, ConstRef(CByteLinkedBuffer::Iterator) end) :
	_begin(begin),
	_end(end)
{
}

CHtmlPage::ContentPointer::~ContentPointer()
{
}

void __stdcall ContentPointers_DeleteFunc(ConstPointer data, Pointer context)
{
	CHtmlPage::ContentPointer* p = CastAnyPtr(CHtmlPage::ContentPointer, CastMutable(Pointer, data));

	p->release();
}

CHtmlPage::ResourceItem::ResourceItem(ConstRef(CStringBuffer) resource) :
	_resource(resource),
	_filePath(),
	_pointers(__FILE__LINE__ 16, 16),
	_updated(false)
{
}

CHtmlPage::ResourceItem::~ResourceItem()
{
}

void __stdcall ResourceItems_DeleteFunc( ConstPointer data, Pointer context )
{
	CHtmlPage::ResourceItem* p = CastAnyPtr(CHtmlPage::ResourceItem, CastMutable(Pointer, data));

	p->release();
}

sword __stdcall ResourceItems_SearchAndSortFunc( ConstPointer pa, ConstPointer pb )
{
	CHtmlPage::ResourceItem* ppa = CastAnyPtr(CHtmlPage::ResourceItem, CastMutable(Pointer, pa));
	CHtmlPage::ResourceItem* ppb = CastAnyPtr(CHtmlPage::ResourceItem, CastMutable(Pointer, pb));

	if ( ppa->_resource < ppb->_resource )
		return -1;
	if ( ppa->_resource == ppb->_resource )
		return 0;
	return 1;
}

CHtmlPage::ServerItem::ServerItem(ConstRef(CStringBuffer) server) :
	_server(server),
	_items(__FILE__LINE__ 16, 16)
{
}

CHtmlPage::ServerItem::~ServerItem()
{
}

sword __stdcall ServerItems_SearchAndSortFunc( ConstPointer pa, ConstPointer pb )
{
	CHtmlPage::ServerItem* ppa = CastAnyPtr(CHtmlPage::ServerItem, CastMutable(Pointer, pa));
	CHtmlPage::ServerItem* ppb = CastAnyPtr(CHtmlPage::ServerItem, CastMutable(Pointer, pb));

	if ( ppa->_server < ppb->_server )
		return -1;
	if ( ppa->_server == ppb->_server )
		return 0;
	return 1;
}

void __stdcall ServerItems_DeleteFunc( ConstPointer data, Pointer context )
{
	CHtmlPage::ServerItem* p = CastAnyPtr(CHtmlPage::ServerItem, CastMutable(Pointer, data));

	p->release();
}

CHtmlPage::CHtmlPage(void):
    _cachePath(),
    _serverPath(),
    _documentBuffer(),
	_serverItems(__FILE__LINE__ 16, 16)
{
}

CHtmlPage::CHtmlPage(ConstRef(CFilePath) fpath, ConstRef(CStringBuffer) spath, ConstRef(CByteLinkedBuffer) buffer):
    _cachePath(fpath),
    _serverPath(spath),
    _documentBuffer(buffer),
	_serverItems(__FILE__LINE__ 16, 16)
{
}

CHtmlPage::~CHtmlPage(void)
{
}

void CHtmlPage::ReplaceImages()
{
	BPointer str2Find[] = {
		CastAnyPtr(byte, CastMutablePtr(char, "<")),
		CastAnyPtr(byte, CastMutablePtr(char, "img")),
		CastAnyPtr(byte, CastMutablePtr(char, "src")),
		CastAnyPtr(byte, CastMutablePtr(char, "\"")),
		CastAnyPtr(byte, CastMutablePtr(char, "/")),
		CastAnyPtr(byte, CastMutablePtr(char, ">"))
	};
	dword str2FindLen[] = {
		1, 3, 3, 1, 1, 1
	};
	CByteLinkedBuffer::Iterator ccit(_documentBuffer.Begin());
	CByteLinkedBuffer::Iterator ccit1(ccit);
	CByteBuffer buffer;
	CStringBuffer sBuf;
	CStringBuffer sBuf1;
	CUrl urlHelper;
	dword i;
	dword state;

	if ( !ccit ) 
		return;
	ccit.FindAnyStr(str2Find, str2FindLen, 6);
	state = 0;
	while ( ccit )
	{
		for (i = 0; i < 6; ++i)
		{
			if (ccit.Compare(str2Find[i], str2FindLen[i]))
				break;
		}
		if (i < 6)
			++ccit;
		else
			break;
		// <img src="/common/icons/facebook.png" alt="Facebook" title="Join us on Facebook" width="20" height="20" />
		// 0 1  2   3 4     4     4            3     3        3       3                   3       3  3        3  3 4 5
		// 0 1  2   3                          4                                                                   5 6 0
		// <img src="blahblah">text</img>
		switch (i)
		{
		case 0:
			switch (state)
			{
			case 0:
				++state;
				break;
			}
			break;
		case 1:
			switch (state)
			{
			case 1:
				++state;
				break;
			}
			break;
		case 2:
			switch (state)
			{
			case 2:
				++state;
				break;
			}
			break;
		case 3:
			switch (state)
			{
			case 1:
				++state;
				break;
			case 2:
				state = 1;
				break;
			case 3:
				++state;
				ccit1 = ccit;
				break;
			case 4:
				--ccit;
				buffer.set_BufferSize(__FILE__LINE__ ccit - ccit1);
				_documentBuffer.GetSubBuffer(ccit1, buffer);
				sBuf.convertFromByteBuffer(buffer);
				if (!(urlHelper.set_Url(sBuf)))
				{
					sBuf.PrependString(_serverPath);
					sBuf.PrependString(_T("http://"));
					if (!(urlHelper.set_Url(sBuf)))
						break;
				}
				{
					ServerItem serverItem(urlHelper.get_Server());
					ServerItems::Iterator itS = _serverItems.FindSorted(&serverItem);
					ServerItem* pServerItem = NULL;

					if (_serverItems.MatchSorted(itS, &serverItem))
						pServerItem = (*itS);
					else
					{
						pServerItem = OK_NEW_OPERATOR ServerItem(urlHelper.get_Server());
						itS = _serverItems.InsertSorted(pServerItem);
						if (itS)
							pServerItem = (*itS);
						else
							pServerItem = NULL;
					}
					if ((pServerItem != NULL) && (!(urlHelper.get_Resource().IsEmpty())))
					{
						ResourceItem resourceItem(urlHelper.get_Resource());
						ResourceItems::Iterator itR = pServerItem->_items.FindSorted(&resourceItem);
						ResourceItem* pResourceItem = NULL;

						if (pServerItem->_items.MatchSorted(itR, &resourceItem))
							pResourceItem = *itR;
						else
						{
							pResourceItem = OK_NEW_OPERATOR ResourceItem(urlHelper.get_Resource());
							itR = pServerItem->_items.InsertSorted(pResourceItem);
							if (itR)
								pResourceItem = (*itR);
							else
								pResourceItem = NULL;
						}
						if (pResourceItem != NULL)
						{
							ContentPointer* pContentPointer = OK_NEW_OPERATOR ContentPointer(ccit1, ccit);
							pResourceItem->_pointers.Append(pContentPointer);
						}
					}
				}
				++state;
				break;
			}
			break;
		case 4:
			switch (state)
			{
			case 5:
				++state;
				break;
			}
			break;
		case 5:
			switch (state)
			{
			case 1:
				state = 0;
				break;
			case 6:
				state = 0;
				break;
			}
			break;
		default:
			break;
		}
		ccit.FindAnyStr(str2Find, str2FindLen, 6);
	}

	CHttpClient client;
	ServerItems::Iterator itS;
	ServerItem* pServerItem;
	ResourceItems::Iterator itR;
	ResourceItem* pResourceItem;
	CHttpClient::LoadOptions options;
	CFilePath path;
	CFilePath fpath;
	CDiskFile file;
	CDirectoryIterator dirIt;
	dword numFile;

	itS = _serverItems.Begin();
	while ( itS )
	{
		pServerItem = *itS;
		itR = pServerItem->_items.Begin();
		sBuf = _cachePath.get_Path();
		sBuf.AppendString(pServerItem->_server);
		sBuf.AppendString(_T(".images\\"));
		path.set_Path(sBuf);
		numFile = 0;
		try
		{
			CDirectoryIterator::MakeDirectory(path);
			path.set_Filename(_T("Image*.*"));
			dirIt.Open(path);
			while ( dirIt )
			{
				fpath = dirIt.get_Name();
				sBuf = fpath.get_Filename();
				sBuf.ScanString(_T("Image%d"), &i);
				if (i > numFile)
					numFile = i;
				++dirIt;
			}
		}
		catch ( CDirectoryIteratorException* )
		{
			dirIt.Close();
		}
		i = 0;
		while ( itR )
		{
			pResourceItem = *itR;
			options = CHttpClient::OPEN_TRANSIENT;
			if ( i == 0 )
				options = CHttpClient::OPEN_PERSISTENT;
			else if ( i < (pServerItem->_items.Count() - 1) )
				options = CHttpClient::STAY_PERSISTENT;
			else
				options = CHttpClient::CLOSE_PERSISTENT;
			client.ClearAll();
			if ( client.InitRequest(pServerItem->_server, pResourceItem->_resource) )
			{
				if ( client.Load(options) )
				{
					client.get_ResponseContent().GetBuffer(buffer);

					sBuf = client.get_ResponseData(CStringBuffer(__FILE__LINE__ _T("Content-Type")));
					if ( sBuf == CStringLiteral(_T("image/png")) )
					{
						numFile++;
						sBuf.FormatString(__FILE__LINE__ _T("Image%08d.png"), numFile);
					}
					else if ( sBuf == CStringLiteral(_T("image/gif")) )
					{
						numFile++;
						sBuf.FormatString(__FILE__LINE__ _T("Image%08d.gif"), numFile);
					}
					else if ( sBuf == CStringLiteral(_T("image/jpeg")) )
					{
						numFile++;
						sBuf.FormatString(__FILE__LINE__ _T("Image%08d.jpg"), numFile);
					}
					else if ( sBuf == CStringLiteral(_T("image/tiff")) )
					{
						numFile++;
						sBuf.FormatString(__FILE__LINE__ _T("Image%08d.tif"), numFile);
					}
					else
						sBuf.Clear();
					if ( !(sBuf.IsEmpty()) )
					{
						path.set_Filename(sBuf);
						try
						{
							file.Create(path);
							file.Write(buffer);
							file.Close();
							pResourceItem->_filePath = path;
						}
						catch ( CFileException* )
						{
							file.Close();
						}
					}
				}
				else
					break;
			}
			else
				break;
			++i;
			++itR;
		}
		++itS;
	}

	ContentPointers::Iterator itC;
	ContentPointer* pContentPointer;
	ReplacePointers replacePointers(__FILE__LINE__ 16, 16);
	ReplacePointer* pReplacePointer;

	itS = _serverItems.Begin();
	while ( itS )
	{
		pServerItem = *itS;
		itR = pServerItem->_items.Begin();
		while ( itR )
		{
			pResourceItem = *itR;
			itC = pResourceItem->_pointers.Begin();
			while ( itC )
			{
				pContentPointer = *itC;

				if ( !(pResourceItem->_filePath.get_Path().IsEmpty()) )
				{
					pReplacePointer = OK_NEW_OPERATOR ReplacePointer(pContentPointer->_begin, pContentPointer->_end, pResourceItem->_filePath);

					replacePointers.InsertSorted(pReplacePointer);
				}
				++itC;
			}
			++itR;
		}
		++itS;
	}

	ReplacePointers::Iterator itRe;

	itRe = replacePointers.Last();
	while ( itRe )
	{
		pReplacePointer = *itRe;
		sBuf = pReplacePointer->_filePath.get_Path();
		sBuf.PrependString(_T("file:///"));
		sBuf.ReplaceString(_T("\\"), _T("/"));
		sBuf.convertToByteBuffer(buffer);
		_documentBuffer.ReplaceSubBuffer(pReplacePointer->_begin, pReplacePointer->_end - pReplacePointer->_begin, buffer);
		--itRe;
	}
	replacePointers.Close();

	ccit = _documentBuffer.Begin();
	ccit.Find(CastAnyPtr(byte, CastMutablePtr(char, "<head>")), 6);
	if (ccit)
	{
		sBuf.FormatString(__FILE__LINE__ _T("<head><base href=\"http://%s\">"), _serverPath.GetString());
		sBuf.convertToByteBuffer(buffer);
		_documentBuffer.ReplaceSubBuffer(ccit, 6, buffer);
	}
}

bool CHtmlPage::GetAssocPath(ConstRef(CStringBuffer) url, Ref(CFilePath) path)
{
	CUrl vUrl;

	path.set_Path(__FILE__LINE__ _T(""));
	if (!(vUrl.set_Url(url)))
		return false;
	ServerItem sItem(vUrl.get_Server());
	ServerItems::Iterator itS = _serverItems.FindSorted(&sItem);

	if (_serverItems.MatchSorted(itS, &sItem))
	{
		ResourceItem rItem(vUrl.get_Resource());
		ResourceItems::Iterator itR = (*itS)->_items.FindSorted(&rItem);

		if ((*itS)->_items.MatchSorted(itR, &rItem))
		{
			path = (*itR)->_filePath;
			(*itR)->_updated = true;
			return true;
		}
	}
	return false;
}