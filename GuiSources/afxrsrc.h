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
#pragma once

class CBitmapData;
class CBitmap
{
public:
	CBitmap();
	CBitmap(HICON hIcon);
	CBitmap(HBITMAP hBitmap);
	CBitmap(BITMAPINFO* pBitmap, LPVOID pData);
	CBitmap(LPCTSTR pResourceID, UINT fflag);
	CBitmap(Gdiplus::Bitmap* pBitmap);
	CBitmap(const CBitmap& copy);
	~CBitmap();

	const CBitmap& operator=(const CBitmap& copy);
	const CBitmap& operator=(HICON hIcon);
	const CBitmap& operator=(HBITMAP hBitmap);
	const CBitmap& operator=(Gdiplus::Bitmap* pBitmap);

	Gdiplus::Bitmap* get_bitmap();
	HGLOBAL get_clipboarddata();

private:
	CBitmapData* m_data;
};

class CImageListData;
class CImageList
{
public:
	CImageList();
	CImageList(HBITMAP hBitmap);
	CImageList(BITMAPINFO* pBitmap, LPVOID pData);
	CImageList(LPCTSTR pResourceID, UINT fflag);
	CImageList(const CImageList& copy);
	~CImageList();

	const CImageList& operator=(const CImageList& copy);
	const CImageList& operator=(HBITMAP hBitmap);

	dword get_bitmapcnt();
	Gdiplus::Bitmap* get_bitmap(dword ix);

private:
	CImageListData* m_data;
};
