/******************************************************************************
    
	This file is part of XTest, which is part of UserLib.

    Copyright (C) 1995-2014  Oliver Kreis (okdev10@arcor.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by 
	the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/
#pragma once

#define _CRT_RAND_S
#include "pdefs.h"

#include "CppObject.h"
#include "StringLiteral.h"
#include "StringBuffer.h"
#include "ByteBuffer.h"
#include "DataVector.h"

#include <search.h>
#if defined(OK_COMP_MSC) && defined(__DEBUG__)
#include <conio.h>
#endif
#include <time.h>

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

#ifdef OK_SYS_UNIX
#define _tprintf printf
#endif

void OpenTestFile(CConstPointer _TestFunction);
sdword WriteTestFile(int testcase, CConstPointer format, ...);
sdword WriteErrorTestFile(const char* file, int line, int testcase, CConstPointer format, ...);
sdword AssertErrorTestFile(const char* file, int line, int testcase, CConstPointer text, ...);
#define ASSERTTESTFILE(testcase, boolexpr, ...) if (!(boolexpr)) AssertErrorTestFile(__FILE__, __LINE__, (testcase), _T(#boolexpr), __VA_ARGS__)
sdword WriteSuccessTestFile(int testcase);
void CloseTestFile();

void TestCppSources();
void TestDataStructures();
void TestCSources();
void TestLinkedByteBuffer();

void TestDirectoryIterator();
void ScanDirectory(TMBCharList&, CStringLiteral);

void TestDbase();

void TestThreads();
void TestThreads1();
void TestThreads2();

void TestTCPServer();
void TestTCPClient(CStringLiteral);

void TestTCPServerKeepAlive();
void TestTCPClientKeepAlive(CStringLiteral);

void TestFTPClient();

void TestConsole();
void TestSQL();

void TestHTTP1(ConstRef(TMBCharList));
void TestHTTP2(CStringLiteral);

void TestAsyncFile();
void TestAsyncTCPClient();
void TestAsyncTCPServer();

void TestFileFilter();
void TestSAXParser();
void TestSSLFunc();
void TestSSLServerFunc();
void TestEventLog();

void TestSqLite3ServiceClient();
void TestWinSources();
