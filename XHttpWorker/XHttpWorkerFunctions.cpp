/******************************************************************************
    
	This file is part of XHttpWorker, which is part of UserLib.

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
#include "HttpServer.h"
#include "HttpDate.h"
#include "MD5Buffer.h"
#include "DataVector.h"
#include "DirectoryIterator.h"
#include "SecurityFile.h"
#include "SecurityContext.h"
#include "File.h"
#include "URL.h"
#include "ZLibFilter.h"
#include "GZipFilter.h"
#include "EventLogger.h"
#include "WinDirectoryIterator.h"

static CStringBuffer DumpQList(ConstRef(CHttpServer::RequestDataList) qlist)
{
	CHttpServer::RequestDataList::iterator qit(qlist.begin());
	CStringBuffer tmp;

	while (*qit)
	{
		CHttpServer::RequestDataItem qitem = (*qit)->item;

		tmp.AppendString(qitem.Key);
		tmp.AppendString(_T(": "));
		tmp.AppendString(qitem.Value);
		tmp.AppendString(_T("\n"));
		++qit;
	}
	return tmp;
}

static CStringBuffer DumpPList(ConstRef(CHttpServer::ResponseDataList) qlist)
{
	CHttpServer::ResponseDataList::iterator qit(qlist.begin());
	CStringBuffer tmp;

	while (*qit)
	{
		CHttpServer::ResponseDataItem qitem = (*qit)->item;

		tmp.AppendString(qitem.Key);
		tmp.AppendString(_T(": "));
		tmp.AppendString(qitem.Value);
		tmp.AppendString(_T("\n"));
		++qit;
	}
	return tmp;
}

class QualityFactorItem : public CCppObject
{
public:
	QualityFactorItem() {}
	QualityFactorItem(ConstRef(CStringBuffer) name, WDouble q): _name(name), _q(q) {}
	virtual ~QualityFactorItem() {}

	CStringBuffer DumpQ()
	{
		CStringBuffer tmp;

		tmp.FormatString(__FILE__LINE__ _T("%s;q=%lg, "), _name.GetString(), _q);
		return tmp;
	}

	bool CompareQ2(ConstPtr(QualityFactorItem) other) const
	{
		WULong l = _name.GetLength();
		WULong ol = other->_name.GetLength();

		if ((l < ol) && (_name.Compare(other->_name, l, CStringLiteral::cIgnoreCase | CStringLiteral::cLimited) == 0))
			return true;
		return false;
	}

	__inline ConstRef(CStringBuffer) get_Name() const { return _name; }
	__inline WDouble get_QualityFactor() const { return _q; }

protected:
	CStringBuffer _name;
	WDouble _q;
};

static void __stdcall QualityFactorItemDeleteFunc(ConstPointer data, Pointer context)
{
	Ptr(QualityFactorItem) item = CastAnyPtr(QualityFactorItem, CastMutable(Pointer, data));

	item->release();
}

static sword __stdcall QualityFactorItemSearchAndSortFunc2(ConstPointer ArrayItem, ConstPointer DataItem)
{
	Ptr(QualityFactorItem) pArrayItem = CastAnyPtr(QualityFactorItem, CastMutable(Pointer, ArrayItem));
	Ptr(QualityFactorItem) pDataItem = CastAnyPtr(QualityFactorItem, CastMutable(Pointer, DataItem));

	if (pArrayItem->CompareQ2(pDataItem) == -1)
		return 0;
	return 1;
}

class QualityFactorItemLessFunctor
{
public:
	bool operator()(ConstPtr(QualityFactorItem) r1, ConstPtr(QualityFactorItem) r2) const
	{
		return r1->CompareQ2(r2);
	}
};

class QualityFactorList : public CDataVectorT<QualityFactorItem>
{
	typedef CDataVectorT<QualityFactorItem> super;

public:
	QualityFactorList() : super(__FILE__LINE__ 16, 16) {}
	~QualityFactorList() {}

	void SplitQ(ConstRef(CStringBuffer) list)
	{
		super sorted(__FILE__LINE__ 16, 16);

		Split(list, _T(","));

		Iterator it = Begin();

		while (it)
		{
			Ptr(QualityFactorItem) item = *it;

			item->addRef();
			if (sorted.Count() == 0)
				sorted.Append(item);
			else
			{
				Iterator it2 = sorted.Find<QualityFactorItemLessFunctor>(item);

				if (it2)
					sorted.InsertBefore(it2, item);
				else
					sorted.Append(item);
			}
			++it;
		}
		while (Count() > 0)
			Remove(Last());
		it = sorted.Begin();
		while (it)
		{
			(*it)->addRef();
			Append(*it);
			++it;
		}
	}

	CStringBuffer DumpQ()
	{
		Iterator it = Begin();
		CStringBuffer tmp;

		while (it)
		{
			tmp.AppendString((*it)->DumpQ());
			++it;
		}
		return tmp;
	}

protected:
	void AppendQ(ConstRef(CStringBuffer) text)
	{
		CStringBuffer tmp(text);
		CStringBuffer tmp1;
		CPointer sar[8];
		dword max;
		WDouble q;
		Ptr(QualityFactorItem) item = NULL;

		tmp.Split(_T(";"), sar, 8, &max);
		switch (max)
		{
		case 1:
			tmp1.SetString(__FILE__LINE__ sar[0]);
			tmp1.Trim();
			item = OK_NEW_OPERATOR QualityFactorItem(tmp1, 1.0);
			break;
		case 2:
			tmp1.SetString(__FILE__LINE__ sar[1]);
			tmp1.Trim();
			tmp1.ScanString(_T("q=%lg"), &q);
			tmp1.SetString(__FILE__LINE__ sar[0]);
			tmp1.Trim();
			item = OK_NEW_OPERATOR QualityFactorItem(tmp1, q);
			break;
		default:
			CEventLogger::WriteLog(CEventLogger::Information, _T("QualityFactorList::AppendQ, max out of range"));
			return;
		}
		Append(item);
	}

	void Split(ConstRef(CStringBuffer) text, CStringLiteral sch)
	{
		CStringBuffer tmp(text);
		CStringConstIterator it(tmp);
		dword cnt = 1;
		dword schl = sch.GetLength();

		it.Find(sch.GetString());
		while (!(it.IsEnd()))
		{
			++cnt;
			it += schl;
			it.Find(sch.GetString());
		}

		CArray sar = CastAny(CArray, TFalloc(cnt * szPointer));
		dword max;

		tmp.Split(sch.GetString(), sar, cnt, &max);
		for (dword ix = 0; ix < max; ++ix)
		{
			CStringBuffer tmp(__FILE__LINE__ sar[ix]);

			tmp.Trim();
			AppendQ(tmp);
		}
		TFfree(sar);
	}
};

class AnalyzeQList
{
public:
	AnalyzeQList(ConstRef(CHttpServer::RequestDataList) qlist) : _qlist(qlist) {}
	~AnalyzeQList() {}

	void analyzeAccept()
	{
		CHttpServer::RequestDataItem item;
			
		item = _qlist.search(CStringBuffer(__FILE__LINE__ _T("Accept")));
		if (!(item.Value.IsEmpty()))
			_acceptMediaTypes.SplitQ(item.Value);
		item = _qlist.search(CStringBuffer(__FILE__LINE__ _T("AcceptCharset")));
		if (!(item.Value.IsEmpty()))
			_acceptCharSet.SplitQ(item.Value);
		item = _qlist.search(CStringBuffer(__FILE__LINE__ _T("AcceptEncoding")));
		if (!(item.Value.IsEmpty()))
			_acceptEncoding.SplitQ(item.Value);
		item = _qlist.search(CStringBuffer(__FILE__LINE__ _T("AcceptLanguage")));
		if (!(item.Value.IsEmpty()))
			_acceptLanguage.SplitQ(item.Value);
	}

	CStringBuffer DumpQ()
	{
		CStringBuffer tmp;

		if (_acceptMediaTypes.Count() > 0)
		{
			tmp.AppendString(_T("Accept: "));
			tmp.AppendString(_acceptMediaTypes.DumpQ());
		}
		if (_acceptCharSet.Count() > 0)
		{
			tmp.AppendString(_T("AcceptCharset: "));
			tmp.AppendString(_acceptCharSet.DumpQ());
		}
		if (_acceptEncoding.Count() > 0)
		{
			tmp.AppendString(_T("AcceptEncoding: "));
			tmp.AppendString(_acceptEncoding.DumpQ());
		}
		if (_acceptLanguage.Count() > 0)
		{
			tmp.AppendString(_T("AcceptLanguage: "));
			tmp.AppendString(_acceptLanguage.DumpQ());
		}
		return tmp;
	}

	__inline ConstRef(QualityFactorList) get_AcceptMediaTypes() const { return _acceptMediaTypes; }
	__inline ConstRef(QualityFactorList) get_AcceptCharSet() const { return _acceptCharSet; }
	__inline ConstRef(QualityFactorList) get_AcceptEncoding() const { return _acceptEncoding; }
	__inline ConstRef(QualityFactorList) get_AcceptLanguage() const { return _acceptLanguage; }

protected:
	ConstRef(CHttpServer::RequestDataList) _qlist;
	QualityFactorList _acceptMediaTypes;
	QualityFactorList _acceptCharSet;
	QualityFactorList _acceptEncoding;
	QualityFactorList _acceptLanguage;
};

class CByteLinkedBufferFilterOutput : public CFilterOutput
{
public:
	CByteLinkedBufferFilterOutput(Ref(CByteLinkedBuffer) buf): _buf(buf) {}
	virtual ~CByteLinkedBufferFilterOutput() {}

	virtual void open()
	{

	}

	virtual void write(Ref(CByteBuffer) outputbuf)
	{
		_buf.AddBufferItem(outputbuf);
	}

	virtual void close()
	{

	}


protected:
	Ref(CByteLinkedBuffer) _buf;
};

void RunClient(CConstPointer queueName, bool bLogging)
{
	CHttpServer client(true);
	CHttpServer::RequestDataList qlist(250);
	CHttpServer::RequestDataItem item;
	CHttpServer::ResponseDataList plist(250);
	QualityFactorList::Iterator it;
	CByteLinkedBuffer body;
	CByteBuffer bBuf;
	CStringBuffer tmp;
	CStringBuffer tmp2;
	long retCode;
	bool bstdfopen;
	bool bencoding;
	bool b404Error;
	bool bKeepAlive;
	DWORD trystate;
	DWORD svtrystate = 0;
	DWORD timeout = INFINITE;
	bool bContinue = true;
	bool bRecReq = true;

	client.OpenRequestQueue(queueName);
	while (bContinue)
	{
		retCode = 200;
		bstdfopen = true;
		bencoding = true;
		b404Error = true;
		try
		{
			if (bRecReq)
			{
				trystate = 0;

				client.ReceiveRequestHeader(qlist, timeout);
				if (qlist.count() == 0)
					break;
			}

			trystate = 1;

			CEventLogger::WriteLog(CEventLogger::Information, DumpQList(qlist));

			AnalyzeQList analyzer(qlist);

			analyzer.analyzeAccept();
			CEventLogger::WriteLog(CEventLogger::Information, analyzer.DumpQ());

			bKeepAlive = false;
			item = qlist.search(CStringBuffer(__FILE__LINE__ _T("Connection")));
			if (!(item.Value.IsEmpty()))
			{
				if (item.Value.Compare(CStringLiteral(_T("keep-alive")), 0, CStringLiteral::cIgnoreCase) == 0)
					bKeepAlive = true;
			}

			CHttpDate vDate;

			vDate.Now();
			tmp = vDate.GetDate();
			plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("Date")), tmp));
			plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("Server")), CStringBuffer(__FILE__LINE__ _T("Olivers HTTP Server 1.0, 2014"))));

			item = qlist.search(CStringBuffer(__FILE__LINE__ _T("$$Verb")));
			if (!(item.Value.IsEmpty()))
			{
				if (item.Value.Compare(CStringLiteral(_T("POST")), 0, CStringLiteral::cIgnoreCase) == 0)
				{
					body.Clear();
					client.ReceiveRequestBody(body);
					body.GetBuffer(bBuf);
					body.Clear();

					item = qlist.search(CStringBuffer(__FILE__LINE__ _T("ContentLength")));
					if (!(item.Value.IsEmpty()))
					{
						int l;

						tmp = item.Value;
						if (tmp.ScanString(_T("%d"), &l) >= 0)
							bBuf.set_BufferSize(__FILE__LINE__ l);
					}
					tmp.convertFromByteBuffer(bBuf);
					CEventLogger::WriteLog(CEventLogger::Information, tmp);

					item = qlist.search(CStringBuffer(__FILE__LINE__ _T("$$RawUrl")));
					if (!(item.Value.IsEmpty()))
					{
						qlist.remove(CStringBuffer(__FILE__LINE__ _T("$$RawUrl")));
						item.Value.AppendString(_T("?"));
						item.Value.AppendString(tmp);
						qlist.insert(item);
					}
				}
			}

			item = qlist.search(CStringBuffer(__FILE__LINE__ _T("$$RawUrl")));
			if (!(item.Value.IsEmpty()))
			{
				trystate = 2;

				CUrl rawurl(item.Value);
				CFilePath path(rawurl.get_Resource(), CDirectoryIterator::UnixPathSeparatorString());
				bool bPathExists = false;
				CFilePath path2;
				CStringBuffer md5Buffer;

				path.Normalize(_T("C:\\HttpServerRoot\\"));
				if (path.get_Filename().IsEmpty())
				{
					path.set_Filename(_T("index.html"));
					if (CWinDirectoryIterator::FileExists(path))
					{
						tmp = item.Value;
						tmp.AppendString(_T("index.html"));
						plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("Location")), tmp));
						retCode = 301;
						bstdfopen = false;
						bencoding = false;
						b404Error = false;
						bPathExists = true;
					}
				}
				else if (!(CWinDirectoryIterator::FileExists(path)) && (CWinDirectoryIterator::DirectoryExists(path) >= 0))
				{
					path.MakeDirectory();
					path.set_Filename(_T("index.html"));
					if (CWinDirectoryIterator::FileExists(path))
					{
						tmp = item.Value;
						tmp.AppendString(_T("/index.html"));
						plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("Location")), tmp));
						retCode = 301;
						bstdfopen = false;
						bencoding = false;
						b404Error = false;
						bPathExists = true;
					}
				}
				if (retCode == 200)
				{
					tmp = path.get_Directory(1);
					if (tmp.Compare(CStringLiteral(_T("homepage")), 0, CStringLiteral::cIgnoreCase) == 0)
					{
						it = analyzer.get_AcceptLanguage().Begin();
						while (it)
						{
							Ptr(QualityFactorItem) qfitem = *it;

							path2.set_Root(path.get_Root());
							path2.set_Directory(path.get_Directory(0), 0);
							path2.set_Directory(path.get_Directory(1), 1);
							path2.set_Directory(qfitem->get_Name(), 2);

							if (CWinDirectoryIterator::DirectoryExists(path2) >= 0)
							{
								plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("ContentLanguage")), qfitem->get_Name()));
								tmp = path.get_Path();
								tmp.ToLowerCase();
								tmp2.SetString(__FILE__LINE__ _T("homepage\\"));
								tmp2.AppendString(qfitem->get_Name());
								tmp.ReplaceString(_T("homepage"), tmp2);
								path.set_Path(tmp);
								bPathExists = false;
								tmp2.ReplaceString(_T("\\"), _T("/"));
								tmp = item.Value;
								tmp.ToLowerCase();
								tmp.ReplaceString(_T("homepage"), tmp2);
								plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("ContentLocation")), tmp));
								break;
							}
							++it;
						}
					}
				}
				if (!bPathExists)
					bPathExists = CWinDirectoryIterator::FileExists(path);

				CEventLogger::WriteFormattedLog(CEventLogger::Information, _T("path=%s, FileExists=%d, DirectoryExists=%d"),
					path.GetString(), Castdword(bPathExists), bPathExists ? -1 : CWinDirectoryIterator::DirectoryExists(path));

				if (bPathExists)
				{
					CSecurityFile file(path);
					CMD5Buffer buf(&file);
					CByteBuffer bBuf(__FILE__LINE__ 16);

					file.Close();
					md5Buffer = buf.GetDigest();
					md5Buffer.convertFromHex(bBuf);
					tmp.convertToBase64(bBuf);
					plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("Etag")), md5Buffer));
					plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("ContentMd5")), tmp));
				}
				if (bencoding && bPathExists)
				{
					trystate = 3;

					CSystemTime crDateTime;
					CSystemTime laDateTime;
					CSystemTime lmDateTime;
					bool bCreateContent = true;

					CWinDirectoryIterator::ReadFileTimes(path, crDateTime, laDateTime, lmDateTime);
					vDate.SetDate(lmDateTime);
					tmp = vDate.GetDate();
					plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("LastModified")), tmp));

					item = qlist.search(CStringBuffer(__FILE__LINE__ _T("IfNoneMatch")));
					if (!(item.Value.IsEmpty()))
					{
						if (item.Value.Compare(md5Buffer, 0, CStringLiteral::cIgnoreCase) == 0)
						{
							retCode = 304;
							bstdfopen = false;
							b404Error = false;
							bCreateContent = false;
						}
					}
					item = qlist.search(CStringBuffer(__FILE__LINE__ _T("IfModifiedSince")));
					if (!(item.Value.IsEmpty()))
					{
						CHttpDate vDate2;

						vDate2.SetDate(item.Value);
						if (vDate <= vDate2)
						{
							retCode = 304;
							bstdfopen = false;
							b404Error = false;
							bCreateContent = false;
						}
						else if (retCode == 304)
						{
							retCode = 200;
							bstdfopen = true;
							bCreateContent = true;
						}
					}
					item = qlist.search(CStringBuffer(__FILE__LINE__ _T("$$Verb")));
					if (!(item.Value.IsEmpty()))
					{
						if (item.Value.Compare(CStringLiteral(_T("HEAD")), 0, CStringLiteral::cIgnoreCase) == 0)
						{
							bstdfopen = false;
							b404Error = false;
							bCreateContent = false;
						}
					}
					it = analyzer.get_AcceptMediaTypes().Begin();
					while (it)
					{
						Ptr(QualityFactorItem) qfitem = *it;

						if ((path.get_Extension().Compare(CStringLiteral(_T("html")), 0, CStringLiteral::cIgnoreCase) == 0) &&
							(qfitem->get_Name().Compare(CStringLiteral(_T("text/html")), 0, CStringLiteral::cIgnoreCase) == 0))
						{
							plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("ContentType")), CStringBuffer(__FILE__LINE__ _T("text/html; charset=utf-8"))));
							break;
						}
						if ((path.get_Extension().Compare(CStringLiteral(_T("css")), 0, CStringLiteral::cIgnoreCase) == 0) &&
							(qfitem->get_Name().Compare(CStringLiteral(_T("text/css")), 0, CStringLiteral::cIgnoreCase) == 0))
						{
							plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("ContentType")), CStringBuffer(__FILE__LINE__ _T("text/css; charset=utf-8"))));
							break;
						}
						if ((path.get_Extension().Compare(CStringLiteral(_T("png")), 0, CStringLiteral::cIgnoreCase) == 0) &&
							(qfitem->get_Name().Compare(CStringLiteral(_T("image/png")), 0, CStringLiteral::cIgnoreCase) == 0))
						{
							plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("ContentType")), CStringBuffer(__FILE__LINE__ _T("image/png"))));
							bCreateContent = false;
							break;
						}
						if ((path.get_Extension().Compare(CStringLiteral(_T("gif")), 0, CStringLiteral::cIgnoreCase) == 0) &&
							(qfitem->get_Name().Compare(CStringLiteral(_T("image/*")), 0, CStringLiteral::cIgnoreCase) == 0))
						{
							plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("ContentType")), CStringBuffer(__FILE__LINE__ _T("image/gif"))));
							bCreateContent = false;
							break;
						}
						if ((path.get_Extension().Compare(CStringLiteral(_T("jpg")), 0, CStringLiteral::cIgnoreCase) == 0) &&
							(qfitem->get_Name().Compare(CStringLiteral(_T("image/*")), 0, CStringLiteral::cIgnoreCase) == 0))
						{
							plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("ContentType")), CStringBuffer(__FILE__LINE__ _T("image/jpg"))));
							bCreateContent = false;
							break;
						}
						++it;
					}
					if (bCreateContent)
					{
						it = analyzer.get_AcceptEncoding().Begin();
						while (it)
						{
							Ptr(QualityFactorItem) qfitem = *it;

							if (qfitem->get_Name().Compare(CStringLiteral(_T("deflate")), 0, CStringLiteral::cIgnoreCase) == 0)
							{
								CSecurityFile* pInputFile = OK_NEW_OPERATOR CSecurityFile(path);
								CFileFilterInput* pInput = OK_NEW_OPERATOR CFileFilterInput(pInputFile);
								CByteLinkedBufferFilterOutput* pOutput = OK_NEW_OPERATOR CByteLinkedBufferFilterOutput(body);

								CZLibCompressFilter* pFilter = OK_NEW_OPERATOR CZLibCompressFilter(pInput, pOutput);

								pFilter->open();
								pFilter->do_filter();
								pFilter->close();

								pFilter->release();
								pOutput->release();
								pInput->release();
								pInputFile->release();

								plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("ContentEncoding")), CStringBuffer(__FILE__LINE__ _T("deflate"))));
								plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("TransferEncoding")), CStringBuffer(__FILE__LINE__ _T("deflate"))));

								bstdfopen = false;
								b404Error = false;
								break;
							}
							if (qfitem->get_Name().Compare(CStringLiteral(_T("gzip")), 0, CStringLiteral::cIgnoreCase) == 0)
							{
								CSecurityFile* pInputFile = OK_NEW_OPERATOR CSecurityFile(path);
								CFileFilterInput* pInput = OK_NEW_OPERATOR CFileFilterInput(pInputFile);
								CByteLinkedBufferFilterOutput* pOutput = OK_NEW_OPERATOR CByteLinkedBufferFilterOutput(body);

								CGZipCompressFilter* pFilter = OK_NEW_OPERATOR CGZipCompressFilter(pInput, pOutput);

								pFilter->open();
								pFilter->do_filter();
								pFilter->close();

								pFilter->release();
								pOutput->release();
								pInput->release();
								pInputFile->release();

								plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("ContentEncoding")), CStringBuffer(__FILE__LINE__ _T("gzip"))));
								plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("TransferEncoding")), CStringBuffer(__FILE__LINE__ _T("gzip"))));

								bstdfopen = false;
								b404Error = false;
								break;
							}
							++it;
						}
					}
				}
				if (bstdfopen && bPathExists)
				{
					trystate = 4;

					CSecurityFile file(path);
					CFile::TFileSize sz = file.GetSize();
					dword isz;

					while (sz > 0)
					{
						isz = (sz > 4096) ? 4096 : Castdword(sz);
						body.AddBufferItem(isz);
						sz -= isz;
					}
					file.Read(body);
					file.Close();
					b404Error = false;
				}
				if (b404Error)
				{
					const char* sNotFound = "<html><head><title>404 - Not Found</title></head><body><p>Your request could not be fulfilled, because the requested resource was not found.</body></html>";
					size_t lNotFound = strlen(sNotFound);

					s_memcpy(body.AddBufferItem(Castdword(lNotFound)), sNotFound, lNotFound);
					retCode = 404;
				}
			}
			else
			{
				const char* sReqIncomplete = "<html><head><title>500 - Internal Server Error</title></head><body><p>Your request could not be fulfilled, because the request was incomplete.</body></html>";
				size_t lReqIncomplete = strlen(sReqIncomplete);

				s_memcpy(body.AddBufferItem(Castdword(lReqIncomplete)), sReqIncomplete, lReqIncomplete);
				retCode = 500;
			}
			trystate = 5;

			if ((retCode == 404) || (retCode == 500))
				bKeepAlive = false;
			if (bKeepAlive)
			{
				plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("Connection")), CStringBuffer(__FILE__LINE__ _T("Keep-Alive"))));
				plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("KeepAlive")), CStringBuffer(__FILE__LINE__ _T("500"))));
			}
			else
				plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("Connection")), CStringBuffer(__FILE__LINE__ _T("close"))));

			tmp.FormatString(__FILE__LINE__ _T("%ld"), body.GetTotalLength());
			plist.insert(CHttpServer::ResponseDataItem(CStringBuffer(__FILE__LINE__ _T("ContentLength")), tmp));
			client.SendResponse(client.get_StatusCode(retCode), plist, body, bLogging);
			CEventLogger::WriteLog(CEventLogger::Information, DumpPList(plist));
			if (!bKeepAlive)
				bContinue = false;
			bRecReq = true;
			timeout = 500;
			qlist.clear();
			plist.clear();
		}
		catch (CBaseException* ex)
		{
			CEventLogger::WriteLog(CEventLogger::Error, ex->GetExceptionMessage());
			switch (trystate)
			{
			case 0: // client.ReceiveRequestHeader
				bContinue = false;
				break;
			case 1:
				if (svtrystate == 1)
				{
					bContinue = false;
					break;
				}
				bContinue = true;
				bRecReq = false;
				plist.clear();
				svtrystate = 1;
				break;
			case 2:
				if (svtrystate == 2)
				{
					bContinue = false;
					break;
				}
				item = qlist.search(CStringBuffer(__FILE__LINE__ _T("$$RawUrl")));
				// TODO: check item
				bContinue = true;
				bRecReq = false;
				plist.clear();
				svtrystate = 2;
				break;
			case 3:
				if (svtrystate == 3)
				{
					bContinue = false;
					break;
				}
				bContinue = true;
				bRecReq = false;
				plist.clear();
				svtrystate = 3;
				break;
			case 4:
				if (svtrystate == 4)
				{
					bContinue = false;
					break;
				}
				bContinue = true;
				bRecReq = false;
				plist.clear();
				svtrystate = 4;
				break;
			case 5: // client.SendResponse
				bContinue = false;
				break;
			}
		}
	}
	try
	{
		client.ShutdownRequestQueue();
	}
	catch (CBaseException* ex)
	{
		CEventLogger::WriteLog(CEventLogger::Error, ex->GetExceptionMessage());
	}
	try
	{
		client.CloseRequestQueue();
	}
	catch (CBaseException* ex)
	{
		CEventLogger::WriteLog(CEventLogger::Error, ex->GetExceptionMessage());
	}
	CSecurityContext_FreeInstance
	CEventLogger::CleanUp();
}