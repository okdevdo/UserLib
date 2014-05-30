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
#include "PDEFS.H"

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

sword __stdcall TestSortFunc( ConstPointer pa, ConstPointer pb );
sword __stdcall TestSortUserFunc( ConstPointer pa, ConstPointer pb, Pointer context );
sword __stdcall TestSortFuncUInt( ConstPointer pa, ConstPointer pb );
sword __stdcall TestSortFuncULongPointer(ConstPointer pa, ConstPointer pb);
int __cdecl TestCompareSRand(const void * pA, const void * pB);
int __cdecl TestCompareSRand64(const void * pA, const void * pB);
void __stdcall TestDeleteFunc( ConstPointer data, Pointer context );

class TestFuncUIntLessFunctor
{
public:
	bool operator()(ConstPtr(unsigned int) r1, ConstPtr(unsigned int) r2) const
	{
		return *r1 < *r2;
	}
};

typedef CDataVectorT<unsigned int, TestFuncUIntLessFunctor, CCppObjectNullFunctor<unsigned int>> TestFuncUIntVector;

void OpenTestFile(CConstPointer _TestFunction);
sdword WriteTestFile(int testcase, CConstPointer format, ...);
sdword WriteErrorTestFile(int testcase, CConstPointer format, ...);
sdword WriteSuccessTestFile(int testcase);
void CloseTestFile();

struct Item
{
	unsigned int _key;
	int _value;
	unsigned int key() const { return _key; }
	bool null() { return _key == 0; }
	int value() { return _value; }
	void show(OSTREAM& os) { os << _T("Key: ") << _key << _T(", Value: ") << _value << endl; }
	Item(): _key(0), _value(0) {}
	Item(int pKey, int pValue): _key(pKey), _value(pValue) {}
};

__inline bool operator == (const Item& a, const Item& b)
{
	return a.key() == b.key();
}

void showVisitor(Item v);

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
