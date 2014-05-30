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
#include "DataVector.h"

class CODBCConnection;
class CODBCException;
class CODBCEnvironmentImpl;
class WINSOURCES_API CODBCEnvironment : public CCppObject
{
public:
	CODBCEnvironment(void);
	virtual ~CODBCEnvironment(void);

	__inline CODBCEnvironmentImpl* get_Implementation() { return _lpImpl; }

	CODBCException* get_LastError() const;
	void set_LastError(CODBCException* pLastError);

	CODBCConnection* create_Connection();

	void Open();
	void Close();

protected:
	CODBCEnvironmentImpl* _lpImpl;
};

class CODBCStatement;
class CODBCConnectionImpl;
class WINSOURCES_API CODBCConnection : public CCppObject
{
public:
	CODBCConnection(CODBCEnvironment* lpEnv = NULL, CODBCConnectionImpl* lpImpl = NULL);
	virtual ~CODBCConnection(void);

	__inline CODBCEnvironment* get_Environment() { return _lpEnv; }
	__inline CODBCConnectionImpl* get_Implementation() { return _lpImpl; }

	bool can_Transact();
	void close_Transact(bool commit = true);
	void set_AutoCommit(bool val = true);
	bool is_AutoCommit();
	bool is_Connected();

	CODBCStatement* create_Statement();
	void free_Statement(Ref(Ptr(CODBCStatement)) pStmt);

	void Open(CConstPointer _pszConnStr = NULL);
	void Close();

protected:
	CODBCEnvironment* _lpEnv;
	CODBCConnectionImpl* _lpImpl;
};

class WINSOURCES_API CODBCColumn : public CCppObject
{
public:
	enum TDataType
	{
		// http://msdn.microsoft.com/en-us/library/ms710150(v=vs.85).aspx
		eSQL_NULL,
		eSQL_CHAR, // CHAR(n), Character string of fixed string length n.
		eSQL_VARCHAR, // VARCHAR(n), Variable - length character string with a maximum string length n.
		eSQL_LONGVARCHAR, // LONG VARCHAR, Variable length character data. Maximum length is data source–dependent.[9]
		eSQL_WCHAR, // WCHAR(n), Unicode character string of fixed string length n
		eSQL_WVARCHAR, // VARWCHAR(n), Unicode variable - length character string with a maximum string length n
		eSQL_WLONGVARCHAR, // LONGWVARCHAR, Unicode variable - length character data.Maximum length is data source–dependent
		eSQL_DECIMAL, // DECIMAL(p, s), Signed, exact, numeric value with a precision of at least p and scale s. (The maximum precision is driver - defined.) (1 <= p <= 15; s <= p).[4]
		eSQL_NUMERIC, // NUMERIC(p, s), Signed, exact, numeric value with a precision p and scale s(1 <= p <= 15; s <= p).[4]
		eSQL_SMALLINT, // SMALLINT, Exact numeric value with precision 5 and scale 0 (signed: –32, 768 <= n <= 32, 767, unsigned : 0 <= n <= 65, 535)[3].
		eSQL_INTEGER, // INTEGER, Exact numeric value with precision 10 and scale 0 (signed: –2[31] <= n <= 2[31] – 1, unsigned : 0 <= n <= 2[32] – 1)[3].
		eSQL_REAL, // REAL, Signed, approximate, numeric value with a binary precision 24 (zero or absolute value 10[–38] to 10[38]).
		eSQL_FLOAT, // FLOAT(p), Signed, approximate, numeric value with a binary precision of at least p. (The maximum precision is driver - defined.)[5]
		eSQL_DOUBLE, // DOUBLE PRECISION, Signed, approximate, numeric value with a binary precision 53 (zero or absolute value 10[–308] to 10[308]).
		eSQL_BIT, // BIT, Single bit binary data.[8]
		eSQL_TINYINT, // TINYINT, Exact numeric value with precision 3 and scale 0 (signed: –128 <= n <= 127, unsigned : 0 <= n <= 255)[3].
		eSQL_BIGINT, // BIGINT, Exact numeric value with precision 19 (if signed) or 20 (if unsigned) and scale 0 (signed: –2[63] <= n <= 2[63] – 1, unsigned : 0 <= n <= 2[64] – 1)[3], [9].
		eSQL_BINARY, // BINARY(n), Binary data of fixed length n.[9]
		eSQL_VARBINARY, // VARBINARY(n), Variable length binary data of maximum length n.The maximum is set by the user.[9]
		eSQL_LONGVARBINARY, // LONG VARBINARY, Variable length binary data.Maximum length is data source–dependent.[9]
		eSQL_TYPE_DATE, // DATE, Year, month, and day fields, conforming to the rules of the Gregorian calendar. (See Constraints of the Gregorian Calendar, later in this appendix.)
		eSQL_TYPE_TIME, // TIME(p), Hour, minute, and second fields, with valid values for hours of 00 to 23, valid values for minutes of 00 to 59, and valid values for seconds of 00 to 61. Precision p indicates the seconds precision.
		eSQL_TYPE_TIMESTAMP, // TIMESTAMP(p), Year, month, day, hour, minute, and second fields, with valid values as defined for the DATE and TIME data types.
		eSQL_INTERVAL_MONTH, // INTERVAL MONTH(p), Number of months between two dates; p is the interval leading precision.
		eSQL_INTERVAL_YEAR, // INTERVAL YEAR(p), Number of years between two dates; p is the interval leading precision.
		eSQL_INTERVAL_YEAR_TO_MONTH, // INTERVAL YEAR(p) TO MONTH, Number of years and months between two dates; p is the interval leading precision.
		eSQL_INTERVAL_DAY, // INTERVAL DAY(p), Number of days between two dates; p is the interval leading precision.
		eSQL_INTERVAL_HOUR, // INTERVAL HOUR(p), Number of hours between two date / times; p is the interval leading precision.
		eSQL_INTERVAL_MINUTE, // INTERVAL MINUTE(p), Number of minutes between two date / times; p is the interval leading precision.
		eSQL_INTERVAL_SECOND, // INTERVAL SECOND(p, q), Number of seconds between two date / times; p is the interval leading precision and q is the interval seconds precision.
		eSQL_INTERVAL_DAY_TO_HOUR, // INTERVAL DAY(p) TO HOUR, Number of days / hours between two date / times; p is the interval leading precision.
		eSQL_INTERVAL_DAY_TO_MINUTE, // INTERVAL DAY(p) TO MINUTE, Number of days / hours / minutes between two date / times; p is the interval leading precision.
		eSQL_INTERVAL_DAY_TO_SECOND, // INTERVAL DAY(p) TO SECOND(q), Number of days / hours / minutes / seconds between two date / times; p is the interval leading precision and q is the interval seconds precision.
		eSQL_INTERVAL_HOUR_TO_MINUTE, // INTERVAL HOUR(p) TO MINUTE, Number of hours / minutes between two date / times; p is the interval leading precision.
		eSQL_INTERVAL_HOUR_TO_SECOND, // INTERVAL HOUR(p) TO SECOND(q), Number of hours / minutes / seconds between two date / times; p is the interval leading precision and q is the interval seconds precision.
		eSQL_INTERVAL_MINUTE_TO_SECOND, // INTERVAL MINUTE(p) TO SECOND(q), Number of minutes / seconds between two date / times; p is the interval leading precision and q is the interval seconds precision.
		eSQL_GUID // GUID, Fixed length GUID.
	};

	static sqword NullValue;

	CODBCColumn(CConstPointer name = NULL, TDataType dataType = eSQL_NULL, word decp = 0, bool isNull = FALSE, dword dataLen = 0);
	CODBCColumn(ConstRef(CStringBuffer) name, TDataType dataType = eSQL_NULL, word decp = 0, bool isNull = FALSE, dword dataLen = 0);
	virtual ~CODBCColumn();

	__inline TDataType get_DataType() const { return _dataType; }
	CStringBuffer get_DataTypeAsStr() const;
	__inline ConstRef(CByteBuffer) get_Data() const { return _data; }
	__inline void set_Data(ConstRef(CByteBuffer) buf) { _data = buf; }
	__inline ConstRef(CStringBuffer) get_Name() const { return _name; }
	__inline word get_DecimalPoint() const { return _decimalPoint; }
	__inline bool get_Nullable() const { return _nullable; }
#ifdef OK_CPU_64BIT
	__inline SQWPointer get_IndPtr() { return &_ind; }
	__inline sqword get_Ind() { return _ind; }
#endif
#ifdef OK_CPU_32BIT
	__inline SDWPointer get_IndPtr() { return &_ind; }
	__inline sdword get_Ind() { return _ind; }
#endif
	
protected:
	CByteBuffer _data;
	TDataType _dataType;
	CStringBuffer _name;
	word _decimalPoint;
	bool _nullable;
#ifdef OK_CPU_64BIT
	sqword _ind;
#endif
#ifdef OK_CPU_32BIT
	sdword _ind;
#endif
};

class CODBCStatementImpl;
class WINSOURCES_API CODBCStatement : public CCppObject
{
public:
	typedef CDataVectorT<CODBCColumn, CStringByNameLessFunctor<CODBCColumn>> CODBCColumns;

	enum TParamDirection
	{
		eParamInput,
		eParamOutput,
		eParamInOutput
	};

#ifdef OK_CPU_64BIT
	static sqword NullParameter;
	static sqword NullTerminatedParameterString;
#endif
#ifdef OK_CPU_32BIT
	static sdword NullParameter;
	static sdword NullTerminatedParameterString;
#endif

	CODBCStatement(CODBCEnvironment* lpEnv = NULL, CODBCStatementImpl* lpStmtImpl = NULL);
	virtual ~CODBCStatement(void);

	__inline CODBCEnvironment* get_Environment() { return _lpEnv; }
	__inline CODBCStatementImpl* get_Implementation() { return _lpImpl; }
	__inline void reset_Implementation() { _lpImpl = NULL; }

	word get_ColumnCount() const;
	CODBCColumn* get_ColumnInfo(word ix) const;
	CODBCColumn* get_ColumnInfo(LPCTSTR name);

	sqword get_RowCount() const;

	ConstRef(CStringBuffer) get_Command() const;

	void Prepare(CConstPointer pCommand, dword arraysize = 1);
#ifdef OK_CPU_64BIT
	void BindParameter(word no, SDWConstPointer value, SQWConstPointer valueSize = NULL, TParamDirection paramdir = eParamInput);
	void BindParameter(word no, const double* value, SQWConstPointer valueSize = NULL, TParamDirection paramdir = eParamInput);
	void BindParameter(word no, CConstPointer value, qword maxvaluesize, SQWConstPointer valueSize, TParamDirection paramdir = eParamInput);
	void BindParameter(word no, CConstPointer name, SDWConstPointer value, SQWConstPointer valueSize, TParamDirection paramdir = eParamInput);
	void BindParameter(word no, CConstPointer name, const double* value, SQWConstPointer valueSize, TParamDirection paramdir = eParamInput);
	void BindParameter(word no, CConstPointer name, CConstPointer value, qword maxvaluesize, SQWConstPointer valueSize, TParamDirection paramdir = eParamInput);
#endif
#ifdef OK_CPU_32BIT
	void BindParameter(word no, SDWConstPointer value, SDWConstPointer valueSize = NULL, TParamDirection paramdir = eParamInput);
	void BindParameter(word no, const double* value, SDWConstPointer valueSize = NULL, TParamDirection paramdir = eParamInput);
	void BindParameter(word no, CConstPointer value, dword maxvaluesize, SDWConstPointer valueSize, TParamDirection paramdir = eParamInput);
	void BindParameter(word no, CConstPointer name, SDWConstPointer value, SDWConstPointer valueSize, TParamDirection paramdir = eParamInput);
	void BindParameter(word no, CConstPointer name, const double* value, SDWConstPointer valueSize, TParamDirection paramdir = eParamInput);
	void BindParameter(word no, CConstPointer name, CConstPointer value, dword maxvaluesize, SDWConstPointer valueSize, TParamDirection paramdir = eParamInput);
#endif
	void Execute(CConstPointer pCommand = NULL);
	void BindColumns();
	bool Fetch();
	void FetchSingleValue(SDWPointer value);
	void FetchSingleValue(float* value);
	void FetchSingleValue(double* value);
	void FetchSingleValue(CPointer value, qword valueSize);
	void Reset();
	void Close();
	void Free();
	
protected:
	CODBCEnvironment* _lpEnv;
	CODBCStatementImpl* _lpImpl;
	CODBCColumns _columns;
};

class WINSOURCES_API CODBCException : public CCppObject
{
public:
	CODBCException(CConstPointer pText = NULL, int lench = -1);
	CODBCException(ConstRef(CStringBuffer) pText);
	virtual ~CODBCException(void);

	__inline CConstPointer GetString() const { return _text.GetString(); }
	__inline void SetString(CConstPointer pText, int lench = -1) { _text.SetString(__FILE__LINE__ pText, lench); }
	__inline void AppendString(CConstPointer pText, int lench = -1) { _text.AppendString(pText, lench); }
	bool FormatString(CConstPointer format, ...);

#define E_SQL_FATALERROR	-1
#define E_SQL_NORMAL		0
	__inline int get_Severity() { return _severity; }
	__inline void set_Severity(int severity) { _severity = severity; }

protected:
	CStringBuffer _text;
	int _severity;
};

