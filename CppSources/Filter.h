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
#include "File.h"
#include "BaseException.h"
#include "DataVector.h"

class CPPSOURCES_API CAbstractFilterInput
{
public:
	CAbstractFilterInput();
	virtual ~CAbstractFilterInput();

	virtual void open() = 0;
	virtual CFile::TFileSize size() = 0;
	virtual void read(Ref(CByteBuffer) inputbuf) = 0;
	virtual void close() = 0;
};

class CPPSOURCES_API CAbstractFilterOutput
{
public:
	CAbstractFilterOutput();
	virtual ~CAbstractFilterOutput();

	virtual void open() = 0;
	virtual void write(Ref(CByteBuffer) outputbuf) = 0;
	virtual void close() = 0;
};

class CPPSOURCES_API CFilterInput: public CCppObject, public CAbstractFilterInput
{
public:
	CFilterInput();
	virtual ~CFilterInput();

	virtual void open();
	virtual CFile::TFileSize size();
	virtual void read(Ref(CByteBuffer) inputbuf);
	virtual void close();
};

class CPPSOURCES_API CFilterOutput: public CCppObject, public CAbstractFilterOutput
{
public:
	CFilterOutput();
	virtual ~CFilterOutput();

	virtual void open();
	virtual void write(Ref(CByteBuffer) outputbuf);
	virtual void close();
};

class CPPSOURCES_API CStringVectorFilterInput : public CFilterInput
{
public:
	CStringVectorFilterInput(ConstRef(CDataVectorT<CStringBuffer>) input);
	virtual ~CStringVectorFilterInput();

	virtual void open();
	virtual CFile::TFileSize size();
	virtual void read(Ref(CByteBuffer) inputbuf);
	virtual void close();

protected:
	ConstRef(CDataVectorT<CStringBuffer>) m_input;
	CDataVectorT<CStringBuffer>::Iterator m_inputIt;

private:
	CStringVectorFilterInput();
};

class CPPSOURCES_API CFileFilterInput : public CFilterInput
{
public:
	CFileFilterInput(ConstRef(CFilePath) fname);
	CFileFilterInput(Ptr(CFile) ffile);
	virtual ~CFileFilterInput();

	virtual void open();
	virtual CFile::TFileSize size();
	virtual void read(Ref(CByteBuffer) inputbuf);
	virtual void close();

protected:
	Ptr(CFile) m_inputFile;
	WBool m_canReOpen;
	CFilePath m_fileName;

private:
	CFileFilterInput();
};

class CPPSOURCES_API CStdInFilterInput : public CFilterInput
{
public:
	CStdInFilterInput();
	virtual ~CStdInFilterInput();

	virtual void open();
	virtual CFile::TFileSize size();
	virtual void read(Ref(CByteBuffer) inputbuf);
	virtual void close();
};

class CPPSOURCES_API CStringVectorFilterOutput : public CFilterOutput
{
public:
	CStringVectorFilterOutput(ConstRef(CDataVectorT<CStringBuffer>) output);
	virtual ~CStringVectorFilterOutput();

	virtual void open();
	virtual void write(Ref(CByteBuffer) outputbuf);
	virtual void close();

protected:
	ConstRef(CDataVectorT<CStringBuffer>) m_output;

private:
	CStringVectorFilterOutput();
};

class CPPSOURCES_API CSortedStringVectorFilterOutput : public CFilterOutput
{
public:
	CSortedStringVectorFilterOutput(ConstRef(CDataVectorT<CStringBuffer>) output);
	virtual ~CSortedStringVectorFilterOutput();

	virtual void open();
	virtual void write(Ref(CByteBuffer) outputbuf);
	virtual void close();

protected:
	ConstRef(CDataVectorT<CStringBuffer>) m_output;

private:
	CSortedStringVectorFilterOutput();
};

class CPPSOURCES_API CFileFilterOutput : public CFilterOutput
{
public:
	CFileFilterOutput(ConstRef(CFilePath) fname);
	CFileFilterOutput(Ptr(CFile) ffile);
	virtual ~CFileFilterOutput();

	virtual void open();
	virtual void write(Ref(CByteBuffer) outputbuf);
	virtual void close();

protected:
	Ptr(CFile) m_outputFile;
	WBool m_canReOpen;
	CFilePath m_fileName;

private:
	CFileFilterOutput();
};

class CPPSOURCES_API CStdOutFilterOutput : public CFilterOutput
{
public:
	CStdOutFilterOutput(bool bNewLine = false);
	virtual ~CStdOutFilterOutput();

	virtual void open();
	virtual void write(Ref(CByteBuffer) outputbuf);
	virtual void close();

protected:
	bool m_bNewLine;
};

class CPPSOURCES_API CStdErrFilterOutput : public CFilterOutput
{
public:
	CStdErrFilterOutput(bool bNewLine = false);
	virtual ~CStdErrFilterOutput();

	virtual void open();
	virtual void write(Ref(CByteBuffer) outputbuf);
	virtual void close();

protected:
	bool m_bNewLine;
};

class CPPSOURCES_API CNullFilterOutput : public CFilterOutput
{
public:
	CNullFilterOutput();
	virtual ~CNullFilterOutput();

	virtual void open();
	virtual void write(Ref(CByteBuffer) outputbuf);
	virtual void close();
};

class CPPSOURCES_API CFilter: public CCppObject
{
public:
	CFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	virtual ~CFilter(void);

	virtual void open();
	virtual dword do_filter() = 0;
	virtual void close();

	virtual void read(Ref(CByteBuffer) inputbuf);
	virtual void write(Ref(CByteBuffer) outputbuf);

protected:
	Ptr(CAbstractFilterInput) m_input;
	Ptr(CAbstractFilterOutput) m_output;

private:
	CFilter(void);
};

class CPPSOURCES_API CCopyFilter: public CFilter
{
public:
	CCopyFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	virtual ~CCopyFilter(void);

	virtual dword do_filter();

private:
	CCopyFilter(void);
};

class CPPSOURCES_API CLineReadFilter: public CFilter
{
public:
	enum TLineEnd
	{
		DosLineEnd,
		UnixLineEnd,
		MacLineEnd
	};
	CLineReadFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output, TLineEnd lineEnd = DosLineEnd);
	virtual ~CLineReadFilter(void);

	virtual dword do_filter();

protected:
	TLineEnd m_LineEnd;

private:
	CLineReadFilter(void);
};

class CPPSOURCES_API CLineWriteFilter: public CFilter
{
public:
	enum TLineEnd
	{
		DosLineEnd,
		UnixLineEnd,
		MacLineEnd
	};
	CLineWriteFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output, TLineEnd lineEnd = DosLineEnd, dword maxLine = 16384, bool bLastLineHasLineEnd = true);
	virtual ~CLineWriteFilter(void);

	virtual dword do_filter();

protected:
	TLineEnd m_LineEnd;
	dword m_MaxLine;
	bool m_bLastLineHasLineEnd;

private:
	CLineWriteFilter(void);
};

DECL_EXCEPTION(CPPSOURCES_API, CFilterException, CBaseException)
