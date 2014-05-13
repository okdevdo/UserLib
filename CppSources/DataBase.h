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
#pragma once

#include "CppSources.h"
#include "BaseException.h"
#include "FilePath.h"

class CPPSOURCES_API CDBase
{
public:
	CDBase(void);
	~CDBase(void);

	void Init(word fieldcnt, word recordlen);
	void InitField(dword fieldno, CConstPointer fieldname, byte fieldtype, byte fieldlen, byte fielddec);
	void InitIndex(dword indexCnt);
	void InitIndexFields(dword index, dword fieldCnt);
	void InitIndexField(dword index, dword field, dword fieldno, byte fieldlen);
    void Create(const CFilePath& path);

    void Open(const CFilePath& path);
	sqword GetRecordCnt();
	void Read(dword recno);
	void Write(dword recno);
	void Delete(dword recno);
	void Append();
	void Close();

	bool BeginIndex(dword index);
	bool ReadIndex(dword index);
	bool NextIndex(dword index);
	int GetIndexRecno(dword index);

	void BeginBatch();
	void EndBatch();

    void Import(const CFilePath& path, CStringLiteral fielddelim, CStringLiteral recorddelim, bool hasHeader);

	void ClearData();
	bool IsDeleted();
    bool CompareDataEqual();

	int GetFieldNo(CConstPointer fieldname);
    CStringBuffer GetFieldName(dword fno);
	dword GetFieldCnt();
	void GetFieldInfo(dword fieldno, byte* fieldtype, byte* fieldlen, byte* fielddec);

    dword GetIndexCnt();
    dword GetIndexFieldCnt(dword index);
    void GetIndexFieldInfo(dword index, dword indexfield, dword* fieldno, byte* fieldlen);

	Pointer GetFieldRawData(dword fieldno);
	void SetFieldRawData(dword fieldno, Pointer data);

	CStringBuffer GetTextField(dword fieldno); // fType = C, M
    void SetTextField(dword fieldno, const CStringBuffer& value);

	double GetNumericField(dword fieldno); // fType = N, F, I (readOnly)
	void SetNumericField(dword fieldno, double value);

	int GetLogicalField(dword fieldno); // fType = L
	void SetLogicalField(dword fieldno, int value);

	CStringBuffer GetDateField(dword fieldno); // fType = D, Format = YYYYMMDD
    void SetDateField(dword fieldno, const CStringBuffer& value);

	CStringBuffer GetTimeField(dword fieldno); // fType = T, Format = YYYYMMDDHHMMSS
    void SetTimeField(dword fieldno, const CStringBuffer& value);

private:
	Pointer m_dbasefile;
};

DECL_EXCEPTION(CPPSOURCES_API, CDBaseException, CBaseException)
