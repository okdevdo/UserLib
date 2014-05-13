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
#include "CPPS_PCH.H"
#include "DataBase.h"
#include "File.h"
#include "DBASE.H"

IMPL_EXCEPTION(CDBaseException, CBaseException)

CDBase::CDBase(void):
    m_dbasefile(NULL)
{
}

CDBase::~CDBase(void)
{
	Close();
}

void CDBase::Init(word fieldcnt, word recordlen)
{
	m_dbasefile = dbasefile_init(fieldcnt, recordlen);
	if ( PtrCheck(m_dbasefile) )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::Init"), ENOMEM);
}

void CDBase::InitField(dword fieldno, CConstPointer fieldname, byte fieldtype, byte fieldlen, byte fielddec)
{
	if ( PtrCheck(m_dbasefile) )
		return;
	dbasefile_initfield(CastAnyPtr(dbasefile,m_dbasefile), fieldno, fieldname, fieldtype, fieldlen, fielddec);
}

void CDBase::InitIndex(dword indexCnt)
{
	if ( PtrCheck(m_dbasefile) )
		return;

	errno_t result = dbaseindex_init(CastAnyPtr(dbasefile,m_dbasefile), indexCnt);

	if ( result )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::InitIndex"), result);
}

void CDBase::InitIndexFields(dword index, dword fieldCnt)
{
	if ( PtrCheck(m_dbasefile) )
		return;

	errno_t result = dbaseindex_initfields(CastAnyPtr(dbasefile,m_dbasefile), index, fieldCnt);

	if ( result )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::InitIndexFields"), result);
}

void CDBase::InitIndexField(dword index, dword field, dword fieldno, byte fieldlen)
{
	if ( PtrCheck(m_dbasefile) )
		return;

	dbaseindex_initfield(CastAnyPtr(dbasefile,m_dbasefile), index, field, fieldno, fieldlen);
}

void CDBase::Create(ConstRef(CFilePath) path)
{
	if ( PtrCheck(m_dbasefile) )
		return;

	errno_t result = dbasefile_create(path.get_Path().GetString(), CastAnyPtr(dbasefile,m_dbasefile));

	if ( result )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException, path = '%s'"), _T("CDBase::Create"), path.get_Path().GetString(), result);
	if ( dbasefile_hasmemo(CastAnyPtr(dbasefile,m_dbasefile)) )
	{
		CFilePath path2(path);

        path2.set_Extension(_T("dbt"));

		result = dbasememo_create(path2.get_Path().GetString(), CastAnyPtr(dbasefile,m_dbasefile));

		if ( result )
			throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException, path = '%s'"), _T("CDBase::Create"), path2.get_Path().GetString(), result);
	}
	if ( dbasefile_hasindex(CastAnyPtr(dbasefile,m_dbasefile)) )
	{
		CFilePath path2(path);

        path2.set_Extension(_T("mdx"));

		result = dbaseindex_create(path2.get_Path().GetString(), CastAnyPtr(dbasefile,m_dbasefile));

		if ( result )
			throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException, path = '%s'"), _T("CDBase::Create"), path2.get_Path().GetString(), result);
	}
}

void CDBase::Open(ConstRef(CFilePath) path)
{
	errno_t result = dbasefile_open(path.get_Path().GetString(), CastAnyPtr(dbasefile*,&m_dbasefile));

	if ( result )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException, path = '%s'"), _T("CDBase::Open"), path.get_Path().GetString(), result);
	if ( dbasefile_hasmemo(CastAnyPtr(dbasefile,m_dbasefile)) )
	{
		CFilePath path2(path);

        path2.set_Extension(_T("dbt"));

		result = dbasememo_open(path2.get_Path().GetString(), CastAnyPtr(dbasefile,m_dbasefile));

		if ( result )
			throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException, path = '%s'"), _T("CDBase::Open"), path2.get_Path().GetString(), result);
	}
	if ( dbasefile_hasindex(CastAnyPtr(dbasefile,m_dbasefile)) )
	{
		CFilePath path2(path);

        path2.set_Extension(_T("mdx"));

		result = dbaseindex_open(path2.get_Path().GetString(), CastAnyPtr(dbasefile,m_dbasefile));

		if ( result )
			throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException, path = '%s'"), _T("CDBase::Open"), path2.get_Path().GetString(), result);
	}
}

void CDBase::Read(dword recno)
{
	if ( PtrCheck(m_dbasefile) )
		return;

	errno_t result = dbasefile_read(CastAnyPtr(dbasefile,m_dbasefile), recno);

	if ( result )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::Read"), result);
}

void CDBase::Write(dword recno)
{
	if ( PtrCheck(m_dbasefile) )
		return;

	errno_t result = dbasefile_write(CastAnyPtr(dbasefile,m_dbasefile), recno);

	if ( result )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::Write"), result);

    if ( dbasefile_hasindex(CastAnyPtr(dbasefile,m_dbasefile)) )
    {
        dword indexCnt = dbaseindex_getindexcnt(CastAnyPtr(dbasefile,m_dbasefile));
        dword fieldCnt = dbasefile_getfieldcnt(CastAnyPtr(dbasefile,m_dbasefile));

        for ( dword ix = 0; ix < indexCnt; ++ix )
        {
            dword fcnt = dbaseindex_getindexfieldcnt(CastAnyPtr(dbasefile,m_dbasefile), ix);
            bool bChanged = false;

            for ( dword fno = 0; (fno < fcnt) && (!bChanged); ++fno )
            {
                dword fieldno;
                byte fieldlen;

                dbaseindex_getindexfieldinfo(CastAnyPtr(dbasefile,m_dbasefile), ix, fno, &fieldno, &fieldlen);
                if ( fieldno >= fieldCnt )
                   throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::Write"), EINVAL);
                bChanged = (!(dbasefile_cmpfielddata(CastAnyPtr(dbasefile,m_dbasefile), fieldno)));
            }
            if ( bChanged )
            {
                result = dbaseindex_removekey(CastAnyPtr(dbasefile,m_dbasefile), ix, recno);

                if ( result )
                    throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::Write"), result);

                result = dbaseindex_insertkey(CastAnyPtr(dbasefile,m_dbasefile), ix, recno);

                if ( result )
                    throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::Write"), result);
            }
        }
    }
}

void CDBase::Delete(dword recno)
{
	if ( PtrCheck(m_dbasefile) )
		return;

	errno_t result = dbasefile_delete(CastAnyPtr(dbasefile,m_dbasefile), recno);

	if ( result )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::Delete"), result);

	if ( dbasefile_hasindex(CastAnyPtr(dbasefile,m_dbasefile)) )
	{
        dword indexCnt = dbaseindex_getindexcnt(CastAnyPtr(dbasefile,m_dbasefile));

        for ( dword ix = 0; ix < indexCnt; ++ix )
		{
			result = dbaseindex_removekey(CastAnyPtr(dbasefile,m_dbasefile), ix, recno);

			if ( result )
				throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::Delete"), result);
		}
	}
}

void CDBase::Append()
{
	if ( PtrCheck(m_dbasefile) )
		return;

	errno_t result = dbasefile_append(CastAnyPtr(dbasefile,m_dbasefile));

	if ( result )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::Append"), result);

	if ( dbasefile_hasindex(CastAnyPtr(dbasefile,m_dbasefile)) )
	{
        sqword recno = dbasefile_getrecordcnt(CastAnyPtr(dbasefile,m_dbasefile)) - 1;
        dword indexCnt = dbaseindex_getindexcnt(CastAnyPtr(dbasefile,m_dbasefile));

        for ( dword ix = 0; ix < indexCnt; ++ix )
		{
			result = dbaseindex_insertkey(CastAnyPtr(dbasefile,m_dbasefile), ix, recno);

			if ( result )
				throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::Append"), result);
		}
	}
}

void CDBase::Close()
{
	if ( PtrCheck(m_dbasefile) )
		return;

	errno_t result = dbasefile_close(CastAnyPtr(dbasefile,m_dbasefile));

	if ( result )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::Close"), result);
	m_dbasefile = NULL;
}

bool CDBase::BeginIndex(dword index)
{
    errno_t result;
    sqword recno;
    
	if ( PtrCheck(m_dbasefile) )
		return false;

	result = dbaseindex_getFirstRecno(CastAnyPtr(dbasefile,m_dbasefile), index, &recno);

    if ( result )
        throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::BeginIndex"), result);
    
	if ( recno < 0LL )
		return false;
	Read(Cast(dword,recno));
	return true;
}

bool CDBase::ReadIndex(dword index)
{
    errno_t result;
    sqword recno;
    
	if ( PtrCheck(m_dbasefile) )
        return false;

	result = dbaseindex_getSearchedRecno(CastAnyPtr(dbasefile,m_dbasefile), index, &recno);

    if ( result )
        throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::ReadIndex"), result);
    
	if ( recno < 0LL )
		return false;
	Read(Cast(dword,recno));
	return true;
}

bool CDBase::NextIndex(dword index)
{
    errno_t result;
    sqword recno;
    
	if ( PtrCheck(m_dbasefile) )
		return false;

	result = dbaseindex_getNextRecno(CastAnyPtr(dbasefile,m_dbasefile), index, &recno);

    if ( result )
        throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::NextIndex"), result);

    if ( recno < 0LL )
		return false;
	Read(Cast(dword,recno));
	return true;
}

int CDBase::GetIndexRecno(dword index)
{
    errno_t result;
    sqword recno;
    
	if ( PtrCheck(m_dbasefile) )
		return -1;

	result = dbaseindex_getRecno(CastAnyPtr(dbasefile,m_dbasefile), index, &recno);

    if ( result )
        throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::GetIndexRecno"), result);
    
	return Cast(int,recno);
}

void CDBase::BeginBatch()
{
	if ( PtrCheck(m_dbasefile) )
		return;
	dbasefile_beginbatch(CastAnyPtr(dbasefile,m_dbasefile));
}

void CDBase::EndBatch()
{
	if ( PtrCheck(m_dbasefile) )
		return;

	errno_t result = dbasefile_endbatch(CastAnyPtr(dbasefile,m_dbasefile));

	if ( result )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::EndBatch"), result);
}

void CDBase::Import(ConstRef(CFilePath) path, CStringLiteral fielddelim, CStringLiteral recorddelim, bool hasHeader)
{
	CDiskFile file;
	CByteBuffer buf;
	CStringBuffer sbuf;
	CStringBuffer sbufLine;
	CStringBuffer sbufField;
	CStringConstIterator it;
	sqword fLength;
	dword fieldCount;
	dword fC;
	dword maxfieldCount = 0;
	dword fieldDelimLen;
	dword recordCount;
	dword rC;
	dword maxrecordCount;
	dword recordDelimLen;
	CArray recordsplit;
	CArray fieldsplit = NULL;
	dword ix;
	dword jx;
	byte fieldtype; 
	byte fieldlen; 
	byte fielddec;
	CStringBuffer tmp;
	CStringBuffer datefieldvalue;
	double doublefieldvalue;
	int logicalfieldvalue;

	if ( PtrCheck(m_dbasefile) )
		return;
	try
	{
		file.Open(path);
		fLength = file.GetSize();
		buf.set_BufferSize(__FILE__LINE__ Cast(dword, fLength));
		file.Read(buf);
		file.Close();
	}
	catch ( CFileException* ex )
	{
		file.Close();
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDiskFileException (%s)"), _T("CDBase::Import"), ex->GetExceptionMessage().GetString(), errno);
	}
	sbuf.convertFromUTF8(buf);

	maxrecordCount = 0;
	recordDelimLen = recorddelim.GetLength();
	it = sbuf.Begin();
	it.Find(recorddelim);
	while ( !(it.IsEnd()) )
	{
		++maxrecordCount;
		it += recordDelimLen;
		it.Find(recorddelim);
	}
	++maxrecordCount;
	recordsplit = CastAny(CArray, TFalloc(maxrecordCount * szPointer));
	sbuf.Split(recorddelim, recordsplit, maxrecordCount, &rC);
	recordCount = Cast(dword, rC);
	BeginBatch();
	for ( ix = 0; ix < recordCount; ++ix )
	{
		if ( ix == 0 )
		{
			sbufLine.SetString(__FILE__LINE__ recordsplit[0]);
			fieldDelimLen = fielddelim.GetLength();
			maxfieldCount = 0;
			it = sbufLine.Begin();
			it.Find(fielddelim);
			while ( !(it.IsEnd()) )
			{
				++maxfieldCount;
				it += fieldDelimLen;
				it.Find(fielddelim);
			}
			++maxfieldCount;
			fieldsplit = CastAny(CArray, TFalloc(maxfieldCount * szPointer));
			sbufLine.Split(fielddelim, fieldsplit, maxfieldCount, &fC);
			fieldCount = Cast(dword, fC);
		}
		else
		{
			sbufLine.SetString(__FILE__LINE__ recordsplit[ix]);
			sbufLine.Split(fielddelim, fieldsplit, maxfieldCount, &fC);
			fieldCount = Cast(dword, fC);

			try
			{
				ClearData();
				for ( jx = 0; jx < fieldCount; ++jx )
				{
					sbufField.SetString(__FILE__LINE__ fieldsplit[jx]);
					GetFieldInfo(jx, &fieldtype, &fieldlen, &fielddec);
					switch ( fieldtype )
					{
					case 'C':
						SetTextField(jx, sbufField);
						break;
					case 'N':
						doublefieldvalue = s_strtod(sbufField, NULL);
						SetNumericField(jx, doublefieldvalue);
						break;
					case 'L':
						logicalfieldvalue = s_atoi(sbufField);
						SetLogicalField(jx, logicalfieldvalue);
						break;
					case 'D':
						datefieldvalue.Clear();
						sbufField.SubString(0, 4, tmp);
						datefieldvalue += tmp;
						sbufField.SubString(5, 2, tmp);
						datefieldvalue += tmp;
						sbufField.SubString(8, 2, tmp);
						datefieldvalue += tmp;
						SetDateField(jx, datefieldvalue);
						break;
					default:
						break;
					}
				}
				Append();
			}
			catch ( CDBaseException* )
			{
			}
		}
	}
	TFfree(recordsplit);
	TFfree(fieldsplit);
	EndBatch();
}

sqword CDBase::GetRecordCnt()
{
	if ( PtrCheck(m_dbasefile) )
		return 0;
	return dbasefile_getrecordcnt(CastAnyPtr(dbasefile,m_dbasefile));
}

dword CDBase::GetFieldCnt()
{
	if ( PtrCheck(m_dbasefile) )
		return 0;
	return dbasefile_getfieldcnt(CastAnyPtr(dbasefile,m_dbasefile));
}

int CDBase::GetFieldNo(CConstPointer fieldname)
{
	if ( PtrCheck(m_dbasefile) )
		return -1;
	return dbasefile_getfieldno(CastAnyPtr(dbasefile,m_dbasefile), fieldname);
}

CStringBuffer CDBase::GetFieldName(dword fno)
{
    if ( PtrCheck(m_dbasefile) )
        return CStringBuffer();

    CPointer fn = dbasefile_getfieldname(CastAnyPtr(dbasefile,m_dbasefile), fno);

    if ( PtrCheck(fn) )
        return CStringBuffer();

    CStringBuffer result(__FILE__LINE__ fn);

    TFfree(fn);
    return result;
}

void CDBase::GetFieldInfo(dword fieldno, byte* fieldtype, byte* fieldlen, byte* fielddec)
{
	if ( PtrCheck(m_dbasefile) )
		return;
	dbasefile_getfieldinfo(CastAnyPtr(dbasefile,m_dbasefile), fieldno, fieldtype, fieldlen, fielddec);
}

dword CDBase::GetIndexCnt()
{
    if ( PtrCheck(m_dbasefile) )
        return 0;
    return dbaseindex_getindexcnt(CastAnyPtr(dbasefile,m_dbasefile));
}

dword CDBase::GetIndexFieldCnt(dword index)
{
    if ( PtrCheck(m_dbasefile) )
        return 0;
    return dbaseindex_getindexfieldcnt(CastAnyPtr(dbasefile,m_dbasefile), index);
}

void CDBase::GetIndexFieldInfo(dword index, dword indexfield, dword* fieldno, byte* fieldlen)
{
    if ( PtrCheck(m_dbasefile) )
        return;
    dbaseindex_getindexfieldinfo(CastAnyPtr(dbasefile,m_dbasefile), index, indexfield, fieldno, fieldlen);
}

void CDBase::ClearData()
{
	if ( PtrCheck(m_dbasefile) )
		return;
	dbasefile_cleardata(CastAnyPtr(dbasefile,m_dbasefile));
}

bool CDBase::IsDeleted()
{
	if ( PtrCheck(m_dbasefile) )
		return false;
	return dbasefile_isdeleted(CastAnyPtr(dbasefile,m_dbasefile));
}

bool CDBase::CompareDataEqual()
{
    if ( PtrCheck(m_dbasefile) )
        return false;

    dword fcnt = dbasefile_getfieldcnt(CastAnyPtr(dbasefile,m_dbasefile));

    for ( dword fno = 0; fno < fcnt; ++fno )
        if ( !(dbasefile_cmpfielddata(CastAnyPtr(dbasefile,m_dbasefile), fno)) )
            return false;
    return true;
}

Pointer CDBase::GetFieldRawData(dword fieldno)
{
	if ( PtrCheck(m_dbasefile) )
		return NULL;
	return dbasefile_getfielddata(CastAnyPtr(dbasefile,m_dbasefile), fieldno);
}

void CDBase::SetFieldRawData(dword fieldno, Pointer data)
{
	if ( PtrCheck(m_dbasefile) )
		return;
	dbasefile_setfielddata(CastAnyPtr(dbasefile,m_dbasefile), fieldno, data);
}

CStringBuffer CDBase::GetTextField(dword fieldno)
{
	CStringBuffer result;

	if ( PtrCheck(m_dbasefile) )
		return result;

	byte fieldtype; 
	byte fieldlen; 
	byte fielddec;

	dbasefile_getfieldinfo(CastAnyPtr(dbasefile,m_dbasefile), fieldno, &fieldtype, &fieldlen, &fielddec);

	if ( (fieldtype != 'C') && (fieldtype != 'M') )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::GetTextField"), EINVAL);
	if ( fieldtype == 'C' )
	{
		result.FormatString(__FILE__LINE__ _T("%*.*hs"), fieldlen, fieldlen, dbasefile_getfielddata(CastAnyPtr(dbasefile,m_dbasefile), fieldno));
		result.Trim();

		return result;
	}
	if ( fieldtype == 'M' )
	{
		CByteBuffer data(__FILE__LINE__ CastAny(BPointer, dbasefile_getfielddata(CastAnyPtr(dbasefile,m_dbasefile), fieldno)), fieldlen);
		CStringBuffer value;
		BConstPointer it;
		dword memofilepos;
		errno_t err;

		value.convertFromByteBuffer(data);
		if ( value.ScanString(_T("%ld"), &memofilepos) <= 0 )
			return result;
		if ( !memofilepos )
			return result;

		data.set_BufferSize(__FILE__LINE__ DBASEMEMO_CHUNKSIZE);

		while ( true )
		{
			err = dbasememo_readchunk(CastAnyPtr(dbasefile,m_dbasefile), memofilepos);
			if ( err )
				throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::GetTextField"), err);
			s_memcpy_s(CastMutable(BPointer, data.get_Buffer()), DBASEMEMO_CHUNKSIZE, dbasememo_getdata(CastAnyPtr(dbasefile,m_dbasefile)), DBASEMEMO_CHUNKSIZE);
			it = CastAny(BConstPointer, s_memchr(data.get_Buffer(), 0x1A, DBASEMEMO_CHUNKSIZE));
			if ( NotPtrCheck(it) )
			{
				data.set_BufferSize(__FILE__LINE__ Castdword(it - data.get_Buffer()));
				value.convertFromByteBuffer(data);
				result += value;
				break;
			}
			value.convertFromByteBuffer(data);
			result += value;
			++memofilepos;
		}
	}
	return result;
}

void CDBase::SetTextField(dword fieldno, ConstRef(CStringBuffer) value)
{
	if ( PtrCheck(m_dbasefile) )
		return;

	byte fieldtype; 
	byte fieldlen; 
	byte fielddec;

	dbasefile_getfieldinfo(CastAnyPtr(dbasefile,m_dbasefile), fieldno, &fieldtype, &fieldlen, &fielddec);

	if ( (fieldtype != 'C') && (fieldtype != 'M') )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::SetTextField"), EINVAL);

	CByteBuffer data;
	dword bufsize;
	BPointer p;

	value.convertToByteBuffer(data);
	if ( fieldtype == 'C' )
	{
		if ( data.get_BufferSize() < fieldlen )
		{
			bufsize = data.get_BufferSize();
			data.set_BufferSize(__FILE__LINE__ fieldlen);
			p = data.get_Buffer() + bufsize;
			while ( bufsize < fieldlen )
			{
				*p++ = 0x20;
				++bufsize;
			}
		}
		dbasefile_setfielddata(CastAnyPtr(dbasefile,m_dbasefile), fieldno, data.get_Buffer());
		return;
	}
	if ( fieldtype == 'M' )
	{
		CStringBuffer recnocnv;
		dword bufsize1;
		bool bFirst;
		sqword recno;
		sqword recnofirst = 0;
		errno_t err;

		bufsize = data.get_BufferSize();
		bufsize1 = bufsize + 1;
		bufsize1 = ((bufsize1 + DBASEMEMO_CHUNKSIZE - 1) / DBASEMEMO_CHUNKSIZE) * DBASEMEMO_CHUNKSIZE;
		data.set_BufferSize(__FILE__LINE__ bufsize1);
		p = data.get_Buffer() + bufsize;
		*p++ = 0x1A;
		++bufsize;
		while ( bufsize < bufsize1 )
		{
			*p++ = 0x20;
			++bufsize;
		}
		p = data.get_Buffer();
		bufsize = data.get_BufferSize();
		bFirst = true;
		while ( bufsize > 0 )
		{
			dbasememo_setdata(CastAnyPtr(dbasefile,m_dbasefile), p);
			err = dbasememo_appendchunk(CastAnyPtr(dbasefile,m_dbasefile), &recno);
			if ( err )
				throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::SetTextField"), err);
			if ( bFirst )
			{
				recnofirst = recno;
				bFirst = false;
			}
			p += DBASEMEMO_CHUNKSIZE;
			bufsize -= DBASEMEMO_CHUNKSIZE;
		}
		recnocnv.FormatString(__FILE__LINE__ _T("%*.*ld"), fieldlen, fieldlen, recnofirst);
		recnocnv.convertToByteBuffer(data);
		dbasefile_setfielddata(CastAnyPtr(dbasefile,m_dbasefile), fieldno, data.get_Buffer());
	}
}

double CDBase::GetNumericField(dword fieldno)
{
	if ( PtrCheck(m_dbasefile) )
		return 0.0L;

	byte fieldtype; 
	byte fieldlen; 
	byte fielddec;

	dbasefile_getfieldinfo(CastAnyPtr(dbasefile,m_dbasefile), fieldno, &fieldtype, &fieldlen, &fielddec);

	if ( (fieldtype != 'N') && (fieldtype != 'F') && (fieldtype != 'I') )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::GetNumericField"), EINVAL);

	CByteBuffer data(__FILE__LINE__ CastAny(BPointer, dbasefile_getfielddata(CastAnyPtr(dbasefile,m_dbasefile), fieldno)), fieldlen);
	CStringBuffer value;
	double result;

	value.convertFromByteBuffer(data);
	value.Trim();
	if ( value.IsEmpty() )
		return 0.0L;
	value.ScanString(_T("%lf"), &result);
	return result;
}

void CDBase::SetNumericField(dword fieldno, double value)
{
	if ( PtrCheck(m_dbasefile) )
		return;

	byte fieldtype; 
	byte fieldlen; 
	byte fielddec;

	dbasefile_getfieldinfo(CastAnyPtr(dbasefile,m_dbasefile), fieldno, &fieldtype, &fieldlen, &fielddec);

	if ( (fieldtype != 'N') && (fieldtype != 'F') )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::SetNumericField"), EINVAL);

	CStringBuffer data;
	CByteBuffer byteData;

	if ( data.FormatString(__FILE__LINE__ _T("%*.*lf"), fieldlen, fielddec, value) )
	{
		data.convertToByteBuffer(byteData);

		dbasefile_setfielddata(CastAnyPtr(dbasefile,m_dbasefile), fieldno, byteData.get_Buffer());
	}
}

int CDBase::GetLogicalField(dword fieldno)
{
	if ( PtrCheck(m_dbasefile) )
		return -1;

	byte fieldtype; 
	byte fieldlen; 
	byte fielddec;

	dbasefile_getfieldinfo(CastAnyPtr(dbasefile,m_dbasefile), fieldno, &fieldtype, &fieldlen, &fielddec);

	if ( (fieldtype != 'L') || (fieldlen != 1) )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::GetLogicalField"), EINVAL);

	byte* data = CastAnyPtr(byte, dbasefile_getfielddata(CastAnyPtr(dbasefile,m_dbasefile), fieldno));

	if ( (*data == 'T') || (*data == 't') ||  (*data == 'J') || (*data == 'j') )
		return 1;
	if ( (*data == 'F') || (*data == 'f') ||  (*data == 'N') || (*data == 'n') )
		return 0;
	return -1;
}

void CDBase::SetLogicalField(dword fieldno, int value)
{
	if ( PtrCheck(m_dbasefile) )
		return;

	byte fieldtype; 
	byte fieldlen; 
	byte fielddec;

	dbasefile_getfieldinfo(CastAnyPtr(dbasefile,m_dbasefile), fieldno, &fieldtype, &fieldlen, &fielddec);

	if ( (fieldtype != 'L') || (fieldlen != 1) )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::SetLogicalField"), EINVAL);

	byte bValue;

	if ( value == 1 )
		bValue = 'T';
	else if ( value == 0 )
		bValue = 'F';
	else
		bValue = '?';

	dbasefile_setfielddata(CastAnyPtr(dbasefile,m_dbasefile), fieldno, &bValue);
}

CStringBuffer CDBase::GetDateField(dword fieldno)
{
	CStringBuffer result;

	if ( PtrCheck(m_dbasefile) )
		return result;

	byte fieldtype; 
	byte fieldlen; 
	byte fielddec;

	dbasefile_getfieldinfo(CastAnyPtr(dbasefile,m_dbasefile), fieldno, &fieldtype, &fieldlen, &fielddec);

	if ( (fieldtype != 'D') || (fieldlen != 8) )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::GetDateField"), EINVAL);

	result.FormatString(__FILE__LINE__ _T("%*.*hs"), fieldlen, fieldlen, dbasefile_getfielddata(CastAnyPtr(dbasefile,m_dbasefile), fieldno));
	result.Trim();

	return result;
}

void CDBase::SetDateField(dword fieldno, ConstRef(CStringBuffer) value)
{
	if ( PtrCheck(m_dbasefile) )
		return;

	byte fieldtype; 
	byte fieldlen; 
	byte fielddec;

	dbasefile_getfieldinfo(CastAnyPtr(dbasefile,m_dbasefile), fieldno, &fieldtype, &fieldlen, &fielddec);

	if ( (fieldtype != 'D') || (fieldlen != 8) || (value.GetLength() != 8) )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::SetDateField"), EINVAL);

	CByteBuffer data;

	value.convertToByteBuffer(data);
	dbasefile_setfielddata(CastAnyPtr(dbasefile,m_dbasefile), fieldno, data.get_Buffer());
}

CStringBuffer CDBase::GetTimeField(dword fieldno)
{
	CStringBuffer result;

	if ( PtrCheck(m_dbasefile) )
		return result;

	byte fieldtype; 
	byte fieldlen; 
	byte fielddec;

	dbasefile_getfieldinfo(CastAnyPtr(dbasefile,m_dbasefile), fieldno, &fieldtype, &fieldlen, &fielddec);

	if ( (fieldtype != 'T') || (fieldlen != 14) )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::GetTimeField"), EINVAL);

	result.FormatString(__FILE__LINE__ _T("%*.*hs"), fieldlen, fieldlen, dbasefile_getfielddata(CastAnyPtr(dbasefile,m_dbasefile), fieldno));
	result.Trim();

	return result;
}

void CDBase::SetTimeField(dword fieldno, ConstRef(CStringBuffer) value)
{
	if ( PtrCheck(m_dbasefile) )
		return;

	byte fieldtype; 
	byte fieldlen; 
	byte fielddec;

	dbasefile_getfieldinfo(CastAnyPtr(dbasefile,m_dbasefile), fieldno, &fieldtype, &fieldlen, &fielddec);

	if ( (fieldtype != 'T') || (fieldlen != 14) || (value.GetLength() != 14) )
		throw OK_NEW_OPERATOR CDBaseException(__FILE__LINE__ _T("in %s CDBaseException"), _T("CDBase::SetTimeField"), EINVAL);

	CByteBuffer data;

	value.convertToByteBuffer(data);
	dbasefile_setfielddata(CastAnyPtr(dbasefile,m_dbasefile), fieldno, data.get_Buffer());
}
