/******************************************************************************
    
	This file is part of CppSources, which is part of UserLib.

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
/**
 *  \file StringBuffer.h
 *  \brief Reference counted char buffer.
 */
#pragma once

#define MAX_FORMATTEXT 65536L

#include "CppSources.h"

#include <string>
#ifdef OKBUILD_FOR_QT
#include <QtCore/QString>
#endif

class CPPSOURCES_API CByteBuffer;
class CPPSOURCES_API CStringBuffer: public CStringLiteral
{
public:
	CStringBuffer();
	CStringBuffer(DECL_FILE_LINE dword initialsize);
	CStringBuffer(DECL_FILE_LINE mbchar fill, dword initialsize);
	CStringBuffer(DECL_FILE_LINE CConstPointer pText, int lench = -1);
    CStringBuffer(DECL_FILE_LINE CStringConstIterator _str);
    CStringBuffer(DECL_FILE_LINE CStringLiteral _str);
#ifdef QT_VERSION
    CStringBuffer(const QString& _str);
#endif
	CStringBuffer(const CStringBuffer& copy);
	~CStringBuffer(void);

	void operator = (const CStringBuffer& copy);
#ifdef QT_VERSION
	void operator = (const QString& _str);
#endif

	void operator += (mbchar cc);
    void operator += ( CConstPointer pText );
    void operator += ( const CStringConstIterator& _str );
    void operator += ( const CStringLiteral& _str );
#ifdef QT_VERSION
    void operator += ( const QString& _str);
#endif
    void operator += ( const CStringBuffer& copy );

	void Fill(mbchar fill, dword initialsize = 1);
	void SetSize(DECL_FILE_LINE dword reserve);
	void Clear();
	void SetString(DECL_FILE_LINE CConstPointer pText = NULL, int lench = -1);
	void SetString(DECL_FILE_LINE ConstRef(CStringConstIterator) _str);
	void SetString(DECL_FILE_LINE ConstRef(CStringLiteral) _str);
#ifdef QT_VERSION
    void SetString(const QString& str);
#endif
    void SetString(const CStringBuffer& copy);

	void AppendString(CConstPointer pText, int lench = -1);
	void PrependString(CConstPointer pText, int lench = -1);
	void InsertString(dword pos, CConstPointer pText, int lench = -1);
	void DeleteString(dword pos, dword len = 1);
	void ReplaceString(CConstPointer str, CConstPointer pText, int lench = -1);
	sdword FormatString(DECL_FILE_LINE CConstPointer format, ...);
	sdword FormatString(DECL_FILE_LINE CConstPointer format, va_list argList);
	sdword ScanString(CConstPointer format, ...) const;
	sdword ScanString(CConstPointer format, va_list argList) const;
	void SubString(dword startpos, dword chLen, CStringBuffer& result) const;
	void Trim();
	void ToLowerCase();
	void ToUpperCase();
    void SplitAny(CConstPointer delimiter, CArray output, dword maxoutputsize, dword* outputsize);
    void Split(CConstPointer delimiter, CArray output, dword maxoutputsize, dword* outputsize);
	void SplitQuoted(CConstPointer quote, CConstPointer delimiter, CArray output, dword maxoutputsize, dword* outputsize);
	void SplitQuoted(CConstPointer recorddelimiter, CConstPointer fieldquote, CConstPointer fielddelimiter, TSplitFunc output, Pointer outputcontext);

    void convertToBase64(const CByteBuffer& buffer);
    void convertFromBase64(CByteBuffer& buffer) const;
    void convertToByteBuffer(CByteBuffer& buffer) const;
    void convertFromByteBuffer(const CByteBuffer& buffer);
    void convertToUTF8(CByteBuffer& buffer, bool hasBOM = true) const;
    void convertFromUTF8(const CByteBuffer& buffer, bool detectBOM = true, BArray mapping = NULL);
    void convertToUTF16(CByteBuffer& buffer, bool littleEndian = true, bool hasBOM = true) const;
	void convertFromUTF16(const CByteBuffer& buffer, Ptr(bool) littleEndian = NULL, bool detectBOM = true);
    void convertToHex(const CByteBuffer& buffer);
    void convertFromHex(CByteBuffer& buffer) const;
    void convertToStdWString(std::wstring & str) const;
    void convertFromStdWString(const std::wstring & str);
    void convertToStdString(std::string & str) const;
    void convertFromStdString(const std::string & str);
#ifdef QT_VERSION
    void convertToQString(QString & str) const;
    void convertFromQString(const QString & str);
#endif

	__inline void assign(CConstPointer _ptr) { _free_Buffer(); _string = CastMutable(CPointer, _ptr); }
	__inline void release() { _string = NULL; }
	__inline WULong GetAllocSize() const { return (PtrCheck(_string))?0:Castdword(TFsize(_string)); }
	__inline sdword addRef() const  { if ( PtrCheck(_string) ) return -1; return TFincrefcnt(_string); }
	__inline sdword countRef() const { if ( PtrCheck(_string) ) return -1; return TFrefcnt(_string); }
	__inline sdword releaseRef() const { if ( PtrCheck(_string) ) return -1; return TFdecrefcnt(_string); }

	static ConstRef(CStringBuffer) null();

private:
	CPointer _alloc_Buffer(dword size);
	void _alloc_Buffer2(dword size, dword lenCh);
	void _free_Buffer();
#ifdef __DEBUG__
	const char* _file;
	int _line;
#endif
	static CStringBuffer _null;
};

__inline Ref(CStringBuffer) operator << (Ref(CStringBuffer) buf, ConstRef(CStringLiteral) str)
{
	if (NotPtrCheck(str.GetString()))
		buf.AppendString(str);
	return buf;
}

__inline Ref(CStringBuffer) operator << (Ref(CStringBuffer) buf, ConstRef(CByteBuffer) bBuf)
{
	CStringBuffer tmp;

	tmp.convertFromByteBuffer(bBuf);
	buf.AppendString(tmp);
	return buf;
}

__inline Ref(CStringBuffer) operator << (Ref(CStringBuffer) buf, double value)
{
	CStringBuffer tmp;

	tmp.FormatString(__FILE__LINE__ _T("%lf"), value);
	buf.AppendString(tmp);
	return buf;
}

__inline Ref(CStringBuffer) operator << (Ref(CStringBuffer) buf, float value)
{
	CStringBuffer tmp;

	tmp.FormatString(__FILE__LINE__ _T("%f"), value);
	buf.AppendString(tmp);
	return buf;
}

__inline Ref(CStringBuffer) operator << (Ref(CStringBuffer) buf, sqword value)
{
	CStringBuffer tmp;

	tmp.FormatString(__FILE__LINE__ _T("%lld"), value);
	buf.AppendString(tmp);
	return buf;
}

__inline Ref(CStringBuffer) operator << (Ref(CStringBuffer) buf, qword value)
{
	CStringBuffer tmp;

	tmp.FormatString(__FILE__LINE__ _T("%lld"), value);
	buf.AppendString(tmp);
	return buf;
}

__inline Ref(CStringBuffer) operator << (Ref(CStringBuffer) buf, sdword value)
{
	CStringBuffer tmp;

	tmp.FormatString(__FILE__LINE__ _T("%ld"), value);
	buf.AppendString(tmp);
	return buf;
}

__inline Ref(CStringBuffer) operator << (Ref(CStringBuffer) buf, dword value)
{
	CStringBuffer tmp;

	tmp.FormatString(__FILE__LINE__ _T("%ld"), value);
	buf.AppendString(tmp);
	return buf;
}

__inline Ref(CStringBuffer) operator << (Ref(CStringBuffer) buf, sword value)
{
	CStringBuffer tmp;

	tmp.FormatString(__FILE__LINE__ _T("%hd"), value);
	buf.AppendString(tmp);
	return buf;
}

__inline Ref(CStringBuffer) operator << (Ref(CStringBuffer) buf, word value)
{
	CStringBuffer tmp;

	tmp.FormatString(__FILE__LINE__ _T("%hd"), value);
	buf.AppendString(tmp);
	return buf;
}

__inline Ref(CStringBuffer) operator << (Ref(CStringBuffer) buf, WChar value)
{
	CStringBuffer tmp;

	tmp.FormatString(__FILE__LINE__ _T("%c"), value);
	buf.AppendString(tmp);
	return buf;
}

#ifdef UNICODE
__inline Ref(CStringBuffer) operator << (Ref(CStringBuffer) buf, char value)
{
	CStringBuffer tmp;

	tmp.FormatString(__FILE__LINE__ _T("%c"), value);
	buf.AppendString(tmp);
	return buf;
}
#endif
