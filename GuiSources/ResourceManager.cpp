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
#include "StdAfx.h"
#include "ResourceManager.h"
#include "Configuration.h"
#include "GuiApplication.h"
#include "UTLPTR.H"

//***********************************************************
// CResourceManager
//***********************************************************
CResourceManager::CResourceManager(void):
	m_resourceCache(__FILE__LINE__ 64, 64)
{
}

CResourceManager::~CResourceManager(void)
{
}

void CResourceManager::add_resource(LPTSTR wndclass, CResourceManager::TResourceType rsrctype, DWORD id)
{
	Ptr(TResourceItem) item = OK_NEW_OPERATOR TResourceItem(wndclass, rsrctype, id);
	TResourceItems::Iterator it = m_resourceCache.FindSorted(item);

	if (it)
	{
		m_resourceCache.SetData(it, item);
		return;
	}
	m_resourceCache.InsertSorted(item);
}

HANDLE CResourceManager::get_resource(LPTSTR wndclass, CResourceManager::TResourceType rsrctype)
{
	TResourceItem item(wndclass, rsrctype);
	TResourceItems::Iterator it = m_resourceCache.FindSorted(&item);

	if (it)
	{
		if ( (*it)->CachedResource != nullptr )
			return (*it)->CachedResource;
		switch ( (*it)->ResourceType ) 
		{
		case WindowTitle:
			(*it)->CachedResource = TFalloc(100 * szchar);
			::LoadString(theGuiApp->get_GuiInstance(), (*it)->ResourceID, CastAny(LPTSTR, (*it)->CachedResource), 100);
			break;
		case WindowAccelarator:
			(*it)->CachedResource = ::LoadAccelerators(theGuiApp->get_GuiInstance(), MAKEINTRESOURCE((*it)->ResourceID));
			break;
		case WindowIcon:
			(*it)->CachedResource = ::LoadImage(theGuiApp->get_GuiInstance(), MAKEINTRESOURCE((*it)->ResourceID), IMAGE_ICON, 0, 0, LR_SHARED);
			break;
		case WindowSmallIcon:
			(*it)->CachedResource = ::LoadImage(theGuiApp->get_GuiInstance(), MAKEINTRESOURCE((*it)->ResourceID), IMAGE_ICON, 0, 0, LR_SHARED);
			break;
		case WindowMenu:
			(*it)->CachedResource = ::LoadMenu(theGuiApp->get_GuiInstance(), MAKEINTRESOURCE((*it)->ResourceID));
			break;
		default:
			(*it)->CachedResource = nullptr;
			break;
		}
		return (*it)->CachedResource;
	}
	return nullptr;
}

DWORD CResourceManager::get_resourceID(LPTSTR wndclass, CResourceManager::TResourceType rsrctype)
{
	TResourceItem item(wndclass, rsrctype);
	TResourceItems::Iterator it = m_resourceCache.FindSorted(&item);

	if (it)
		return (*it)->ResourceID;
	return 0;
}

//***********************************************************
// CFontManager
//***********************************************************
CFontManager::CFontManager(void):
	m_prefix(),
	m_pDefaultFont(new Gdiplus::Font(TEXT("Arial"), 8)),
	m_fontCache(__FILE__LINE__ 64, 64)
{
}

CFontManager::CFontManager(CStringLiteral _prefix):
	m_prefix(__FILE__LINE__ _prefix),
	m_pDefaultFont(new Gdiplus::Font(TEXT("Arial"), 8)),
	m_fontCache(__FILE__LINE__ 64, 64)
{
}

CFontManager::~CFontManager(void)
{
	delete m_pDefaultFont;
}

Gdiplus::Font* CFontManager::parse_Font(ConstRef(CStringBuffer) _text)
{
	CStringBuffer vFontData(_text);
	CPointer vSplit[64];
	dword vSplitCnt = 0;
	CStringBuffer vBuffer;
	CStringBuffer vFontFamily(__FILE__LINE__ _T("Arial"));
	WLong emSize = 8;
	Gdiplus::Unit emSizeUnit = Gdiplus::UnitPoint;
	INT fontStyle = Gdiplus::FontStyleRegular;

	vFontData.Split(_T(","), vSplit, 64, &vSplitCnt);
	if ( vSplitCnt == 0 )
		return nullptr;
	if ( vSplitCnt > 0 )
	{
		vFontFamily.SetString(__FILE__LINE__ vSplit[0]);
		vFontFamily.Trim();
	}
	if ( vSplitCnt > 1 )
	{
		CStringConstIterator it(vSplit[1]);
		CPointer vWord;
		WULong vWordLen;

		it.EatWhite();
		it.EatDecimal(emSize);
		it.EatWhite();
		it.EatWord(vWord, vWordLen);
		vBuffer.SetString(__FILE__LINE__ vWord, vWordLen);
		if ( vBuffer.Compare(_T("Pt"), 0, CStringLiteral::cIgnoreCase) == 0 )
			emSizeUnit = Gdiplus::UnitPoint;
		else if ( vBuffer.Compare(_T("Point"), 0, CStringLiteral::cIgnoreCase) == 0 )
			emSizeUnit = Gdiplus::UnitPoint;
		else if ( vBuffer.Compare(_T("Pixel"), 0, CStringLiteral::cIgnoreCase) == 0 )
			emSizeUnit = Gdiplus::UnitPixel;
	}
	if ( vSplitCnt > 2 )
	{
		CPointer vSplit1[64];
		dword vSplitCnt1 = 0;
		CStringLiteral vBuffer1;

		vBuffer.SetString(__FILE__LINE__ vSplit[2]);
		vBuffer.Trim();
		vBuffer.Split(_T(" "), vSplit1, 64, &vSplitCnt1);

		for ( dword ix = 0; ix < vSplitCnt1; ++ix )
		{
			vBuffer1 = vSplit1[ix];

			if ( vBuffer1.Compare(_T("Regular"), 0, CStringLiteral::cIgnoreCase) == 0 )
			{
				fontStyle = Gdiplus::FontStyleRegular;
				break;
			}
			if ( vBuffer1.Compare(_T("Bold"), 0, CStringLiteral::cIgnoreCase) == 0 )
				fontStyle |= Gdiplus::FontStyleBold;
			else if ( vBuffer1.Compare(_T("Italic"), 0, CStringLiteral::cIgnoreCase) == 0 )
				fontStyle |= Gdiplus::FontStyleItalic;
			else if ( vBuffer1.Compare(_T("Underline"), 0, CStringLiteral::cIgnoreCase) == 0 )
				fontStyle |= Gdiplus::FontStyleUnderline;
			else if ( vBuffer1.Compare(_T("Strikeout"), 0, CStringLiteral::cIgnoreCase) == 0 )
				fontStyle |= Gdiplus::FontStyleStrikeout;
		}
	}

	Gdiplus::FontFamily fontFamily(vFontFamily);

	if ( fontFamily.GetLastStatus() != Gdiplus::Ok )
		return nullptr;

	Gdiplus::Font* vFont = new Gdiplus::Font(&fontFamily, Cast(Gdiplus::REAL, emSize), fontStyle, emSizeUnit);

	if ( vFont->GetLastStatus() != Gdiplus::Ok )
	{
		delete vFont;
		return nullptr;
	}
	return vFont;
}

bool CFontManager::has_Font(CStringLiteral _key)
{
	if ( _key.IsEmpty() )
		return false;

	TFontItem item(_key);
	TFontItems::Iterator it = m_fontCache.FindSorted(&item);

	if (it)
		return true;

	if ( !(m_prefix.IsEmpty()) )
	{
		CStringBuffer vPrefix;

		vPrefix = m_prefix;
		vPrefix.AppendString(_T("User."));
		vPrefix.AppendString(_key);

		if ( theGuiApp->config()->HasValue(vPrefix) )
			return true;

		vPrefix = m_prefix;
		vPrefix.AppendString(_T("."));
		vPrefix.AppendString(_key);

		return (theGuiApp->config()->HasValue(vPrefix));
	}
	return (theGuiApp->config()->HasValue(_key));
}

Gdiplus::Font* CFontManager::get_Font(CStringLiteral _key)
{
	if ( _key.IsEmpty() )
		return m_pDefaultFont;

	TFontItem item(_key);
	TFontItems::Iterator it = m_fontCache.FindSorted(&item);

	if (it)
		return (*it)->font;

	if ( !(m_prefix.IsEmpty()) )
	{
		CStringBuffer vPrefix;

		vPrefix = m_prefix;
		vPrefix.AppendString(_T("User."));
		vPrefix.AppendString(_key);

		if ( theGuiApp->config()->HasValue(vPrefix) )
		{
			item.font = parse_Font(theGuiApp->config()->GetValue(vPrefix));
			if ( !(item.font) )
				return m_pDefaultFont;
			item.prefix.addRef();
			m_fontCache.InsertSorted(OK_NEW_OPERATOR TFontItem(item));
			return item.font;
		}

		vPrefix = m_prefix;
		vPrefix.AppendString(_T("."));
		vPrefix.AppendString(_key);

		if ( theGuiApp->config()->HasValue(vPrefix) )
		{
			item.font = parse_Font(theGuiApp->config()->GetValue(vPrefix));
			if ( !(item.font) )
				return m_pDefaultFont;
			item.prefix.addRef();
			m_fontCache.InsertSorted(OK_NEW_OPERATOR TFontItem(item));
			return item.font;
		}
	}
	if ( theGuiApp->config()->HasValue(_key) )
	{
		item.font = parse_Font(theGuiApp->config()->GetValue(_key));
		if ( !(item.font) )
			return m_pDefaultFont;
		item.prefix.addRef();
		m_fontCache.InsertSorted(OK_NEW_OPERATOR TFontItem(item));
		return item.font;
	}
	return m_pDefaultFont;
}

void CFontManager::set_Font(CStringLiteral _key, ConstRef(CStringBuffer) _text)
{
	if ( _key.IsEmpty() )
		return;
	
	bool bDelete = _text.IsEmpty();
	Gdiplus::Font* pFont = nullptr;

	if ( !bDelete )
	{
		pFont = parse_Font(_text);
		if ( !pFont )
			return;
	}
	if ( !(m_prefix.IsEmpty()) )
	{
		CStringBuffer vPrefix;

		vPrefix = m_prefix;
		vPrefix.AppendString(_T("User."));
		vPrefix.AppendString(_key);

		theGuiApp->config()->SetValue(vPrefix, _text);
	}
	else
		theGuiApp->config()->SetValue(_key, _text);

	TFontItem item(_key);
	TFontItems::Iterator it = m_fontCache.FindSorted(&item);

	if (it)
	{
		if ( bDelete )
			m_fontCache.Remove(it);
		else
		{
			delete (*it)->font;
			(*it)->font = pFont;
		}
		return;
	}
	if ( !bDelete )
	{
		item.font = pFont;
		item.prefix.addRef();
		m_fontCache.InsertSorted(OK_NEW_OPERATOR TFontItem(item));
	}
}

//***********************************************************
// CBrushManager
//***********************************************************
class TBrushItemEqualFunctor
{
public:
	bool operator()(ConstPtr(CBrushManager::TBrushItem) ppa, ConstPtr(CBrushManager::TBrushItem) ppd) const
	{
		sdword res = Castsdword(v_ptrdiff(ppa->brush, ppd->brush));

		if (res == 0)
		{
			if (ppa->bPrivateCopy == ppd->bPrivateCopy)
				return true;
		}
		return false;
	}
};

static struct TBrushManagerNamedColors
{
	LPCTSTR name;
	Gdiplus::Color color;
} vBrushManagerNamedColors[] = {
	{ _T("AliceBlue"), Gdiplus::Color(Gdiplus::Color::AliceBlue) },
	{ _T("AntiqueWhite"), Gdiplus::Color(Gdiplus::Color::AntiqueWhite) },
	{ _T("Aqua"), Gdiplus::Color(Gdiplus::Color::Aqua) },
	{ _T("Aquamarine"), Gdiplus::Color(Gdiplus::Color::Aquamarine) },
	{ _T("Azure"), Gdiplus::Color(Gdiplus::Color::Azure) },
	{ _T("Beige"), Gdiplus::Color(Gdiplus::Color::Beige) },
	{ _T("Bisque"), Gdiplus::Color(Gdiplus::Color::Bisque) },
	{ _T("Black"), Gdiplus::Color(Gdiplus::Color::Black) },
	{ _T("BlanchedAlmond"), Gdiplus::Color(Gdiplus::Color::BlanchedAlmond) },
	{ _T("Blue"), Gdiplus::Color(Gdiplus::Color::Blue) },
	{ _T("BlueViolet"), Gdiplus::Color(Gdiplus::Color::BlueViolet) },
	{ _T("Brown"), Gdiplus::Color(Gdiplus::Color::Brown) },
	{ _T("BurlyWood"), Gdiplus::Color(Gdiplus::Color::BurlyWood) },
	{ _T("CadetBlue"), Gdiplus::Color(Gdiplus::Color::CadetBlue) },
	{ _T("Chartreuse"), Gdiplus::Color(Gdiplus::Color::Chartreuse) },
	{ _T("Chocolate"), Gdiplus::Color(Gdiplus::Color::Chocolate) },
	{ _T("Coral"), Gdiplus::Color(Gdiplus::Color::Coral) },
	{ _T("CornflowerBlue"), Gdiplus::Color(Gdiplus::Color::CornflowerBlue) },
	{ _T("Cornsilk"), Gdiplus::Color(Gdiplus::Color::Cornsilk) },
	{ _T("Crimson"), Gdiplus::Color(Gdiplus::Color::Crimson) },
	{ _T("Cyan"), Gdiplus::Color(Gdiplus::Color::Cyan) },
	{ _T("DarkBlue"), Gdiplus::Color(Gdiplus::Color::DarkBlue) },
	{ _T("DarkCyan"), Gdiplus::Color(Gdiplus::Color::DarkCyan) },
	{ _T("DarkGoldenrod"), Gdiplus::Color(Gdiplus::Color::DarkGoldenrod) },
	{ _T("DarkGray"), Gdiplus::Color(Gdiplus::Color::DarkGray) },
	{ _T("DarkGreen"), Gdiplus::Color(Gdiplus::Color::DarkGreen) },
	{ _T("DarkKhaki"), Gdiplus::Color(Gdiplus::Color::DarkKhaki) },
	{ _T("DarkMagenta"), Gdiplus::Color(Gdiplus::Color::DarkMagenta) },
	{ _T("DarkOliveGreen"), Gdiplus::Color(Gdiplus::Color::DarkOliveGreen) },
	{ _T("DarkOrange"), Gdiplus::Color(Gdiplus::Color::DarkOrange) },
	{ _T("DarkOrchid"), Gdiplus::Color(Gdiplus::Color::DarkOrchid) },
	{ _T("DarkRed"), Gdiplus::Color(Gdiplus::Color::DarkRed) },
	{ _T("DarkSalmon"), Gdiplus::Color(Gdiplus::Color::DarkSalmon) },
	{ _T("DarkSeaGreen"), Gdiplus::Color(Gdiplus::Color::DarkSeaGreen) },
	{ _T("DarkSlateBlue"), Gdiplus::Color(Gdiplus::Color::DarkSlateBlue) },
	{ _T("DarkSlateGray"), Gdiplus::Color(Gdiplus::Color::DarkSlateGray) },
	{ _T("DarkTurquoise"), Gdiplus::Color(Gdiplus::Color::DarkTurquoise) },
	{ _T("DarkViolet"), Gdiplus::Color(Gdiplus::Color::DarkViolet) },
	{ _T("DeepPink"), Gdiplus::Color(Gdiplus::Color::DeepPink) },
	{ _T("DeepSkyBlue"), Gdiplus::Color(Gdiplus::Color::DeepSkyBlue) },
	{ _T("DimGray"), Gdiplus::Color(Gdiplus::Color::DimGray) },
	{ _T("DodgerBlue"), Gdiplus::Color(Gdiplus::Color::DodgerBlue) },
	{ _T("Firebrick"), Gdiplus::Color(Gdiplus::Color::Firebrick) },
	{ _T("FloralWhite"), Gdiplus::Color(Gdiplus::Color::FloralWhite) },
    { _T("ForestGreen"), Gdiplus::Color(Gdiplus::Color::ForestGreen) },
    { _T("Fuchsia"), Gdiplus::Color(Gdiplus::Color::Fuchsia) },
    { _T("Gainsboro"), Gdiplus::Color(Gdiplus::Color::Gainsboro) },
    { _T("GhostWhite"), Gdiplus::Color(Gdiplus::Color::GhostWhite) },
    { _T("Gold"), Gdiplus::Color(Gdiplus::Color::Gold) },
    { _T("Goldenrod"), Gdiplus::Color(Gdiplus::Color::Goldenrod) },
    { _T("Gray"), Gdiplus::Color(Gdiplus::Color::Gray) },
    { _T("Green"), Gdiplus::Color(Gdiplus::Color::Green) },
    { _T("GreenYellow"), Gdiplus::Color(Gdiplus::Color::GreenYellow) },
    { _T("Honeydew"), Gdiplus::Color(Gdiplus::Color::Honeydew) },
    { _T("HotPink"), Gdiplus::Color(Gdiplus::Color::HotPink) },
    { _T("IndianRed"), Gdiplus::Color(Gdiplus::Color::IndianRed) },
    { _T("Indigo"), Gdiplus::Color(Gdiplus::Color::Indigo) },
    { _T("Ivory"), Gdiplus::Color(Gdiplus::Color::Ivory) },
    { _T("Khaki"), Gdiplus::Color(Gdiplus::Color::Khaki) },
    { _T("Lavender"), Gdiplus::Color(Gdiplus::Color::Lavender) },
    { _T("LavenderBlush"), Gdiplus::Color(Gdiplus::Color::LavenderBlush) },
    { _T("LawnGreen"), Gdiplus::Color(Gdiplus::Color::LawnGreen) },
    { _T("LemonChiffon"), Gdiplus::Color(Gdiplus::Color::LemonChiffon) },
    { _T("LightBlue"), Gdiplus::Color(Gdiplus::Color::LightBlue) },
    { _T("LightCoral"), Gdiplus::Color(Gdiplus::Color::LightCoral) },
    { _T("LightCyan"), Gdiplus::Color(Gdiplus::Color::LightCyan) },
    { _T("LightGoldenrodYellow"), Gdiplus::Color(Gdiplus::Color::LightGoldenrodYellow) },
    { _T("LightGray"), Gdiplus::Color(Gdiplus::Color::LightGray) },
    { _T("LightGreen"), Gdiplus::Color(Gdiplus::Color::LightGreen) },
    { _T("LightPink"), Gdiplus::Color(Gdiplus::Color::LightPink) },
    { _T("LightSalmon"), Gdiplus::Color(Gdiplus::Color::LightSalmon) },
    { _T("LightSeaGreen"), Gdiplus::Color(Gdiplus::Color::LightSeaGreen) },
    { _T("LightSkyBlue"), Gdiplus::Color(Gdiplus::Color::LightSkyBlue) },
    { _T("LightSlateGray"), Gdiplus::Color(Gdiplus::Color::LightSlateGray) },
    { _T("LightSteelBlue"), Gdiplus::Color(Gdiplus::Color::LightSteelBlue) },
    { _T("LightYellow"), Gdiplus::Color(Gdiplus::Color::LightYellow) },
    { _T("Lime"), Gdiplus::Color(Gdiplus::Color::Lime) },
    { _T("LimeGreen"), Gdiplus::Color(Gdiplus::Color::LimeGreen) },
    { _T("Linen"), Gdiplus::Color(Gdiplus::Color::Linen) },
    { _T("Magenta"), Gdiplus::Color(Gdiplus::Color::Magenta) },
    { _T("Maroon"), Gdiplus::Color(Gdiplus::Color::Maroon) },
    { _T("MediumAquamarine"), Gdiplus::Color(Gdiplus::Color::MediumAquamarine) },
    { _T("MediumBlue"), Gdiplus::Color(Gdiplus::Color::MediumBlue) },
    { _T("MediumOrchid"), Gdiplus::Color(Gdiplus::Color::MediumOrchid) },
    { _T("MediumPurple"), Gdiplus::Color(Gdiplus::Color::MediumPurple) },
    { _T("MediumSeaGreen"), Gdiplus::Color(Gdiplus::Color::MediumSeaGreen) },
    { _T("MediumSlateBlue"), Gdiplus::Color(Gdiplus::Color::MediumSlateBlue) },
    { _T("MediumSpringGreen"), Gdiplus::Color(Gdiplus::Color::MediumSpringGreen) },
    { _T("MediumTurquoise"), Gdiplus::Color(Gdiplus::Color::MediumTurquoise) },
    { _T("MediumVioletRed"), Gdiplus::Color(Gdiplus::Color::MediumVioletRed) },
    { _T("MidnightBlue"), Gdiplus::Color(Gdiplus::Color::MidnightBlue) },
    { _T("MintCream"), Gdiplus::Color(Gdiplus::Color::MintCream) },
    { _T("MistyRose"), Gdiplus::Color(Gdiplus::Color::MistyRose) },
    { _T("Moccasin"), Gdiplus::Color(Gdiplus::Color::Moccasin) },
    { _T("NavajoWhite"), Gdiplus::Color(Gdiplus::Color::NavajoWhite) },
    { _T("Navy"), Gdiplus::Color(Gdiplus::Color::Navy) },
    { _T("OldLace"), Gdiplus::Color(Gdiplus::Color::OldLace) },
    { _T("Olive"), Gdiplus::Color(Gdiplus::Color::Olive) },
    { _T("OliveDrab"), Gdiplus::Color(Gdiplus::Color::OliveDrab) },
    { _T("Orange"), Gdiplus::Color(Gdiplus::Color::Orange) },
    { _T("OrangeRed"), Gdiplus::Color(Gdiplus::Color::OrangeRed) },
    { _T("Orchid"), Gdiplus::Color(Gdiplus::Color::Orchid) },
    { _T("PaleGoldenrod"), Gdiplus::Color(Gdiplus::Color::PaleGoldenrod) },
    { _T("PaleGreen"), Gdiplus::Color(Gdiplus::Color::PaleGreen) },
    { _T("PaleTurquoise"), Gdiplus::Color(Gdiplus::Color::PaleTurquoise) },
    { _T("PaleVioletRed"), Gdiplus::Color(Gdiplus::Color::PaleVioletRed) },
    { _T("PapayaWhip"), Gdiplus::Color(Gdiplus::Color::PapayaWhip) },
    { _T("PeachPuff"), Gdiplus::Color(Gdiplus::Color::PeachPuff) },
    { _T("Peru"), Gdiplus::Color(Gdiplus::Color::Peru) },
    { _T("Pink"), Gdiplus::Color(Gdiplus::Color::Pink) },
    { _T("Plum"), Gdiplus::Color(Gdiplus::Color::Plum) },
    { _T("PowderBlue"), Gdiplus::Color(Gdiplus::Color::PowderBlue) },
    { _T("Purple"), Gdiplus::Color(Gdiplus::Color::Purple) },
    { _T("Red"), Gdiplus::Color(Gdiplus::Color::Red) },
    { _T("RosyBrown"), Gdiplus::Color(Gdiplus::Color::RosyBrown) },
    { _T("RoyalBlue"), Gdiplus::Color(Gdiplus::Color::RoyalBlue) },
    { _T("SaddleBrown"), Gdiplus::Color(Gdiplus::Color::SaddleBrown) },
    { _T("Salmon"), Gdiplus::Color(Gdiplus::Color::Salmon) },
    { _T("SandyBrown"), Gdiplus::Color(Gdiplus::Color::SandyBrown) },
    { _T("SeaGreen"), Gdiplus::Color(Gdiplus::Color::SeaGreen) },
    { _T("SeaShell"), Gdiplus::Color(Gdiplus::Color::SeaShell) },
    { _T("Sienna"), Gdiplus::Color(Gdiplus::Color::Sienna) },
    { _T("Silver"), Gdiplus::Color(Gdiplus::Color::Silver) },
    { _T("SkyBlue"), Gdiplus::Color(Gdiplus::Color::SkyBlue) },
    { _T("SlateBlue"), Gdiplus::Color(Gdiplus::Color::SlateBlue) },
    { _T("SlateGray"), Gdiplus::Color(Gdiplus::Color::SlateGray) },
    { _T("Snow"), Gdiplus::Color(Gdiplus::Color::Snow) },
    { _T("SpringGreen"), Gdiplus::Color(Gdiplus::Color::SpringGreen) },
    { _T("SteelBlue"), Gdiplus::Color(Gdiplus::Color::SteelBlue) },
    { _T("Tan"), Gdiplus::Color(Gdiplus::Color::Tan) },
    { _T("Teal"), Gdiplus::Color(Gdiplus::Color::Teal) },
    { _T("Thistle"), Gdiplus::Color(Gdiplus::Color::Thistle) },
    { _T("Tomato"), Gdiplus::Color(Gdiplus::Color::Tomato) },
    { _T("Transparent"), Gdiplus::Color(Gdiplus::Color::Transparent) },
    { _T("Turquoise"), Gdiplus::Color(Gdiplus::Color::Turquoise) },
    { _T("Violet"), Gdiplus::Color(Gdiplus::Color::Violet) },
    { _T("Wheat"), Gdiplus::Color(Gdiplus::Color::Wheat) },
    { _T("White"), Gdiplus::Color(Gdiplus::Color::White) },
    { _T("WhiteSmoke"), Gdiplus::Color(Gdiplus::Color::WhiteSmoke) },
    { _T("Yellow"), Gdiplus::Color(Gdiplus::Color::Yellow) },
 	{ _T("YellowGreen"), Gdiplus::Color(Gdiplus::Color::YellowGreen) }
};

static sword __stdcall TBrushManagerNamedColorsSearchAndSortFunc( ConstPointer ArrayItem, ConstPointer DataItem )
{
	Ptr(TBrushManagerNamedColors) ppa = CastAnyPtr(TBrushManagerNamedColors, CastMutable(Pointer, ArrayItem));
	LPCTSTR ppd = CastAny(LPCTSTR, CastMutable(Pointer, DataItem));

	int res = s_stricmp(ppa->name, ppd);

	if ( res < 0 )
		return -1;
	if ( res > 0 )
		return 1;
	return 0;
}

static sword __stdcall
s_bsearch(Pointer table, dword size, ConstPointer ptr, word max, TSearchAndSortFunc func, sword _mode)
{
	sword ux = 1;
	sword ox = max;
	sword ix = -1;
	sword erg;
	if (PtrCheck(table) || PtrCheck(func) || (size == 0))
		return -1;
	while (ox >= ux)
	{
		ix = ((ox + ux) / 2) - 1;
		erg = func(l_ptradd(table, ((long)size) * ((long)ix)), CastMutable(Pointer, ptr));
		if (erg < 0)
			ux = ix + 2;
		else if (erg > 0)
			ox = ix;
		else if (_mode == UTLPTR_INSERTMODE)
		{
			max--;
			for (; ix < max; ix++)
				if ((erg = func(l_ptradd(table, ((long)size) * ((long)(ix + 1))), CastMutable(Pointer, ptr))) != 0)
					return ix;
			return max;
		}
		else
		{
			for (; ix > 0; ix--)
				if ((erg = func(l_ptradd(table, ((long)size) * ((long)(ix - 1))), CastMutable(Pointer, ptr))) != 0)
					return ix;
			return 0;
		}
	}
	if (ix >= 0)
	{
		switch (_mode)
		{
		case UTLPTR_INSERTMODE:
			if (func(l_ptradd(table, ((long)size) * ((long)ix)), CastMutable(Pointer, ptr)) > 0)
				ix--;
			break;
		case UTLPTR_SEARCHMODE:
			if (func(l_ptradd(table, ((long)size) * ((long)ix)), CastMutable(Pointer, ptr)) >= 0)
				break;
			if (ix < (max - 1))
				ix++;
			break;
		default:
			ix = -1;
			break;
		}							   /* endswitch */
	}
	return ix;
}								   /* end of s_bsearch */

CBrushManager::CBrushManager(void):
	m_brushCache(__FILE__LINE__ 16, 16)
{
}

CBrushManager::CBrushManager(CStringLiteral _prefix):
    m_prefix(__FILE__LINE__ _prefix),
	m_brushCache(__FILE__LINE__ 16, 16)
{
}

CBrushManager::~CBrushManager(void)
{
}

bool CBrushManager::has_Brush(CStringLiteral _key)
{
	if ( _key.IsEmpty() )
		return false;

	TBrushItem item(_key);
	TBrushItems::Iterator it = m_brushCache.FindSorted(&item);

	if (it)
		return true;

	if ( !(m_prefix.IsEmpty()) )
	{
		CStringBuffer vPrefix;

		vPrefix = m_prefix;
		vPrefix.AppendString(_T("User."));
		vPrefix.AppendString(_key);

		if ( theGuiApp->config()->HasValue(vPrefix) )
			return true;

		vPrefix = m_prefix;
		vPrefix.AppendString(_T("."));
		vPrefix.AppendString(_key);

		return (theGuiApp->config()->HasValue(vPrefix));
	}
	return (theGuiApp->config()->HasValue(_key));
}

Gdiplus::Brush* CBrushManager::get_Brush(CStringLiteral _key, ConstRef(Gdiplus::Color) _defaultcolor)
{
	if ( _key.IsEmpty() )
		return nullptr;

	TBrushItem item(_key);
	TBrushItems::Iterator it = m_brushCache.FindSorted(&item);

	if (it)
		return (*it)->brush;

	if ( !(m_prefix.IsEmpty()) )
	{
		CStringBuffer vPrefix;
		CStringBuffer vBrushData;
		Gdiplus::Brush* vBrush;

		vPrefix = m_prefix;
		vPrefix.AppendString(_T("User."));
		vPrefix.AppendString(_key);

		vBrushData = theGuiApp->config()->GetValue(vPrefix);
		vBrush = parse_Brush(vBrushData);

		if ( vBrush )
		{
			item.brush = vBrush;
			item.prefix.addRef();
			m_brushCache.InsertSorted(OK_NEW_OPERATOR TBrushItem(item));
			return item.brush;
		}

		vPrefix = m_prefix;
		vPrefix.AppendString(_T("."));
		vPrefix.AppendString(_key);

		vBrushData = theGuiApp->config()->GetValue(vPrefix);
		vBrush = parse_Brush(vBrushData);

		if ( vBrush )
		{
			item.brush = vBrush;
			item.prefix.addRef();
			m_brushCache.InsertSorted(OK_NEW_OPERATOR TBrushItem(item));
			return item.brush;
		}
	}
	item.brush = new Gdiplus::SolidBrush(_defaultcolor);
	item.prefix.addRef();
	m_brushCache.InsertSorted(OK_NEW_OPERATOR TBrushItem(item));
	return item.brush;
}

void CBrushManager::set_Brush(CStringLiteral _key, ConstRef(Gdiplus::Color) _brushColor)
{
	if ( _key.IsEmpty() )
		return;

	TBrushItem item(_key);
	TBrushItems::Iterator it = m_brushCache.FindSorted(&item);

	if (it)
	{
		if ( (*it)->bPrivateCopy )
		{
			remove_Duplicates((*it)->brush);
			it = m_brushCache.FindSorted(&item);
			if (!it)
				return;
			delete (*it)->brush;
		}
		(*it)->bPrivateCopy = true;
		(*it)->brush = new Gdiplus::SolidBrush(_brushColor);
		return;
	}
	item.brush = new Gdiplus::SolidBrush(_brushColor);
	item.bPrivateCopy = true;
	item.prefix.addRef();
	m_brushCache.InsertSorted(OK_NEW_OPERATOR TBrushItem(item));
}

void CBrushManager::set_Brush(CStringLiteral _key, Gdiplus::Brush* _brush, bool _bPrivateCopy)
{
	if ( _key.IsEmpty() )
		return;

	TBrushItem item(_key);
	TBrushItems::Iterator it = m_brushCache.FindSorted(&item);

	if (it)
	{
		if ( (*it)->bPrivateCopy )
		{
			remove_Duplicates((*it)->brush);
			it = m_brushCache.FindSorted(&item);
			if (!it)
				return;
			delete (*it)->brush;
		}
		(*it)->bPrivateCopy = _bPrivateCopy;
		(*it)->brush = _brush;
		return;
	}
	item.brush = _brush;
	item.bPrivateCopy = _bPrivateCopy;
	item.prefix.addRef();
	m_brushCache.InsertSorted(OK_NEW_OPERATOR TBrushItem(item));
}

void CBrushManager::set_Brush(CStringLiteral _key, ConstRef(CStringBuffer) _text)
{
	if ( _key.IsEmpty() )
		return;
	
	bool bDelete = _text.IsEmpty();
	Gdiplus::Brush* pBrush = nullptr;

	if ( !bDelete )
	{
		pBrush = parse_Brush(_text);
		if ( !pBrush )
			return;
	}
	if ( !(m_prefix.IsEmpty()) )
	{
		CStringBuffer vPrefix;

		vPrefix = m_prefix;
		vPrefix.AppendString(_T("User."));
		vPrefix.AppendString(_key);

		theGuiApp->config()->SetValue(vPrefix, _text);
	}
	else
		theGuiApp->config()->SetValue(_key, _text);

	TBrushItem item(_key);
	TBrushItems::Iterator it = m_brushCache.FindSorted(&item);

	if (it)
	{
		if ( (*it)->bPrivateCopy )
		{
			remove_Duplicates((*it)->brush);
			it = m_brushCache.FindSorted(&item);
			if (!it)
				return;
			if ( !bDelete )
				delete (*it)->brush;
		}
		if ( bDelete )
			m_brushCache.Remove(it);
		else
		{
			(*it)->bPrivateCopy = true;
			(*it)->brush = pBrush;
		}
		return;
	}
	if ( !bDelete )
	{
		item.brush = pBrush;
		item.bPrivateCopy = true;
		item.prefix.addRef();
		m_brushCache.InsertSorted(OK_NEW_OPERATOR TBrushItem(item));
	}
}

void CBrushManager::enumerate_Colors(Ref(CDataVectorT<CStringBuffer>) _colors)
{
	for ( int ix = 0; ix < (sizeof(vBrushManagerNamedColors)/sizeof(TBrushManagerNamedColors)); ++ix )
		_colors.Append(CStringBuffer(__FILE__LINE__ vBrushManagerNamedColors[ix].name));
}

void CBrushManager::remove_Duplicates(Gdiplus::Brush* _brush)
{
	TBrushItem item(_brush);
	TBrushItems::Iterator it = m_brushCache.Find<TBrushItemEqualFunctor>(&item);

	while ( it )
	{
		m_brushCache.Remove(it);
		it = m_brushCache.Find<TBrushItemEqualFunctor>(&item);
	}
}

Gdiplus::Brush* CBrushManager::parse_Brush(ConstRef(CStringBuffer) _text)
{
	if ( _text.IsEmpty() )
		return nullptr;

	sword ix = s_bsearch(
		vBrushManagerNamedColors, 
		sizeof(TBrushManagerNamedColors), 
		_text.GetString(), 
		sizeof(vBrushManagerNamedColors)/sizeof(TBrushManagerNamedColors), 
		TBrushManagerNamedColorsSearchAndSortFunc, 
		UTLPTR_MATCHMODE);

	if ( ix >= 0 )
		return new Gdiplus::SolidBrush(vBrushManagerNamedColors[ix].color);

	CStringBuffer vBuffer(_text);
	CPointer vSplit[8];
	dword vSplitCnt = 0;
	CStringConstIterator sIt;

	vBuffer.Split(_T(","), vSplit, 8, &vSplitCnt);
	if ( vSplitCnt == 1 )
	{
		WLong c;

		sIt = vSplit[0];
		sIt.EatNumber(c);

		return new Gdiplus::SolidBrush(Gdiplus::Color(c));
	}
	if ( vSplitCnt == 3 )
	{
		WLong r;
		WLong g;
		WLong b;

		sIt = vSplit[0];
		sIt.EatWhite();
		sIt.EatNumber(r);
		sIt = vSplit[1];
		sIt.EatWhite();
		sIt.EatNumber(g);
		sIt = vSplit[2];
		sIt.EatWhite();
		sIt.EatNumber(b);

		return new Gdiplus::SolidBrush(Gdiplus::Color(Cast(BYTE, r & 0xFF), Cast(BYTE, g & 0xFF), Cast(BYTE, b & 0xFF)));
	}
	if ( vSplitCnt == 4 )
	{
		WLong a;
		WLong r;
		WLong g;
		WLong b;

		sIt = vSplit[0];
		sIt.EatWhite();
		sIt.EatNumber(a);
		sIt = vSplit[1];
		sIt.EatWhite();
		sIt.EatNumber(r);
		sIt = vSplit[2];
		sIt.EatWhite();
		sIt.EatNumber(g);
		sIt = vSplit[3];
		sIt.EatWhite();
		sIt.EatNumber(b);

		return new Gdiplus::SolidBrush(Gdiplus::Color(Cast(BYTE, a & 0xFF), Cast(BYTE, r & 0xFF), Cast(BYTE, g & 0xFF), Cast(BYTE, b & 0xFF)));
	}
	return nullptr;
}