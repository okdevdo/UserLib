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
#include "Filter.h"

#include <stdio.h>
#include <iostream>
#include <iomanip>

#ifdef _UNICODE
#define COUT std::wcout
#define CERR std::wcerr
#define OSTREAM std::wostream
#else
#define COUT std::cout
#define CERR std::cerr
#define OSTREAM std::ostream
#endif
using std::endl;

IMPL_EXCEPTION(CFilterException, CBaseException)

CAbstractFilterInput::CAbstractFilterInput()
{
}

CAbstractFilterInput::~CAbstractFilterInput()
{
}

CAbstractFilterOutput::CAbstractFilterOutput()
{
}

CAbstractFilterOutput::~CAbstractFilterOutput()
{
}

CFilterInput::CFilterInput() {}
CFilterInput::~CFilterInput() {}

void CFilterInput::open() {}
CFile::TFileSize CFilterInput::size() { return 0; }
void CFilterInput::read(Ref(CByteBuffer) inputbuf) {}
void CFilterInput::close() {}

CFilterOutput::CFilterOutput() {}
CFilterOutput::~CFilterOutput() {}

void CFilterOutput::open() {}
void CFilterOutput::write(Ref(CByteBuffer) outputbuf) {}
void CFilterOutput::close() {}

CStringVectorFilterInput::CStringVectorFilterInput(ConstRef(CDataVectorT<CStringBuffer>) input):
    m_input(input), m_inputIt()
{
}

CStringVectorFilterInput::~CStringVectorFilterInput()
{
}

void CStringVectorFilterInput::open()
{
	m_inputIt = m_input.Begin();
}

CFile::TFileSize CStringVectorFilterInput::size()
{
	CFile::TFileSize res = 0;
	CDataVectorT<CStringBuffer>::Iterator it = m_input.Begin();

	while ( it )
	{
		res += (*it).GetLength();
		++it;
	}
	return res;
}

void CStringVectorFilterInput::read(Ref(CByteBuffer) inputbuf)
{
	if ( !m_inputIt )
	{
		inputbuf.set_BufferSize(__FILE__LINE__ 0);
		return;
	}
	CStringBuffer tmp = *m_inputIt;

	tmp.convertToByteBuffer(inputbuf);
	++m_inputIt;
}

void CStringVectorFilterInput::close()
{
}

CFileFilterInput::CFileFilterInput(ConstRef(CFilePath) fname):
    CFilterInput(), m_inputFile(nullptr), m_canReOpen(true), m_fileName(fname)
{
}

CFileFilterInput::CFileFilterInput(Ptr(CFile) ffile):
    CFilterInput(), m_inputFile(ffile), m_canReOpen(true), m_fileName()
{
	if ( NotPtrCheck(m_inputFile) )
	{
		m_inputFile->addRef();
		m_fileName = m_inputFile->GetPath();
		m_canReOpen = false;
	}
}

CFileFilterInput::~CFileFilterInput()
{
	if ( NotPtrCheck(m_inputFile) )
		m_inputFile->release();
}

void CFileFilterInput::open()
{
	if ( PtrCheck(m_inputFile) )
	{
		m_inputFile = OK_NEW_OPERATOR CDiskFile(m_fileName, true, false, CFile::BinaryFile_NoEncoding);
		m_canReOpen = false;
		return; 
	}
	if ( m_canReOpen )
		m_inputFile->Open(m_fileName, true, false, CFile::BinaryFile_NoEncoding);
	m_canReOpen = false;
}

CFile::TFileSize CFileFilterInput::size()
{
	if ( PtrCheck(m_inputFile) )
		return 0;
	return m_inputFile->GetSize();
}

void CFileFilterInput::read(Ref(CByteBuffer) inputbuf)
{
	if ( PtrCheck(m_inputFile) )
	{
		inputbuf.set_BufferSize(__FILE__LINE__ 0);
		return;
	}
	m_inputFile->Read(inputbuf);
}

void CFileFilterInput::close()
{
	if ( PtrCheck(m_inputFile) )
		return;
	m_inputFile->Close();
	m_canReOpen = true;
}

CStdInFilterInput::CStdInFilterInput()
{
}

CStdInFilterInput::~CStdInFilterInput()
{
}

void CStdInFilterInput::open()
{
}

CFile::TFileSize CStdInFilterInput::size()
{ 
	return 0;
}

void CStdInFilterInput::read(Ref(CByteBuffer) inputbuf)
{
	if ( feof(stdin) || ferror(stdin) || (inputbuf.get_BufferSize() == 0) )
	{
		inputbuf.set_BufferSize(__FILE__LINE__ 0);
		return;
	}
	
	dword cnt = Castdword(fread(inputbuf.get_Buffer(), 1, inputbuf.get_BufferSize(), stdin));

	inputbuf.set_BufferSize(__FILE__LINE__ cnt);
}

void CStdInFilterInput::close()
{
}

CStringVectorFilterOutput::CStringVectorFilterOutput(ConstRef(CDataVectorT<CStringBuffer>) output):
    m_output(output)
{
}

CStringVectorFilterOutput::~CStringVectorFilterOutput()
{
}

void CStringVectorFilterOutput::open()
{
}

void CStringVectorFilterOutput::write(Ref(CByteBuffer) outputbuf)
{
	CStringBuffer tmp;

	tmp.convertFromByteBuffer(outputbuf);
	m_output.Append(tmp);
}

void CStringVectorFilterOutput::close()
{
}

CSortedStringVectorFilterOutput::CSortedStringVectorFilterOutput(ConstRef(CDataVectorT<CStringBuffer>) output):
    m_output(output)
{
}

CSortedStringVectorFilterOutput::~CSortedStringVectorFilterOutput()
{
}

void CSortedStringVectorFilterOutput::open()
{
}

void CSortedStringVectorFilterOutput::write(Ref(CByteBuffer) outputbuf)
{
	CStringBuffer tmp;

	tmp.convertFromByteBuffer(outputbuf);

	CDataVectorT<CStringBuffer>::Iterator it = m_output.FindSorted(tmp);

	if ( (!it) || (*it != tmp) )
		m_output.InsertSorted(tmp);
}

void CSortedStringVectorFilterOutput::close()
{
}

CFileFilterOutput::CFileFilterOutput(ConstRef(CFilePath) fname):
    CFilterOutput(), m_outputFile(nullptr), m_canReOpen(true), m_fileName(fname)
{
}

CFileFilterOutput::CFileFilterOutput(Ptr(CFile) ffile):
    CFilterOutput(), m_outputFile(ffile), m_canReOpen(true), m_fileName()
{
	if ( NotPtrCheck(m_outputFile) )
	{
		m_outputFile->addRef();
		m_fileName = m_outputFile->GetPath();
		m_canReOpen = false;
	}
}

CFileFilterOutput::~CFileFilterOutput()
{
	if ( NotPtrCheck(m_outputFile) )
		m_outputFile->release();
}

void CFileFilterOutput::open()
{
	if ( PtrCheck(m_outputFile) )
		m_outputFile = OK_NEW_OPERATOR CDiskFile();
	if ( m_canReOpen )
		m_outputFile->Create(m_fileName, false, CFile::BinaryFile_NoEncoding);
	m_canReOpen = false;
}

void CFileFilterOutput::write(Ref(CByteBuffer) outputbuf)
{
	if ( PtrCheck(m_outputFile) )
		return;
	m_outputFile->Write(outputbuf);
}

void CFileFilterOutput::close()
{
	if ( PtrCheck(m_outputFile) )
		return;
	m_outputFile->Close();
	m_canReOpen = true;
}

CStdOutFilterOutput::CStdOutFilterOutput(bool bNewLine): m_bNewLine(bNewLine)
{
}

CStdOutFilterOutput::~CStdOutFilterOutput()
{
}

void CStdOutFilterOutput::open()
{
}

void CStdOutFilterOutput::write(Ref(CByteBuffer) outputbuf)
{
	CStringBuffer tmp;

	tmp.convertFromByteBuffer(outputbuf);
	COUT << tmp;
	if ( m_bNewLine )
		COUT << endl;
}

void CStdOutFilterOutput::close()
{
}

CStdErrFilterOutput::CStdErrFilterOutput(bool bNewLine): m_bNewLine(bNewLine)
{
}

CStdErrFilterOutput::~CStdErrFilterOutput()
{
}

void CStdErrFilterOutput::open()
{
}

void CStdErrFilterOutput::write(Ref(CByteBuffer) outputbuf)
{
	CStringBuffer tmp;

	tmp.convertFromByteBuffer(outputbuf);
	CERR << tmp;
	if (m_bNewLine)
		CERR << endl;
}

void CStdErrFilterOutput::close()
{
}

CNullFilterOutput::CNullFilterOutput()
{
}

CNullFilterOutput::~CNullFilterOutput()
{
}

void CNullFilterOutput::open()
{
}

void CNullFilterOutput::write(Ref(CByteBuffer) outputbuf)
{
}

void CNullFilterOutput::close()
{
}

CFilter::CFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
    m_input(input), m_output(output)
{
}

CFilter::~CFilter(void)
{
}

void CFilter::open()
{
	if ( NotPtrCheck(m_input) )
		m_input->open();
	if ( NotPtrCheck(m_output) )
		m_output->open();
}

void CFilter::read(Ref(CByteBuffer) inputbuf)
{
	if (PtrCheck(m_input))
		inputbuf.set_BufferSize(__FILE__LINE__ 0);
	else
		m_input->read(inputbuf);
}

void CFilter::write(Ref(CByteBuffer) outputbuf)
{
	if ( NotPtrCheck(m_output) )
		m_output->write(outputbuf);
}

void CFilter::close()
{
	if ( NotPtrCheck(m_input) )
		m_input->close();
	if ( NotPtrCheck(m_output) )
		m_output->close();
}

CCopyFilter::CCopyFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output) :
    CFilter(input, output)
{
}

CCopyFilter::~CCopyFilter(void)
{
}

dword CCopyFilter::do_filter()
{
	CByteBuffer buffer(__FILE__LINE__ 16384);

	read(buffer);
	while ( buffer.get_BufferSize() > 0 )
	{
		write(buffer);
		buffer.set_BufferSize(__FILE__LINE__ 16384);
		read(buffer);
	}
	return 0;
}

CLineReadFilter::CLineReadFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output, TLineEnd lineEnd) :
    CFilter(input, output), m_LineEnd(lineEnd)
{
}

CLineReadFilter::~CLineReadFilter(void)
{
}

dword CLineReadFilter::do_filter()
{
	CByteBuffer buffer[2];
	CByteBuffer subBuf;
	CByteBuffer subBuf1;
	sword curBuf = 0;
	dword bufPos[2];
	int fpos;
	int fposoff = 0;
	bool bTestDosLineEnd = false;

	buffer[0].set_BufferSize(__FILE__LINE__ 16384);
	bufPos[0] = 0;
	buffer[1].set_BufferSize(__FILE__LINE__ 16384);
	bufPos[1] = 16384;

	read(buffer[curBuf]);
	while ( buffer[curBuf].get_BufferSize() > 0 )
	{
		while ( true )
		{
			if ( bTestDosLineEnd )
			{
				if ( *(buffer[curBuf].get_Buffer() + bufPos[curBuf]) == '\n' )
					++(bufPos[curBuf]);
				bTestDosLineEnd = false;
			}
			if ( bufPos[1-curBuf] < buffer[1-curBuf].get_BufferSize() )
			{
				buffer[1-curBuf].get_SubBuffer(bufPos[1-curBuf], buffer[1-curBuf].get_BufferSize() - bufPos[1-curBuf], subBuf1);
				bufPos[1-curBuf] = buffer[1-curBuf].get_BufferSize();
				subBuf.concat_Buffer(subBuf1);
			}
			switch ( m_LineEnd )
			{
			case DosLineEnd:
				fpos = buffer[curBuf].find_Char('\r', bufPos[curBuf]);
				if ( fpos < 0 )
					break;
				if ( fpos == (Castsdword(buffer[curBuf].get_BufferSize()) - 1) )
				{
					bTestDosLineEnd = true;
					break;
				}
				fposoff = 1;
				if ( *(buffer[curBuf].get_Buffer() + fpos + 1) == '\n' )
					++fposoff;
				break;
			case UnixLineEnd:
				fpos = buffer[curBuf].find_Char('\n', bufPos[curBuf]);
				fposoff = 1;
				break;
			case MacLineEnd:
				fpos = buffer[curBuf].find_Char('\r', bufPos[curBuf]);
				fposoff = 1;
				break;
			}
			if ( fpos < 0 )
				break;
			subBuf.concat_Buffer(buffer[curBuf].get_Buffer() + bufPos[curBuf], fpos - bufPos[curBuf]);
			write(subBuf);
			subBuf.set_BufferSize(__FILE__LINE__ 0);
			bufPos[curBuf] = fpos + fposoff;
			if ( bTestDosLineEnd )
				break;
		}
		curBuf = 1 - curBuf;
		buffer[curBuf].set_BufferSize(__FILE__LINE__ 16384);
		read(buffer[curBuf]);
		bufPos[curBuf] = 0;
	}
	if ( bufPos[1-curBuf] < buffer[1-curBuf].get_BufferSize() )
	{
		buffer[1-curBuf].get_SubBuffer(bufPos[1-curBuf], buffer[1-curBuf].get_BufferSize() - bufPos[1-curBuf], subBuf1);
		subBuf.concat_Buffer(subBuf1);
		write(subBuf);
	}
	return 0;
}

CLineWriteFilter::CLineWriteFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output, TLineEnd lineEnd, dword maxLine, bool bLastLineHasLineEnd) :
    CFilter(input, output), m_LineEnd(lineEnd), m_MaxLine(maxLine), m_bLastLineHasLineEnd(bLastLineHasLineEnd)
{
	if ( 0 == m_MaxLine )
		m_MaxLine = 16384;
}

CLineWriteFilter::~CLineWriteFilter(void)
{
}

dword CLineWriteFilter::do_filter()
{
	CByteBuffer bufNL;

	switch ( m_LineEnd )
	{
	case DosLineEnd:
		bufNL.set_Buffer(__FILE__LINE__ CastAny(BConstPointer, "\r\n"), 2);
		break;
	case UnixLineEnd:
		bufNL.set_Buffer(__FILE__LINE__ CastAny(BConstPointer, "\n"), 1);
		break;
	case MacLineEnd:
		bufNL.set_Buffer(__FILE__LINE__ CastAny(BConstPointer, "\r"), 1);
		break;
	}
	if ( m_bLastLineHasLineEnd )
	{
		CByteBuffer buf(__FILE__LINE__ m_MaxLine);

		read(buf);
		while ( buf.get_BufferSize() > 0 )
		{
			buf.concat_Buffer(bufNL);
			write(buf);
			buf.set_BufferSize(__FILE__LINE__ m_MaxLine);
			read(buf);
		}
	}
	else
	{
		CByteBuffer buf1(__FILE__LINE__ m_MaxLine);
		CByteBuffer buf2(__FILE__LINE__ m_MaxLine);

		read(buf1);
		if ( buf1.get_BufferSize() > 0 )
			read(buf2);
		while ( (buf1.get_BufferSize() > 0) && (buf2.get_BufferSize() > 0) )
		{
			buf1.concat_Buffer(bufNL);
			write(buf1);
			buf1.set_BufferSize(__FILE__LINE__ m_MaxLine);
			read(buf1);
			if ( buf1.get_BufferSize() > 0 )
				buf2.concat_Buffer(bufNL);
			write(buf2);
			if ( buf1.get_BufferSize() == 0 )
				break;
			buf2.set_BufferSize(__FILE__LINE__ m_MaxLine);
			read(buf2);
		}
		if ( buf1.get_BufferSize() > 0 )
			write(buf1);
	}
	return 0;
}