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
#include "Program.h"
#include "ByteLinkedBuffer.h"

#include "STRUTIL.H"

static const char* sTest1 = 
	"When you create the project, you can specify the include files\n"
	"and macro definitions that will be used by the built-in parser.\n"
	"Or if the project has been built with debug information, you can\n"
	"have the built-in parser automatically search for the include\n"
	"files and macro definitions for each compiled source file.\n";
static const char* sTest2 = 
	"To specify additional code assistance configuration information\n"
	"that will improve the accuracy of the code assistance features\n"
	"of the IDE for a project, use the Configure Code Assistance wizard.\n"
	"To start the wizard, right-click your project and choose Configure\n"
	"Code Assistance.";
static const char* sTest3 = "TestTestTestTest";

#define tstrlen(p) Castdword(strlen(p))

void TestLinkedByteBuffer() 
{
	CByteLinkedBuffer buf;
	CByteLinkedBuffer::Iterator it(buf.Begin());
	CByteBuffer bufB;
	CStringBuffer bufS;
	BPointer p;

	p = buf.AddBufferItem(tstrlen(sTest1));
	s_memcpy(p, (BPointer)sTest1, tstrlen(sTest1));

	p = buf.AddBufferItem(tstrlen(sTest2));
	s_memcpy(p, (BPointer)sTest2, tstrlen(sTest2));

	buf.GetBuffer(bufB);
	bufS.convertFromByteBuffer(bufB);

	COUT << bufS << endl;

	bufB.set_BufferSize(__FILE__LINE__ tstrlen(sTest3));
	s_memcpy(bufB.get_Buffer(), (BPointer)sTest3, tstrlen(sTest3));
	it += tstrlen(sTest1) - (tstrlen(sTest3)/2);
	buf.ReplaceSubBuffer(it, tstrlen(sTest3), bufB);

	bufB.set_BufferSize(__FILE__LINE__ tstrlen(sTest3));
	buf.GetSubBuffer(it, bufB);
	bufS.convertFromByteBuffer(bufB);

	COUT << bufS << endl;

	buf.GetBuffer(bufB);
	bufS.convertFromByteBuffer(bufB);

	COUT << bufS << endl << endl;

	BPointer strs[] = {
		CastAnyPtr(byte, CastMutablePtr(char, "additional")),
		CastAnyPtr(byte, CastMutablePtr(char, "code")),
		CastAnyPtr(byte, CastMutablePtr(char, "wizard"))
	};
	dword strslen[] = { 10, 4, 6 };

	it = buf.Begin();
	it.FindAnyStr(strs, strslen, 3);
	while (it)
	{
		bufB.set_BufferSize(__FILE__LINE__ 10);
		buf.GetSubBuffer(it, bufB);
		bufS.convertFromByteBuffer(bufB);

		COUT << _T("Index=") << it.GetIndex() << _T(", Position=") << it.GetPosition() << _T(", bufS=") << bufS << endl;
		++it;
		it.FindAnyStr(strs, strslen, 3);
	}

	it = buf.Begin();
	it += 10;
	buf.InsertBuffer(it, CastAnyPtr(byte, CastMutablePtr(char, sTest3)), tstrlen(sTest3));

	buf.GetBuffer(bufB);
	bufS.convertFromByteBuffer(bufB);

	COUT << bufS << endl;

	buf.Clear();
	for ( int ix = 0; ix < 100000; ++ix )
	{
		p = buf.AddBufferItem(tstrlen(sTest1));
		s_memcpy(p, (BPointer)sTest1, tstrlen(sTest1));
	}
	COUT << _T("tstrlen(sTest1) = ") << tstrlen(sTest1) << endl;
	COUT << _T("buf.GetBufferItemCount() = ") << buf.GetBufferItemCount() << endl;
	COUT << _T("buf.GetTotalLength() = ") << buf.GetTotalLength() << endl;
	buf.Clear();
	for ( int ix = 0; ix < 100000; ++ix )
	{
		p = buf.AddBufferItem(tstrlen(sTest1));
		s_memcpy(p, (BPointer)sTest1, tstrlen(sTest1));
	}
	COUT << _T("tstrlen(sTest1) = ") << tstrlen(sTest1) << endl;
	COUT << _T("buf.GetBufferItemCount() = ") << buf.GetBufferItemCount() << endl;
	COUT << _T("buf.GetTotalLength() = ") << buf.GetTotalLength() << endl;
	it = buf.Begin();
	for ( int ix = 12500; ix < 100000; ix += 12500 )
	{
		it += 12500 * 313;
		bufB.set_BufferSize(__FILE__LINE__ tstrlen(sTest1));
		buf.GetSubBuffer(it, bufB);
		bufS.convertFromByteBuffer(bufB);
		COUT << bufS << endl << endl;
		it += 313;
	}
	buf.Clear();
	for ( int ix = 0; ix < 100000; ++ix )
	{
		p = buf.AddBufferItem(tstrlen(sTest1) * 2);
		s_memcpy(p, (BPointer)sTest1, tstrlen(sTest1));
		s_memcpy(p + tstrlen(sTest1), (BPointer)sTest1, tstrlen(sTest1));
	}
	COUT << _T("tstrlen(sTest1) = ") << tstrlen(sTest1) << endl;
	COUT << _T("buf.GetBufferItemCount() = ") << buf.GetBufferItemCount() << endl;
	COUT << _T("buf.GetTotalLength() = ") << buf.GetTotalLength() << endl;
	buf.Clear();
}
