/******************************************************************************
    
	This file is part of GuiSources, which is part of UserLib.

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
#include "stdafx.h"
#include "afxrsrc.h"
#include "GuiApplication.h"

//***********************************************************
// CBitmapData
//***********************************************************
class CBitmapData: public CCppObject
{
public:
	CBitmapData(HICON hIcon);
	CBitmapData(HBITMAP hBitmap);
	CBitmapData(BITMAPINFO* pBitmap, LPVOID pData);
	CBitmapData(LPCTSTR pResourceID, UINT fflag);
	CBitmapData(Gdiplus::Bitmap* pBitmap);
	virtual ~CBitmapData();
	
	__inline Gdiplus::Bitmap* get_bitmap() { return m_pBitmap; }

protected:
	HANDLE m_hResource;
	UINT m_fResourceType;
	LPVOID m_pData;
	BOOL m_ownBitmap;
	Gdiplus::Bitmap* m_pBitmap;
};

CBitmapData::CBitmapData(HICON hIcon)
{
	m_hResource = NULL;
	m_fResourceType = UINT_MAX;
	m_pData = NULL;
	m_ownBitmap = TRUE;
	m_pBitmap = Gdiplus::Bitmap::FromHICON(hIcon);
}

CBitmapData::CBitmapData(HBITMAP hBitmap)
{
	m_hResource = NULL;
	m_fResourceType = UINT_MAX;
	m_pData = NULL;
	m_ownBitmap = TRUE;
	m_pBitmap = Gdiplus::Bitmap::FromHBITMAP(hBitmap, NULL);
}

CBitmapData::CBitmapData(BITMAPINFO* pBitmap, LPVOID pData)
{
	m_hResource = NULL;
	m_fResourceType = UINT_MAX;
	m_pData = pData;
	m_ownBitmap = TRUE;
	m_pBitmap = Gdiplus::Bitmap::FromBITMAPINFO(pBitmap, pData);
}

CBitmapData::CBitmapData(LPCTSTR pResourceID, UINT fflag)
{
	m_hResource = ::LoadImage(theGuiApp->get_GuiInstance(), pResourceID, fflag, 0, 0, LR_DEFAULTCOLOR);
	m_fResourceType = fflag;
	m_pData = NULL;
	switch ( m_fResourceType )
	{
	case IMAGE_BITMAP:
		m_ownBitmap = TRUE;
		m_pBitmap = Gdiplus::Bitmap::FromHBITMAP((HBITMAP)m_hResource, NULL);
		break;
	case IMAGE_ICON:
		m_ownBitmap = TRUE;
		m_pBitmap = Gdiplus::Bitmap::FromHICON((HICON)m_hResource);
		break;
	default:
		m_ownBitmap = FALSE;
		m_pBitmap = NULL;
	}
}

CBitmapData::CBitmapData(Gdiplus::Bitmap* pBitmap)
{
	m_hResource = NULL;
	m_fResourceType = UINT_MAX;
	m_pData = NULL;
	m_ownBitmap = FALSE;
	m_pBitmap = pBitmap;
}

CBitmapData::~CBitmapData()
{
	if ( m_pBitmap && m_ownBitmap )
		delete m_pBitmap;
	m_pBitmap = NULL;
	if ( m_hResource )
	{
		switch ( m_fResourceType )
		{
		case IMAGE_BITMAP:
			::DeleteObject(m_hResource);
			break;
		case IMAGE_ICON:
			::DestroyIcon((HICON)m_hResource);
			break;
		default:
			break;
		}
		m_hResource = NULL;
		m_fResourceType = UINT_MAX;
	}
	if ( m_pData )
	{
		if ( TFisalloc(m_pData) )
			TFfree(m_pData);
		else
			free(m_pData);
		m_pData = NULL;
	}
}

//***********************************************************
// CBitmap
//***********************************************************
CBitmap::CBitmap()
{
	m_data = NULL;
}

CBitmap::CBitmap(HICON hIcon)
{
	m_data = OK_NEW_OPERATOR CBitmapData(hIcon);
}

CBitmap::CBitmap(HBITMAP hBitmap)
{
	m_data = OK_NEW_OPERATOR CBitmapData(hBitmap);
}

CBitmap::CBitmap(BITMAPINFO* pBitmap, LPVOID pData)
{
	m_data = OK_NEW_OPERATOR CBitmapData(pBitmap, pData);
}

CBitmap::CBitmap(LPCTSTR pResourceID, UINT fflag)
{
	m_data = OK_NEW_OPERATOR CBitmapData(pResourceID, fflag);
}

CBitmap::CBitmap(Gdiplus::Bitmap* pBitmap)
{
	m_data = OK_NEW_OPERATOR CBitmapData(pBitmap);
}

CBitmap::CBitmap(const CBitmap& copy)
{
	m_data = copy.m_data;
	if ( m_data )
		TFincrefcnt(m_data);
}

CBitmap::~CBitmap()
{
	if ( m_data )
	{
		if ( !(TFdecrefcnt(m_data)) )
			delete m_data;
		m_data = NULL;
	}
}

const CBitmap& CBitmap::operator=(const CBitmap& copy)
{
	if ( m_data )
	{
		if ( !(TFdecrefcnt(m_data)) )
			delete m_data;
		m_data = NULL;
	}
	m_data = copy.m_data;
	if ( m_data )
		TFincrefcnt(m_data);
	return *this;
}

const CBitmap& CBitmap::operator=(HICON hIcon)
{
	if ( m_data )
	{
		if ( !(TFdecrefcnt(m_data)) )
			delete m_data;
		m_data = NULL;
	}
	m_data = OK_NEW_OPERATOR CBitmapData(hIcon);
	return *this;
}

const CBitmap& CBitmap::operator=(HBITMAP hBitmap)
{
	if ( m_data )
	{
		if ( !(TFdecrefcnt(m_data)) )
			delete m_data;
		m_data = NULL;
	}
	m_data = OK_NEW_OPERATOR CBitmapData(hBitmap);
	return *this;
}

const CBitmap& CBitmap::operator=(Gdiplus::Bitmap* pBitmap)
{
	if ( m_data )
	{
		if ( !(TFdecrefcnt(m_data)) )
			delete m_data;
		m_data = NULL;
	}
	m_data = OK_NEW_OPERATOR CBitmapData(pBitmap);
	return *this;
}

Gdiplus::Bitmap* CBitmap::get_bitmap()
{
	if ( !m_data )
		return NULL;
	return m_data->get_bitmap();
}

HGLOBAL CBitmap::get_clipboarddata()
{
	if ( !m_data )
		return NULL;

	BITMAPINFOHEADER   bi;
	Gdiplus::Bitmap* pBitmap = m_data->get_bitmap();
     
	bi.biSize = sizeof(BITMAPINFOHEADER);    
	bi.biWidth = pBitmap->GetWidth();    
	bi.biHeight = pBitmap->GetHeight();  
	bi.biPlanes = 1;    
	bi.biBitCount = 32;    
	bi.biCompression = BI_RGB;    
	bi.biSizeImage = 0;  
	bi.biXPelsPerMeter = 0;    
	bi.biYPelsPerMeter = 0;    
	bi.biClrUsed = 0;    
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((bi.biWidth * bi.biBitCount + 31) / 32) * 4 * bi.biHeight;
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, bi.biSize + dwBmpSize);

	if ( hglbCopy ) 
	{
		DWORD* lptstrCopy = (DWORD*)(GlobalLock(hglbCopy));
		Gdiplus::Color color;

		MoveMemory(lptstrCopy, &bi, sizeof(BITMAPINFOHEADER));
		lptstrCopy = (DWORD *)(l_ptradd(lptstrCopy, sizeof(BITMAPINFOHEADER)));
		for ( INT i = bi.biHeight - 1; i >= 0; --i )
			for ( INT j = 0; j < bi.biWidth; ++j )
			{
				pBitmap->GetPixel(j, i, &color);
				*lptstrCopy++ = color.GetBlue() + (color.GetGreen() << 8) + (color.GetRed() << 16);
			}

		GlobalUnlock(hglbCopy);
		return hglbCopy;
	}
	return NULL;
}

//***********************************************************
// CImageListData
//***********************************************************
class CImageListData: public CBitmapData
{
public:
	typedef struct tagImageInfo
	{
		LPVOID pixeldata;
		Gdiplus::Bitmap* bitmap;
	} ImageInfo;

	typedef CDataSVectorT<ImageInfo> ImageInfoList;

public:
	CImageListData(HBITMAP hBitmap);
	CImageListData(BITMAPINFO* pBitmap, LPVOID pData);
	CImageListData(LPCTSTR pResourceID, UINT fflag);
	virtual ~CImageListData();

	__inline dword get_bitmapcnt() 
	{ 
		_createliste(); 
		return m_list.Count(); 
	}

	__inline Gdiplus::Bitmap* get_bitmap(dword ix) 
	{ 
		_createliste(); 
		if ( (m_list.Count() == 0) || (ix >= m_list.Count()) )
			return NULL;

		ImageInfoList::Iterator it = m_list.Index(ix);

		if ( !it )
			return NULL;
		return (*it)->bitmap;
	}

private:
	ImageInfoList m_list;

	void _createliste();
	void _createentry();
};

void CImageListData::_createliste()
{
	if ( (m_pBitmap == NULL) || (m_list.Count() > 0) )
		return;
	
	dword nHeight = m_pBitmap->GetHeight();
	dword nWidth = m_pBitmap->GetWidth();
	dword nCnt = nWidth / nHeight;

	for ( dword ix = 0; ix < nCnt; ++ix )
		_createentry();
}

void CImageListData::_createentry()
{
	BITMAPINFOHEADER bi;
 	dword nHeight = m_pBitmap->GetHeight();
	dword ix = m_list.Count();
    
	bi.biSize = sizeof(BITMAPINFOHEADER);    
	bi.biWidth = nHeight;    
	bi.biHeight = nHeight;  
	bi.biPlanes = 1;    
	bi.biBitCount = 32;    
	bi.biCompression = BI_RGB;    
	bi.biSizeImage = 0;  
	bi.biXPelsPerMeter = 0;    
	bi.biYPelsPerMeter = 0;    
	bi.biClrUsed = 0;    
	bi.biClrImportant = 0;

	DWORD dwBmpSize = nHeight * nHeight * sizeof(DWORD);
	DWORD* lptstrCopy = (DWORD*)(TFalloc(dwBmpSize));
	DWORD* lptstrCopySave = lptstrCopy;
	Gdiplus::Color color;

	for ( INT i = nHeight - 1; i >= 0; --i )
		for ( INT j = (ix * nHeight); j < (INT)((ix + 1) * nHeight); ++j )
		{
			m_pBitmap->GetPixel(j, i, &color);
			*lptstrCopy++ = color.GetBlue() + (color.GetGreen() << 8) + (color.GetRed() << 16);
		}

	ImageInfo info;

	info.pixeldata = lptstrCopySave;
	info.bitmap = Gdiplus::Bitmap::FromBITMAPINFO((BITMAPINFO*)(&bi), lptstrCopySave);
	m_list.Append(&info);
}

CImageListData::CImageListData(HBITMAP hBitmap):
    CBitmapData(hBitmap),
	m_list(__FILE__LINE__ 32, 32)
{
}

CImageListData::CImageListData(BITMAPINFO* pBitmap, LPVOID pData):
    CBitmapData(pBitmap, pData),
	m_list(__FILE__LINE__ 32, 32)
{
}

CImageListData::CImageListData(LPCTSTR pResourceID, UINT fflag):
    CBitmapData(pResourceID, fflag),
	m_list(__FILE__LINE__ 32, 32)
{
}

static void __stdcall TDeleteFunc_CImageListData( ConstPointer data, Pointer context )
{
	CImageListData::ImageInfo* pInfo = CastAnyPtr(CImageListData::ImageInfo, CastMutable(Pointer, data));

	TFfree(pInfo->pixeldata);
	delete pInfo->bitmap;
}

CImageListData::~CImageListData()
{
	m_list.Close(TDeleteFunc_CImageListData, NULL);
}

//***********************************************************
// CImageList
//***********************************************************
CImageList::CImageList()
{
	m_data = NULL;
}

CImageList::CImageList(HBITMAP hBitmap)
{
	m_data = OK_NEW_OPERATOR CImageListData(hBitmap);
}

CImageList::CImageList(BITMAPINFO* pBitmap, LPVOID pData)
{
	m_data = OK_NEW_OPERATOR CImageListData(pBitmap, pData);
}

CImageList::CImageList(LPCTSTR pResourceID, UINT fflag)
{
	m_data = OK_NEW_OPERATOR CImageListData(pResourceID, fflag);
}

CImageList::CImageList(const CImageList& copy)
{
	m_data = copy.m_data;
	if ( m_data )
		TFincrefcnt(m_data);
}

CImageList::~CImageList()
{
	if ( m_data )
	{
		if ( !(TFdecrefcnt(m_data)) )
			delete m_data;
		m_data = NULL;
	}
}

const CImageList& CImageList::operator=(const CImageList& copy)
{
	if ( m_data )
	{
		if ( !(TFdecrefcnt(m_data)) )
			delete m_data;
		m_data = NULL;
	}
	m_data = copy.m_data;
	if ( m_data )
		TFincrefcnt(m_data);
	return *this;
}

const CImageList& CImageList::operator=(HBITMAP hBitmap)
{
	if ( m_data )
	{
		if ( !(TFdecrefcnt(m_data)) )
			delete m_data;
		m_data = NULL;
	}
	m_data = OK_NEW_OPERATOR CImageListData(hBitmap);
	return *this;
}

dword CImageList::get_bitmapcnt()
{
	if ( !m_data )
		return 0;
	return m_data->get_bitmapcnt();
}

Gdiplus::Bitmap* CImageList::get_bitmap(dword ix)
{
	if ( !m_data )
		return NULL;
	return m_data->get_bitmap(ix);
}

