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
 *  \file Filter.h
 *  \brief A collection of classes to build filter applications, ie a transformation of any input to any output.
 */
#pragma once

#include "CppSources.h"
#include "File.h"
#include "BaseException.h"
#include "DataVector.h"

/**
 *  \class CAbstractFilterInput
 *  \brief Abstract root of a class tree, representing the input part of the filters.
 */
class CPPSOURCES_API CAbstractFilterInput
{
public:
	/**
	 *  \brief Standard constructor
	 *  
	 *  \details Standard constructor.
	 */
	CAbstractFilterInput();
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor.
	 */
	virtual ~CAbstractFilterInput();

	/**
	 *  \brief Open the input
	 *  
	 *  \return void.
	 *  
	 *  \details Pure virtual function has to be overwritten.
	 */
	virtual void open() = 0;
	/**
	 *  \brief Getter size of input
	 *  
	 *  \return CFile::TFileSize size of input
	 *  
	 *  \details Pure virtual function has to be overwritten.
	 */
	virtual CFile::TFileSize size() = 0;
	/**
	 *  \brief Read in data
	 *  
	 *  \param [out] inputbuf data to be read in
	 *  \return void.
	 *  
	 *  \details Pure virtual function has to be overwritten.
	 */
	virtual void read(Ref(CByteBuffer) inputbuf) = 0;
	/**
	 *  \brief Close the input
	 *  
	 *  \return void.
	 *  
	 *  \details Pure virtual function has to be overwritten.
	 */
	virtual void close() = 0;
};

/**
 *  \class CAbstractFilterOutput
 *  \brief Abstract root of a class tree, representing the output part of the filters.
 */
class CPPSOURCES_API CAbstractFilterOutput
{
public:
	/**
	 *  \brief Standard constructor
	 *  
	 *  \details Standard constructor.
	 */
	CAbstractFilterOutput();
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor.
	 */
	virtual ~CAbstractFilterOutput();

	/**
	 *  \brief Open the output
	 *  
	 *  \return void.
	 *  
	 *  \details Pure virtual function has to be overwritten.
	 */
	virtual void open() = 0;
	/**
	 *  \brief Write out data
	 *  
	 *  \param [in] outputbuf data to be written
	 *  \return void.
	 *  
	 *  \details Pure virtual function has to be overwritten.
	 */
	virtual void write(Ref(CByteBuffer) outputbuf) = 0;
	/**
	 *  \brief Close the output
	 *  
	 *  \return void.
	 *  
	 *  \details Pure virtual function has to be overwritten.
	 */
	virtual void close() = 0;
};

/**
 *  \class CFilterInput
 *  \brief Base class of filter input.
 */
class CPPSOURCES_API CFilterInput: public CCppObject, public CAbstractFilterInput
{
public:
	/**
	 *  \brief Standard constructor
	 *  
	 *  \details Standard constructor.
	 */
	CFilterInput();
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor.
	 */
	virtual ~CFilterInput();

	/**
	 *  \brief Open the input
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void open();
	/**
	 *  \brief Getter size of input
	 *  
	 *  \return CFile::TFileSize size of input
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual CFile::TFileSize size();
	/**
	 *  \brief Read in data
	 *  
	 *  \param [out] inputbuf data to be read in
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void read(Ref(CByteBuffer) inputbuf);
	/**
	 *  \brief Close the input
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void close();
};

/**
 *  \class CFilterOutput
 *  \brief Base class of filter output.
 */
class CPPSOURCES_API CFilterOutput: public CCppObject, public CAbstractFilterOutput
{
public:
	/**
	 *  \brief Standard constructor
	 *  
	 *  \details Standard constructor.
	 */
	CFilterOutput();
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor.
	 */
	virtual ~CFilterOutput();

	/**
	 *  \brief Open the output
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void open();
	/**
	 *  \brief Write out data
	 *  
	 *  \param [in] outputbuf data to be written
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void write(Ref(CByteBuffer) outputbuf);
	/**
	 *  \brief Close the output
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void close();
};

/**
 *  \class CStringVectorFilterInput
 *  \brief Special input class. Input will be delivered from a string vector data type.
 */
class CPPSOURCES_API CStringVectorFilterInput : public CFilterInput
{
public:
	/**
	 *  \brief Constructor
	 *  \param input a string vector delivering input
	 *  \details Constructor.
	 */
	CStringVectorFilterInput(ConstRef(CDataVectorT<CStringBuffer>) input);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor.
	 */
	virtual ~CStringVectorFilterInput();

	/**
	 *  \brief Open the input
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void open();
	/**
	 *  \brief Getter size of input
	 *  
	 *  \return CFile::TFileSize size of input
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual CFile::TFileSize size();
	/**
	 *  \brief Read in data
	 *  
	 *  \param [out] inputbuf data to be read in
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void read(Ref(CByteBuffer) inputbuf);
	/**
	 *  \brief Close the input
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void close();

protected:
	ConstRef(CDataVectorT<CStringBuffer>) m_input;
	CDataVectorT<CStringBuffer>::Iterator m_inputIt;

private:
	CStringVectorFilterInput();
};

/**
 *  \class CFileFilterInput
 *  \brief Special input class. Input will be delivered from a disk file.
 */
class CPPSOURCES_API CFileFilterInput : public CFilterInput
{
public:
	/**
	 *  \brief Constructor
	 *  \param fname file name
	 *  \details Constructor.
	 */
	CFileFilterInput(ConstRef(CFilePath) fname);
	/**
	 *  \brief Constructor
	 *  \param ffile a CFile object
	 *  \details Constructor.
	 */
	CFileFilterInput(Ptr(CFile) ffile);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor.
	 */
	virtual ~CFileFilterInput();

	/**
	 *  \brief Open the input
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void open();
	/**
	 *  \brief Getter size of input
	 *  
	 *  \return CFile::TFileSize size of input
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual CFile::TFileSize size();
	/**
	 *  \brief Read in data
	 *  
	 *  \param [out] inputbuf data to be read in
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void read(Ref(CByteBuffer) inputbuf);
	/**
	 *  \brief Close the input
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void close();

protected:
	Ptr(CFile) m_inputFile;
	WBool m_canReOpen;
	CFilePath m_fileName;

private:
	CFileFilterInput();
};

/**
 *  \class CStdInFilterInput
 *  \brief Special input class. Input will be delivered from the standard input device.
 */
class CPPSOURCES_API CStdInFilterInput : public CFilterInput
{
public:
	/**
	 *  \brief Standard constructor
	 *  
	 *  \details Standard constructor.
	 */
	CStdInFilterInput();
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor.
	 */
	virtual ~CStdInFilterInput();

	/**
	 *  \brief Open the input
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void open();
	/**
	 *  \brief Getter size of input
	 *  
	 *  \return CFile::TFileSize size of input
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual CFile::TFileSize size();
	/**
	 *  \brief Read in data
	 *  
	 *  \param [out] inputbuf data to be read in
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void read(Ref(CByteBuffer) inputbuf);
	/**
	 *  \brief Close the input
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void close();
};

/**
 *  \class CStringVectorFilterOutput
 *  \brief Special output class. Output will be written to a string vector data type.
 */
class CPPSOURCES_API CStringVectorFilterOutput : public CFilterOutput
{
public:
	/**
	 *  \brief Constructor
	 *  \param output a string buffer vector to be filled
	 *  \details Constructor.
	 */
	CStringVectorFilterOutput(ConstRef(CDataVectorT<CStringBuffer>) output);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor.
	 */
	virtual ~CStringVectorFilterOutput();

	/**
	 *  \brief Open the output
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void open();
	/**
	 *  \brief Write out data
	 *  
	 *  \param [in] outputbuf data to be written
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void write(Ref(CByteBuffer) outputbuf);
	/**
	 *  \brief Close the output
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void close();

protected:
	ConstRef(CDataVectorT<CStringBuffer>) m_output;

private:
	CStringVectorFilterOutput();
};

/**
 *  \class CSortedStringVectorFilterOutput
 *  \brief Special output class. Output will be written to a sorted string vector data type.
 */
class CPPSOURCES_API CSortedStringVectorFilterOutput : public CFilterOutput
{
public:
	/**
	 *  \brief Constructor
	 *  \param output a sorted string buffer vector to be filled
	 *  \details Constructor.
	 */
	CSortedStringVectorFilterOutput(ConstRef(CDataVectorT<CStringBuffer>) output);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor.
	 */
	virtual ~CSortedStringVectorFilterOutput();

	/**
	 *  \brief Open the output
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void open();
	/**
	 *  \brief Write out data
	 *  
	 *  \param [in] outputbuf data to be written
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void write(Ref(CByteBuffer) outputbuf);
	/**
	 *  \brief Close the output
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void close();

protected:
	ConstRef(CDataVectorT<CStringBuffer>) m_output;

private:
	CSortedStringVectorFilterOutput();
};

/**
 *  \class CFileFilterOutput
 *  \brief Special output class. Output will be written to disk file.
 */
class CPPSOURCES_API CFileFilterOutput : public CFilterOutput
{
public:
	/**
	 *  \brief Constructor
	 *  \param fname file name
	 *  \details Constructor.
	 */
	CFileFilterOutput(ConstRef(CFilePath) fname);
	/**
	 *  \brief Constructor
	 *  \param ffile a CFile object
	 *  \details Constructor.
	 */
	CFileFilterOutput(Ptr(CFile) ffile);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor.
	 */
	virtual ~CFileFilterOutput();

	/**
	 *  \brief Open the output
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void open();
	/**
	 *  \brief Write out data
	 *  
	 *  \param [in] outputbuf data to be written
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void write(Ref(CByteBuffer) outputbuf);
	/**
	 *  \brief Close the output
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void close();

protected:
	Ptr(CFile) m_outputFile;
	WBool m_canReOpen;
	CFilePath m_fileName;

private:
	CFileFilterOutput();
};

/**
 *  \class CStdOutFilterOutput
 *  \brief Special output class. Output will be written to standard output device.
 */
class CPPSOURCES_API CStdOutFilterOutput : public CFilterOutput
{
public:
	/**
	 *  \brief Constructor
	 *  \param bNewLine
	 *  \details Constructor.
	 */
	CStdOutFilterOutput(bool bNewLine = false);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor.
	 */
	virtual ~CStdOutFilterOutput();

	/**
	 *  \brief Open the output
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void open();
	/**
	 *  \brief Write out data
	 *  
	 *  \param [in] outputbuf data to be written
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void write(Ref(CByteBuffer) outputbuf);
	/**
	 *  \brief Close the output
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void close();

protected:
	bool m_bNewLine;
};

/**
 *  \class CStdErrFilterOutput
 *  \brief Special output class. Output will be written to standard error device.
 */
class CPPSOURCES_API CStdErrFilterOutput : public CFilterOutput
{
public:
	/**
	 *  \brief Constructor
	 *  \param bNewLine
	 *  \details Constructor.
	 */
	CStdErrFilterOutput(bool bNewLine = false);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor.
	 */
	virtual ~CStdErrFilterOutput();

	/**
	 *  \brief Open the output
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void open();
	/**
	 *  \brief Write out data
	 *  
	 *  \param [in] outputbuf data to be written
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void write(Ref(CByteBuffer) outputbuf);
	/**
	 *  \brief Close the output
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void close();

protected:
	bool m_bNewLine;
};

/**
 *  \class CNullFilterOutput
 *  \brief Special output class. Output will not be handled in any way.
 */
class CPPSOURCES_API CNullFilterOutput : public CFilterOutput
{
public:
	/**
	 *  \brief Standard constructor
	 *  
	 *  \details Standard constructor.
	 */
	CNullFilterOutput();
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor.
	 */
	virtual ~CNullFilterOutput();

	/**
	 *  \brief Open the output
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void open();
	/**
	 *  \brief Write out data
	 *  
	 *  \param [in] outputbuf data to be written
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void write(Ref(CByteBuffer) outputbuf);
	/**
	 *  \brief Close the output
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function is overwritten.
	 */
	virtual void close();
};

/**
 *  \class CFilter
 *  \brief Base class of filter classes tree.
 */
class CPPSOURCES_API CFilter: public CCppObject
{
public:
	/**
	 *  \brief Constructor
	 *  \param input any filter input
	 *  \param output any filter output
	 *  \details Constructor.
	 */
	CFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor.
	 */
	virtual ~CFilter(void);

	/**
	 *  \brief Open the input and output with their open function
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function can be overwritten.
	 */
	virtual void open();
	/**
	 *  \brief Perform transformation
	 *  
	 *  \return dword result code of the operation.
	 *  
	 *  \details Pure virtual function has to be overwritten.
	 */
	virtual dword do_filter() = 0;
	/**
	 *  \brief Closes the input and output with their close function
	 *  
	 *  \return void.
	 *  
	 *  \details Virtual function can be overwritten.
	 */
	virtual void close();

	/**
	 *  \brief Read the input with the read function
	 *  
	 *  \param [out] inputbuf data to be read in
	 *  \return void.
	 *  
	 *  \details Virtual function can be overwritten.
	 */
	virtual void read(Ref(CByteBuffer) inputbuf);
	/**
	 *  \brief Write to the output with the write function
	 *  
	 *  \param [in] outputbuf data to be written
	 *  \return void.
	 *  
	 *  \details Virtual function can be overwritten.
	 */
	virtual void write(Ref(CByteBuffer) outputbuf);

protected:
	Ptr(CAbstractFilterInput) m_input;
	Ptr(CAbstractFilterOutput) m_output;

private:
	CFilter(void);
};

/**
 *  \class CCopyFilter
 *  \brief Special filter. Copies input to output.
 */
class CPPSOURCES_API CCopyFilter: public CFilter
{
public:
	/**
	 *  \brief Constructor
	 *  \param input any filter input
	 *  \param output any filter output
	 *  \details Constructor.
	 */
	CCopyFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor.
	 */
	virtual ~CCopyFilter(void);

	/**
	 *  \brief Perform copying
	 *  
	 *  \return dword result code of the operation.
	 *  
	 *  \details Pure virtual function has to be overwritten.
	 */
	virtual dword do_filter();

private:
	CCopyFilter(void);
};

/**
 *  \class CLineReadFilter
 *  \brief Special filter. Reads chunks of data in, and writes lines of data to output.
 */
class CPPSOURCES_API CLineReadFilter: public CFilter
{
public:
	enum TLineEnd
	{
		DosLineEnd,
		UnixLineEnd,
		MacLineEnd
	};
	/**
	 *  \brief Constructor
	 *  \param input any filter input
	 *  \param output any filter output
	 *  \param lineEnd presumed line ending character(s)
	 *  \details Constructor.
	 */
	CLineReadFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output, TLineEnd lineEnd = DosLineEnd);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor.
	 */
	virtual ~CLineReadFilter(void);

	/**
	 *  \brief Perform line read operation
	 *  
	 *  \return dword result code of the operation.
	 *  
	 *  \details Pure virtual function has to be overwritten.
	 */
	virtual dword do_filter();

protected:
	TLineEnd m_LineEnd;

private:
	CLineReadFilter(void);
};

/**
 *  \class CLineWriteFilter
 *  \brief Special filter. Reads data in, and appends a line end character, before writing.
 */
class CPPSOURCES_API CLineWriteFilter: public CFilter
{
public:
	enum TLineEnd
	{
		DosLineEnd,
		UnixLineEnd,
		MacLineEnd
	};
	/**
	 *  \brief Constructor
	 *  \param input any filter input
	 *  \param output any filter output
	 *  \param lineEnd determines the line ending character to be used
	 *  \param maxLine
	 *  \param bLastLineHasLineEnd
	 *  \details Constructor.
	 */
	CLineWriteFilter(Ptr(CAbstractFilterInput) input, Ptr(CAbstractFilterOutput) output, TLineEnd lineEnd = DosLineEnd, dword maxLine = 16384, bool bLastLineHasLineEnd = true);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor.
	 */
	virtual ~CLineWriteFilter(void);

	/**
	 *  \brief Perform line write operation
	 *  
	 *  \return dword result code of the operation.
	 *  
	 *  \details Pure virtual function has to be overwritten.
	 */
	virtual dword do_filter();

protected:
	TLineEnd m_LineEnd;
	dword m_MaxLine;
	bool m_bLastLineHasLineEnd;

private:
	CLineWriteFilter(void);
};

DECL_EXCEPTION(CPPSOURCES_API, CFilterException, CBaseException)
