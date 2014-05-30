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
#include "File.h"
#include "DirectoryIterator.h"
#include "FlagRegister.h"
#include "CppDataStructures.h"
#include "DataDoubleLinkedList.h"
#include "DataArray.h"
#include "DataVector.h"
#include "DataBinaryTree.h"
#include "DataBTree.h"
#include "DataHashtable.h"
#include "DataGraph.h"

static void LoadTestData(CConstPointer _TestFunction, unsigned int *numbers, unsigned int cnt)
{
	Ptr(CFile) _DataFile = NULL;

	CFilePath fname(__FILE__LINE__ _T("DataFiles"));

	fname.MakeDirectory();

	try
	{
		if (CDirectoryIterator::DirectoryExists(fname) < 0)
			CDirectoryIterator::MakeDirectory(fname);
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
		return;
	}

	CStringBuffer sname(__FILE__LINE__ _TestFunction);
	bool fExist = false;
	unsigned int number;
	unsigned int i;
#ifdef OK_COMP_GNUC
	unsigned int j;
#endif

	for (i = 0; i < cnt; ++i)
		numbers[i] = 0;
	sname.AppendString(_T(".num.dat"));
	fname.set_Filename(sname);
	_DataFile = new CStreamFile;

	try
	{
		fExist = CDirectoryIterator::FileExists(fname);
		if (fExist)
		{
			_DataFile->Open(fname, true, false, CFile::BinaryFile_NoEncoding);
			i = 0;
			while (!(_DataFile->IsEOF()))
			{
				_DataFile->Read(_T("%u\r\n"), &number);
				numbers[i++] = number;
				if (i >= cnt)
					break;
			}
		}
		else
		{
			_DataFile->Create(fname, false, CFile::BinaryFile_NoEncoding);
			srand((unsigned)time(NULL));
			for (i = 0; i < cnt; ++i)
			{
#ifdef OK_COMP_GNUC
				bool bOk = true;

				number = numbers[i] = rand();
				while (bOk)
				{
					bOk = false;
					for (j = 0; j < i; ++j)
					if (numbers[j] == number) {
						number = numbers[i] = rand();
						bOk = true;
						break;
					}
				}
#endif
#ifdef OK_COMP_MSC
				errno_t err = rand_s(&number);
				if (err != 0)
					WriteErrorTestFile(1, _T("rand_s failed"));
				numbers[i] = number;
#endif
				_DataFile->Write(_T("%u\r\n"), number);
			}
		}
		_DataFile->Close();
		_DataFile->release();
		_DataFile = NULL;
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
		_DataFile->release();
		_DataFile = NULL;
		return;
	}
}

static void LoadTestData64(CConstPointer _TestFunction, ULongPointer *numbers, unsigned int cnt)
{
	Ptr(CFile) _DataFile = NULL;

#ifdef OK_CPU_32BIT
	CFilePath fname(__FILE__LINE__ _T("DataFiles"));
#endif
#ifdef OK_CPU_64BIT
	CFilePath fname(__FILE__LINE__ _T("DataFiles64"));
#endif

	fname.MakeDirectory();

	try
	{
		if (CDirectoryIterator::DirectoryExists(fname) < 0)
			CDirectoryIterator::MakeDirectory(fname);
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
		return;
	}

	CStringBuffer sname(__FILE__LINE__ _TestFunction);
	bool fExist = false;
	ULongPointer number;
	unsigned int i;
#ifdef OK_COMP_GNUC
	unsigned int j;
#endif

	for (i = 0; i < cnt; ++i)
		numbers[i] = 0;
	sname.AppendString(_T(".num.dat"));
	fname.set_Filename(sname);
	_DataFile = new CStreamFile;

	try
	{
		fExist = CDirectoryIterator::FileExists(fname);
		if (fExist)
		{
			_DataFile->Open(fname, true, false, CFile::BinaryFile_NoEncoding);
			i = 0;
			while (!(_DataFile->IsEOF()))
			{
#ifdef OK_CPU_32BIT
				_DataFile->Read(_T("%u\r\n"), &number);
#endif
#ifdef OK_CPU_64BIT
				_DataFile->Read(_T("%llu\r\n"), &number);
#endif
				numbers[i++] = number;
				if (i >= cnt)
					break;
			}
		}
		else
		{
			_DataFile->Create(fname, false, CFile::BinaryFile_NoEncoding);
			srand((unsigned)time(NULL));
			for (i = 0; i < cnt; ++i)
			{
#ifdef OK_COMP_GNUC
				bool bOk = true;

				number = numbers[i] = (ULongPointer)rand();
				while (bOk)
				{
					bOk = false;
					for (j = 0; j < i; ++j)
					if (numbers[j] == number) {
						number = numbers[i] = rand();
						bOk = true;
						break;
					}
				}
#endif
#ifdef OK_COMP_MSC
				number = 0;
				errno_t err = rand_s(CastAnyPtr(unsigned int, &number));
				if (err != 0)
					WriteErrorTestFile(1, _T("rand_s failed"));
				numbers[i] = number;
#endif
#ifdef OK_CPU_32BIT
				_DataFile->Write(_T("%u\r\n"), number);
#endif
#ifdef OK_CPU_64BIT
				_DataFile->Write(_T("%llu\r\n"), number);
#endif
			}
		}
		_DataFile->Close();
		_DataFile->release();
		_DataFile = NULL;
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
		_DataFile->release();
		_DataFile = NULL;
		return;
	}
}

static void TestFlagRegister()
{
	OpenTestFile(_T("TestFlagRegister"));

	CFlagRegister::pointer p1 = (CFlagRegister::pointer)TFalloc(1024);
	CFlagRegister r1(p1, 1024);

	r1.reset();
	r1.set(0, 16);

	CFlagRegister::pointer p2 = (CFlagRegister::pointer)TFalloc(1024);
	CFlagRegister r2(p2, 1024);

	r2.reset();
	r2 |= r1;

	CFlagRegister::value_type i = r2.findnext();
	CFlagRegister::value_type ix = 0;

	if (i != 0)
		WriteErrorTestFile(1, _T("Start of Loop: i != 0"));
	while (i != CFlagRegister::notfound)
	{
		i = r2.findnext(true, i + 1);
		if (i != CFlagRegister::notfound)
		{
			if (Castdword((i % 16)) != 0)
				WriteErrorTestFile(1, _T("Inside Loop: (i % 16) != 0"));
			if (Castdword((i / 16)) != Castdword(++ix))
				WriteErrorTestFile(1, _T("Inside Loop: (i / 16) != ix"));
		}
	}
	if (ix != 63)
		WriteErrorTestFile(1, _T("End of Loop: ix != 63"));
	TFfree(p1);
	TFfree(p2);
	WriteSuccessTestFile(1);

	CloseTestFile();
}

template <class Item, class Key>
class CBinaryTreeDebugT: public CBinaryTreeT<Item, Key>
{
protected:
	typedef CBinaryTreeT<Item, Key> super;
#ifdef OK_COMP_GNUC
	typedef typename super::node* link;
#endif

	void testR(link h)
	{
		if (h == 0) return;
		if ((h->l == 0) && (h->r == 0))
		{
			if (h->N != 1)
				WriteErrorTestFile(1, _T("h->N != 1"));
			return;
		}
		if (h->l == 0)
		{
			if (h->N != (h->r->N + 1))
				WriteErrorTestFile(1, _T("h->N != (h->r->N + 1)"));
			if (h->item.key() >= h->r->item.key())
				WriteErrorTestFile(1, _T("h->item.key() >= h->r->item.key()"));
			testR(h->r);
			return;
		}
		if (h->r == 0)
		{
			if (h->N != (h->l->N + 1))
				WriteErrorTestFile(1, _T("h->N != (h->l->N + 1)"));
			if (h->item.key() <= h->l->item.key())
				WriteErrorTestFile(1, _T("h->item.key() <= h->l->item.key()"));
			testR(h->l);
			return;
		}
		if (h->N != (h->l->N + h->r->N + 1))
			WriteErrorTestFile(1, _T("h->N != (h->l->N + h->r->N + 1)"));
		if (h->item.key() >= h->r->item.key())
			WriteErrorTestFile(1, _T("h->item.key() >= h->r->item.key()"));
		if (h->item.key() <= h->l->item.key())
			WriteErrorTestFile(1, _T("h->item.key() <= h->l->item.key()"));
		testR(h->l);
		testR(h->r);
	}
public:
	CBinaryTreeDebugT(int maxN = 0):
		super(maxN)
	{
	}
	void test()
	{
		testR(super::head);
	}
};

static void TestCBinaryTreeT()
{
	OpenTestFile(_T("TestCBinaryTreeT"));

	CBinaryTreeDebugT<Item, unsigned int> _st(1000);
	int i;
	Item result;
	unsigned int numbers[1000];

	LoadTestData(_T("TestCBinaryTreeT"), numbers, 1000);
	for ( i = 0; i < 1000; ++i )
	{
		_st.insert(Item(numbers[i], i));
		if ( (i % 100) == 0 )
			_st.test();
	}
	qsort(numbers, 1000, sizeof(unsigned int), TestCompareSRand);

	WriteTestFile(1, _T("Height of Tree before balance = %d"), _st.height());
	_st.test();
	_st.balance();
	WriteTestFile(1, _T("Height of Tree after balance = %d"), _st.height());
	_st.test();
	for ( i = 0; i < 1000; ++i )
	{
		result = _st.search(numbers[i]);
		if (!(result == Item(numbers[i], i)))
			WriteErrorTestFile(1, _T("result != Item(numbers[%d], %d)"), i, i);
	}
	//_st.inOrder(showVisitor);
	result = _st.select(1);
	for ( i = 0; i < 1000; ++i )
	{
		_st.remove(numbers[i]);
		if ( (i % 100) == 0 )
			_st.test();
	}
	WriteSuccessTestFile(1);

	CloseTestFile();
}

template <class Item, class Key, int M> // M must be even
class CDebugBTreeT : public CBTreeT<Item, Key, M>
{
protected:
	typedef CBTreeT<Item, Key, M> super;
#ifdef OK_COMP_GNUC
	typedef typename super::node* link;
#endif
	void testR(link h, int ht)
	{
		if (h->m == 0)
			WriteErrorTestFile(1, _T("h->m == 0"));
		if (ht == 0)
		{
			for (int j = 1; j < h->m; ++j) 
				if (h->b[j - 1].key > h->b[j].key)
					WriteErrorTestFile(1, _T("h->b[j - 1].key > h->b[j].key"));
		}
		else
		{
			if (h->b[0].key != h->b[0].next->b[0].key)
				WriteErrorTestFile(1, _T("h->b[0].key != h->b[0].next->b[0].key"));
			for (int j = 1; j < h->m; ++j)
				if (h->b[j - 1].key >= h->b[j].key)
					WriteErrorTestFile(1, _T("h->b[j - 1].key >= h->b[j].key"));
			for (int j = 0; j < h->m; ++j)
				testR(h->b[j].next, ht - 1);
		}
	}
public:
	void test()
	{
		testR(super::head, super::HT);
	}
};


static void TestCBTreeT()
{
	OpenTestFile(_T("TestCBTreeT"));

	CDebugBTreeT<Item, unsigned int, 100> _st;
	int i;
	int j;
	Item result;
	unsigned int* numbers = new unsigned int[100000];

	LoadTestData(_T("TestCBTreeT"), numbers, 100000);
	for ( i = 0; i < 100000; ++i )
		_st.insert(Item(numbers[i], i));

	_st.test();
	for ( i = 0; i < 100000; ++i )
	{
		result = _st.search(numbers[i]);
		if (!(result == Item(numbers[i], i)))
			WriteErrorTestFile(1, _T("result != Item(numbers[i], i)"));
	}
	WriteTestFile(1, _T("Height of Tree = %d, Count of nodes = %d"), _st.height(), _st.count());

	for ( i = 0; i < 100000; ++i )
	{
		_st.remove(numbers[i]);
		if ((i % 100) == 0)
		{
			_st.test();
			for (j = i + 1; j < 100000; ++j)
			{
				result = _st.search(numbers[j]);
				if (!(result == Item(numbers[j], j)))
					WriteErrorTestFile(1, _T("result != Item(numbers[j], j)"));
			}
		}
	}
	WriteTestFile(1, _T("Height of Tree = %d, Count of nodes = %d"), _st.height(), _st.count());
	delete[] numbers;

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestLinkedListT()
{
	OpenTestFile(_T("TestLinkedListT"));

	CLinkedListT<Item, unsigned int> _st;
	int i;
	Item result;
	unsigned int numbers[1000];

	LoadTestData(_T("TestLinkedListT"), numbers, 1000);
	for (i = 0; i < 1000; ++i)
		_st.insert(Item(numbers[i], i));
	//qsort(numbers, 1000, sizeof(unsigned int), TestCompareSRand);
    _st.sort();
	for ( i = 0; i < 1000; ++i )
	{
		result = _st.search(numbers[i]);
		if (!(result == Item(numbers[i], i)))
			WriteErrorTestFile(1, _T("result != Item(numbers[i], i)"));
	}
	//_st.traverse(showVisitor);
	for ( i = 0; i < 1000; ++i )
		_st.remove(numbers[i]);
	if (!(_st.empty()))
		WriteErrorTestFile(1, _T("!(_st.empty())"));

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestHashLinkedListT()
{
	OpenTestFile(_T("TestHashLinkedListT"));

	CHashLinkedListT<Item, unsigned int, HashFunctorDigit> _st(100000);
	int i;
	Item result;
	unsigned int* numbers = new unsigned int[100000];

	LoadTestData(_T("TestHashLinkedListT"), numbers, 100000);
	for (i = 0; i < 100000; ++i)
		_st.insert(Item(numbers[i], i));

//	qsort(numbers, 100000, sizeof(unsigned int), TestCompareSRand);

	for ( i = 0; i < 100000; ++i )
	{
		result = _st.search(numbers[i]);
		if (!(result == Item(numbers[i], i)))
			WriteErrorTestFile(1, _T("result != Item(numbers[i], i)"));
	}
	for ( i = 0; i < 100000; ++i )
		_st.remove(numbers[i]);
	delete[] numbers;

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestHashLinearExploreT()
{
	OpenTestFile(_T("TestHashLinearExploreT"));

	CHashLinearExploreT<Item, unsigned int, HashFunctorDigit> _st(5000);
	int i;
	Item result;
	unsigned int* numbers = new unsigned int[5000];

	LoadTestData(_T("TestHashLinearExploreT"), numbers, 5000);
	for (i = 0; i < 5000; ++i)
		_st.insert(Item(numbers[i], i));

	// qsort(numbers, 5000, sizeof(unsigned int), TestCompareSRand);

	for ( i = 0; i < 5000; ++i )
	{
		result = _st.search(numbers[i]);
		if (!(result == Item(numbers[i], i)))
			WriteErrorTestFile(1, _T("result != Item(numbers[i], i)"));
	}
	for ( i = 0; i < 5000; ++i )
		_st.remove(numbers[i]);
	delete[] numbers;

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestDoubleLinkedList()
{
	OpenTestFile(_T("TestDoubleLinkedList"));

	CDataDoubleLinkedList list __FILE__LINE__0P;
	CDataDoubleLinkedList::Iterator it;
	dword i;
	dword j;
	ULongPointer numbers[120];

	LoadTestData64(_T("TestDoubleLinkedList"), numbers, 120);
	for (i = 0; i < 120; ++i)
		list.Append((Pointer)(numbers[i]));

	qsort(numbers, 120, sizeof(ULongPointer), TestCompareSRand64);

	list.Sort(TestSortFunc);
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (!(*it == (Pointer)(numbers[i++])))
			WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[i++])"));
		++it;
	}
	if (i != 120)
		WriteErrorTestFile(1, _T("i != 120"));
	it = list.Last();
	i = 120;
	while (it)
	{
		if (!(*it == (Pointer)(numbers[--i])))
			WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[--i])"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted((Pointer)numbers[i], TestSortFunc);
		j = i;
		while (it)
		{
			if (!(*it == (Pointer)(numbers[j++])))
				WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[j++])"));
			++it;
		}
		if (j != 120)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < 120; i += 2)
		list.RemoveSorted((Pointer)numbers[i], TestSortFunc, TestDeleteFunc, NULL);
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted((Pointer)numbers[i], TestSortFunc);
		if ((i % 2) == 0)
		{
			if (*it == (Pointer)(numbers[i]))
				WriteErrorTestFile(1, _T("*it == (Pointer)(numbers[i])"));
			if (*it != (Pointer)(numbers[i + 1]))
				WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[i + 1])"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (!(*it == (Pointer)(numbers[j])))
					WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[j])"));
				j += 2;
				++it;
			}
			if (j != 121)
				WriteErrorTestFile(1, _T("j != 121"));
		}
	}
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestDoubleLinkedListT()
{
	OpenTestFile(_T("TestDoubleLinkedListT"));

	CDataDoubleLinkedListT<unsigned int> list __FILE__LINE__0P;
	CDataDoubleLinkedListT<unsigned int>::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[120];

	LoadTestData(_T("TestDoubleLinkedListT"), numbers, 120);
	for (i = 0; i < 120; ++i)
		list.Append(numbers + i);

	qsort(numbers, 120, sizeof(unsigned int), TestCompareSRand);

	list.Sort(TestSortFuncUInt);
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (**it != numbers[i++])
			WriteErrorTestFile(1, _T("**it != numbers[i++]"));
		++it;
	}
	if (i != 120)
		WriteErrorTestFile(1, _T("i != 120"));
	it = list.Last();
	i = 120;
	while (it)
	{
		if (**it != numbers[--i])
			WriteErrorTestFile(1, _T("**it != numbers[--i]"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		j = i;
		while (it)
		{
			if (**it != numbers[j++])
				WriteErrorTestFile(1, _T("**it != numbers[j++]"));
			++it;
		}
		if (j != 120)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < 120; i += 2)
		list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL);
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if ((i % 2) == 0)
		{
			if (**it == numbers[i])
				WriteErrorTestFile(1, _T("**it == numbers[i]"));
			if (**it != numbers[i + 1])
				WriteErrorTestFile(1, _T("**it != numbers[i + 1]"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (**it != numbers[j])
					WriteErrorTestFile(1, _T("**it != numbers[j]"));
				j += 2;
				++it;
			}
			if (j != 121)
				WriteErrorTestFile(1, _T("j != 121"));
		}
	}
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestSDoubleLinkedList()
{
	OpenTestFile(_T("TestSDoubleLinkedList"));

	CDataSDoubleLinkedList list(__FILE__LINE__ sizeof(unsigned int));
	CDataSDoubleLinkedList::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[120];

	LoadTestData(_T("TestSDoubleLinkedList"), numbers, 120);
	for (i = 0; i < 120; ++i)
		list.Append(numbers + i);

	qsort(numbers, 120, sizeof(unsigned int), TestCompareSRand);

	list.Sort(TestSortFuncUInt);
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (DerefAnyPtr(unsigned int, *it) != numbers[i++])
			WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[i++]"));
		++it;
	}
	if (i != 120)
		WriteErrorTestFile(1, _T("i != 120"));
	it = list.Last();
	i = 120;
	while (it)
	{
		if (DerefAnyPtr(unsigned int, *it) != numbers[--i])
			WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[--i]"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		j = i;
		while (it)
		{
			if (DerefAnyPtr(unsigned int, *it) != numbers[j++])
				WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[j++]"));
			++it;
		}
		if (j != 120)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < 120; i += 2)
		list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL);
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if ((i % 2) == 0)
		{
			if (DerefAnyPtr(unsigned int, *it) == numbers[i])
				WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) == numbers[i]"));
			if (DerefAnyPtr(unsigned int, *it) != numbers[i + 1])
				WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[i + 1]"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (DerefAnyPtr(unsigned int, *it) != numbers[j])
					WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[j]"));
				j += 2;
				++it;
			}
			if (j != 121)
				WriteErrorTestFile(1, _T("j != 121"));
		}
	}
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestSDoubleLinkedListT()
{
	OpenTestFile(_T("TestSDoubleLinkedListT"));

	CDataSDoubleLinkedListT<unsigned int> list __FILE__LINE__0P;
	CDataSDoubleLinkedListT<unsigned int>::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[120];

	LoadTestData(_T("TestSDoubleLinkedListT"), numbers, 120);
	for (i = 0; i < 120; ++i)
		list.Append(numbers + i);

	qsort(numbers, 120, sizeof(unsigned int), TestCompareSRand);

	list.Sort(TestSortFuncUInt);
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (**it != numbers[i++])
			WriteErrorTestFile(1, _T("**it != numbers[i++]"));
		++it;
	}
	if (i != 120)
		WriteErrorTestFile(1, _T("i != 120"));
	it = list.Last();
	i = 120;
	while (it)
	{
		if (**it != numbers[--i])
			WriteErrorTestFile(1, _T("**it != numbers[--i]"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		j = i;
		while (it)
		{
			if (**it != numbers[j++])
				WriteErrorTestFile(1, _T("**it != numbers[j++]"));
			++it;
		}
		if (j != 120)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < 120; i += 2)
		list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL);
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if ((i % 2) == 0)
		{
			if (**it == numbers[i])
				WriteErrorTestFile(1, _T("**it == numbers[i]"));
			if (**it != numbers[i + 1])
				WriteErrorTestFile(1, _T("**it != numbers[i + 1]"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (**it != numbers[j])
					WriteErrorTestFile(1, _T("**it != numbers[j]"));
				j += 2;
				++it;
			}
			if (j != 121)
				WriteErrorTestFile(1, _T("j != 121"));
		}
	}
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestArray()
{
	OpenTestFile(_T("TestArray"));

	CDataArray list(__FILE__LINE__ 120);
	CDataArray::Iterator it;
	dword i;
	dword j;
	ULongPointer numbers[120];

	LoadTestData64(_T("TestArray"), numbers, 120);
	for (i = 0; i < 120; ++i)
		list.Append((Pointer)(numbers[i]));

	qsort(numbers, 120, sizeof(ULongPointer), TestCompareSRand64);

	list.Sort(TestSortFunc);
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (!(*it == (Pointer)(numbers[i++])))
			WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[i++])"));
		++it;
	}
	if (i != 120)
		WriteErrorTestFile(1, _T("i != 120"));
	it = list.Last();
	i = 120;
	while (it)
	{
		if (!(*it == (Pointer)(numbers[--i])))
			WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[--i])"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted((Pointer)numbers[i], TestSortFunc);
		j = i;
		while (it)
		{
			if (!(*it == (Pointer)(numbers[j++])))
				WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[j++])"));
			++it;
		}
		if (j != 120)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < 120; i += 2)
		list.RemoveSorted((Pointer)numbers[i], TestSortFunc, TestDeleteFunc, NULL);
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted((Pointer)numbers[i], TestSortFunc);
		if ((i % 2) == 0)
		{
			++it;
			if (*it == (Pointer)(numbers[i]))
				WriteErrorTestFile(1, _T("*it == (Pointer)(numbers[i])"));
			if (*it != (Pointer)(numbers[i + 1]))
				WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[i + 1])"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (!(*it == (Pointer)(numbers[j])))
					WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[j])"));
				j += 2;
				++it;
			}
			if (j != 121)
				WriteErrorTestFile(1, _T("j != 121"));
		}
	}
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestArrayT()
{
	OpenTestFile(_T("TestArrayT"));

	CDataArrayT<unsigned int> list(__FILE__LINE__ 120);
	CDataArrayT<unsigned int>::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[120];

	LoadTestData(_T("TestArrayT"), numbers, 120);
	for (i = 0; i < 120; ++i)
		list.Append(numbers + i);

	qsort(numbers, 120, sizeof(unsigned int), TestCompareSRand);

	list.Sort(TestSortFuncUInt);
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (**it != numbers[i++])
			WriteErrorTestFile(1, _T("**it != numbers[i++]"));
		++it;
	}
	if (i != 120)
		WriteErrorTestFile(1, _T("i != 120"));
	it = list.Last();
	i = 120;
	while (it)
	{
		if (**it != numbers[--i])
			WriteErrorTestFile(1, _T("**it != numbers[--i]"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		j = i;
		while (it)
		{
			if (**it != numbers[j++])
				WriteErrorTestFile(1, _T("**it != numbers[j++]"));
			++it;
		}
		if (j != 120)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < 120; i += 2)
		list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL);
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if ((i % 2) == 0)
		{
			++it;
			if (**it == numbers[i])
				WriteErrorTestFile(1, _T("**it == numbers[i]"));
			if (**it != numbers[i + 1])
				WriteErrorTestFile(1, _T("**it != numbers[i + 1]"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (**it != numbers[j])
					WriteErrorTestFile(1, _T("**it != numbers[j]"));
				j += 2;
				++it;
			}
			if (j != 121)
				WriteErrorTestFile(1, _T("j != 121"));
		}
	}
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestSArray()
{
	OpenTestFile(_T("TestSArray"));

	CDataSArray list(__FILE__LINE__ 120, sizeof(unsigned int));
	CDataSArray::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[120];

	LoadTestData(_T("TestSArray"), numbers, 120);
	for (i = 0; i < 120; ++i)
		list.Append(numbers + i);

	qsort(numbers, 120, sizeof(unsigned int), TestCompareSRand);

	list.Sort(TestSortFuncUInt);
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (DerefAnyPtr(unsigned int, *it) != numbers[i++])
			WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[i++]"));
		++it;
	}
	if (i != 120)
		WriteErrorTestFile(1, _T("i != 120"));
	it = list.Last();
	i = 120;
	while (it)
	{
		if (DerefAnyPtr(unsigned int, *it) != numbers[--i])
			WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[--i]"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		j = i;
		while (it)
		{
			if (DerefAnyPtr(unsigned int, *it) != numbers[j++])
				WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[j++]"));
			++it;
		}
		if (j != 120)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < 120; i += 2)
		list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL);
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if ((i % 2) == 0)
		{
			++it;
			if (DerefAnyPtr(unsigned int, *it) == numbers[i])
				WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) == numbers[i]"));
			if (DerefAnyPtr(unsigned int, *it) != numbers[i + 1])
				WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[i + 1]"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (DerefAnyPtr(unsigned int, *it) != numbers[j])
					WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[j]"));
				j += 2;
				++it;
			}
			if (j != 121)
				WriteErrorTestFile(1, _T("j != 121"));
		}
	}
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestSArrayT()
{
	OpenTestFile(_T("TestSArrayT"));

	CDataSArrayT<unsigned int> list(__FILE__LINE__ 120);
	CDataSArrayT<unsigned int>::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[120];

	LoadTestData(_T("TestSArrayT"), numbers, 120);
	for (i = 0; i < 120; ++i)
		list.Append(numbers + i);

	qsort(numbers, 120, sizeof(unsigned int), TestCompareSRand);

	list.Sort(TestSortFuncUInt);
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (**it != numbers[i++])
			WriteErrorTestFile(1, _T("**it != numbers[i++]"));
		++it;
	}
	if (i != 120)
		WriteErrorTestFile(1, _T("i != 120"));
	it = list.Last();
	i = 120;
	while (it)
	{
		if (**it != numbers[--i])
			WriteErrorTestFile(1, _T("**it != numbers[--i]"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		j = i;
		while (it)
		{
			if (**it != numbers[j++])
				WriteErrorTestFile(1, _T("**it != numbers[j++]"));
			++it;
		}
		if (j != 120)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < 120; i += 2)
		list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL);
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if ((i % 2) == 0)
		{
			++it;
			if (**it == numbers[i])
				WriteErrorTestFile(1, _T("**it == numbers[i]"));
			if (**it != numbers[i + 1])
				WriteErrorTestFile(1, _T("**it != numbers[i + 1]"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (**it != numbers[j])
					WriteErrorTestFile(1, _T("**it != numbers[j]"));
				j += 2;
				++it;
			}
			if (j != 121)
				WriteErrorTestFile(1, _T("j != 121"));
		}
	}
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestVector()
{
	OpenTestFile(_T("TestVector"));

	CDataVector list(__FILE__LINE__ 16, 16, TestDeleteFunc, NULL, TestSortUserFunc);
	CDataVector::Iterator it;
	dword i;
	dword j;
	ULongPointer numbers[120];

	LoadTestData64(_T("TestVector"), numbers, 120);
	for (i = 0; i < 120; ++i)
		list.Append((Pointer)(numbers[i]));

	qsort(numbers, 120, sizeof(ULongPointer), TestCompareSRand64);

	list.Sort();
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (!(*it == (Pointer)(numbers[i++])))
			WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[i++])"));
		++it;
	}
	if (i != 120)
		WriteErrorTestFile(1, _T("i != 120"));
	it = list.Last();
	i = 120;
	while (it)
	{
		if (!(*it == (Pointer)(numbers[--i])))
			WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[--i])"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted((Pointer)numbers[i]);
		j = i;
		while (it)
		{
			if (!(*it == (Pointer)(numbers[j++])))
				WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[j++])"));
			++it;
		}
		if (j != 120)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < 120; i += 2)
		list.RemoveSorted((Pointer)numbers[i]);
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted((Pointer)numbers[i]);
		if ((i % 2) == 0)
		{
			++it;
			if (*it == (Pointer)(numbers[i]))
				WriteErrorTestFile(1, _T("*it == (Pointer)(numbers[i])"));
			if (*it != (Pointer)(numbers[i + 1]))
				WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[i + 1])"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (!(*it == (Pointer)(numbers[j])))
					WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[j])"));
				j += 2;
				++it;
			}
			if (j != 121)
				WriteErrorTestFile(1, _T("j != 121"));
		}
	}

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestVectorT()
{
	OpenTestFile(_T("TestVectorT"));

	TestFuncUIntVector list(__FILE__LINE__ 16, 16);
	TestFuncUIntVector::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[120];

	LoadTestData(_T("TestVectorT"), numbers, 120);
	for (i = 0; i < 120; ++i)
		list.Append(numbers + i);

	qsort(numbers, 120, sizeof(unsigned int), TestCompareSRand);

	list.Sort();
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (**it != numbers[i++])
			WriteErrorTestFile(1, _T("**it != numbers[i++]"));
		++it;
	}
	if (i != 120)
		WriteErrorTestFile(1, _T("i != 120"));
	it = list.Last();
	i = 120;
	while (it)
	{
		if (**it != numbers[--i])
			WriteErrorTestFile(1, _T("**it != numbers[--i]"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i);
		j = i;
		while (it)
		{
			if (**it != numbers[j++])
				WriteErrorTestFile(1, _T("**it != numbers[j++]"));
			++it;
		}
		if (j != 120)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < 120; i += 2)
		list.RemoveSorted(numbers + i);
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i);
		if ((i % 2) == 0)
		{
			++it;
			if (**it == numbers[i])
				WriteErrorTestFile(1, _T("**it == numbers[i]"));
			if (**it != numbers[i + 1])
				WriteErrorTestFile(1, _T("**it != numbers[i + 1]"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (**it != numbers[j])
					WriteErrorTestFile(1, _T("**it != numbers[j]"));
				j += 2;
				++it;
			}
			if (j != 121)
				WriteErrorTestFile(1, _T("j != 121"));
		}
	}
	
	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestSVector()
{
	OpenTestFile(_T("TestSVector"));

	CDataSVector list(__FILE__LINE__ 16, 16, sizeof(unsigned int));
	CDataSVector::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[120];

	LoadTestData(_T("TestSVector"), numbers, 120);
	for (i = 0; i < 120; ++i)
		list.Append(numbers + i);

	qsort(numbers, 120, sizeof(unsigned int), TestCompareSRand);

	list.Sort(TestSortFuncUInt);
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (DerefAnyPtr(unsigned int, *it) != numbers[i++])
			WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[i++]"));
		++it;
	}
	if (i != 120)
		WriteErrorTestFile(1, _T("i != 120"));
	it = list.Last();
	i = 120;
	while (it)
	{
		if (DerefAnyPtr(unsigned int, *it) != numbers[--i])
			WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[--i]"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		j = i;
		while (it)
		{
			if (DerefAnyPtr(unsigned int, *it) != numbers[j++])
				WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[j++]"));
			++it;
		}
		if (j != 120)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < 120; i += 2)
		list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL);
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if ((i % 2) == 0)
		{
			++it;
			if (DerefAnyPtr(unsigned int, *it) == numbers[i])
				WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) == numbers[i]"));
			if (DerefAnyPtr(unsigned int, *it) != numbers[i + 1])
				WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[i + 1]"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (DerefAnyPtr(unsigned int, *it) != numbers[j])
					WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[j]"));
				j += 2;
				++it;
			}
			if (j != 121)
				WriteErrorTestFile(1, _T("j != 121"));
		}
	}
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestSVectorT()
{
	OpenTestFile(_T("TestSVectorT"));

	CDataSVectorT<unsigned int> list(__FILE__LINE__ 16, 16);
	CDataSVectorT<unsigned int>::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[120];

	LoadTestData(_T("TestSVectorT"), numbers, 120);
	for (i = 0; i < 120; ++i)
		list.Append(numbers + i);

	qsort(numbers, 120, sizeof(unsigned int), TestCompareSRand);

	list.Sort(TestSortFuncUInt);
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (**it != numbers[i++])
			WriteErrorTestFile(1, _T("**it != numbers[i++]"));
		++it;
	}
	if (i != 120)
		WriteErrorTestFile(1, _T("i != 120"));
	it = list.Last();
	i = 120;
	while (it)
	{
		if (**it != numbers[--i])
			WriteErrorTestFile(1, _T("**it != numbers[--i]"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		j = i;
		while (it)
		{
			if (**it != numbers[j++])
				WriteErrorTestFile(1, _T("**it != numbers[j++]"));
			++it;
		}
		if (j != 120)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < 120; i += 2)
		list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL);
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if ((i % 2) == 0)
		{
			++it;
			if (**it == numbers[i])
				WriteErrorTestFile(1, _T("**it == numbers[i]"));
			if (**it != numbers[i + 1])
				WriteErrorTestFile(1, _T("**it != numbers[i + 1]"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (**it != numbers[j])
					WriteErrorTestFile(1, _T("**it != numbers[j]"));
				j += 2;
				++it;
			}
			if (j != 121)
				WriteErrorTestFile(1, _T("j != 121"));
		}
	}
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestAVLBinaryTree()
{
	OpenTestFile(_T("TestAVLBinaryTree"));

	dword cnt = 1200;
	CDataAVLBinaryTree list __FILE__LINE__0P;
	CDataAVLBinaryTree::Iterator it;
	dword i;
	dword j;
	ULongPointer numbers[1200];
	ULongPointer number;

	LoadTestData64(_T("TestAVLBinaryTree"), numbers, cnt);
	for (i = 0; i < cnt; ++i)
	{
		//WriteTestFile(1, _T("InsertSorted numbers[i]=%lu"), numbers[i]);
		it = list.InsertSorted((Pointer)numbers[i], TestSortFunc);
		if (*it == NULL)
			WriteErrorTestFile(1, _T("*it == NULL"));
		if (!it)
			WriteErrorTestFile(1, _T("!it"));
		if (!(*it == (Pointer)(numbers[i])))
			WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[i])"));
		if ((i % 10) == 0)
			WriteTestFile(1, _T("InsertSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
		it = list.Begin();
		j = 0;
		while (it)
		{
			if (j > 0)
			{
				if (number > (ULongPointer)(*it))
					WriteErrorTestFile(1, _T("number > (ULongPointer)(*it)"));
			}
			number = (ULongPointer)(*it);
			// WriteTestFile(1, _T("*it=%lu"), *it);
			++it; ++j;
		}
	}

	qsort(numbers, cnt, sizeof(ULongPointer), TestCompareSRand64);

	WriteTestFile(1, _T("InsertSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (!(*it == (Pointer)(numbers[i++])))
			WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[i++])"));
		++it;
	}
	if (i != cnt)
		WriteErrorTestFile(1, _T("i != cnt"));
	it = list.Last();
	i = cnt;
	while (it)
	{
		if (!(*it == (Pointer)(numbers[--i])))
			WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[--i])"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < cnt; ++i)
	{
		it = list.FindSorted((Pointer)numbers[i], TestSortFunc);
		j = i;
		while (it)
		{
			if (!(*it == (Pointer)(numbers[j++])))
				WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[j++])"));
			++it;
		}
		if (j != cnt)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < cnt; i += 2)
	{
		if (!list.RemoveSorted((Pointer)numbers[i], TestSortFunc, TestDeleteFunc, NULL))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 10) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	for (i = 0; i < cnt; ++i)
	{
		it = list.FindSorted((Pointer)numbers[i], TestSortFunc);
		if ((i % 2) == 0)
		{
			if (*it != NULL)
				WriteErrorTestFile(1, _T("*it != NULL"));
			if (!!it)
				WriteErrorTestFile(1, _T("!!it"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (!(*it == (Pointer)(numbers[j])))
					WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[j])"));
				j += 2;
				++it;
			}
			if (j != (cnt + 1))
				WriteErrorTestFile(1, _T("j != (cnt + 1)"));
		}
	}
	for (i = 1; i < cnt; i += 2)
	{
		if (!list.RemoveSorted((Pointer)numbers[i], TestSortFunc, TestDeleteFunc, NULL))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 5) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestAVLBinaryTreeT()
{
	OpenTestFile(_T("TestAVLBinaryTreeT"));

	CDataAVLBinaryTreeT<unsigned int> list __FILE__LINE__0P;
	CDataAVLBinaryTreeT<unsigned int>::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[120];

	LoadTestData(_T("TestAVLBinaryTreeT"), numbers, 120);
	qsort(numbers, 120, sizeof(unsigned int), TestCompareSRand);

	for (i = 0; i < 120; ++i)
		list.InsertSorted(numbers + i, TestSortFuncUInt);

	WriteTestFile(1, _T("InsertSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (**it != numbers[i++])
			WriteErrorTestFile(1, _T("**it != numbers[i++]"));
		++it;
	}
	if (i != 120)
		WriteErrorTestFile(1, _T("i != 120"));
	it = list.Last();
	i = 120;
	while (it)
	{
		if (**it != numbers[--i])
			WriteErrorTestFile(1, _T("**it != numbers[--i]"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		j = i;
		while (it)
		{
			if (**it != numbers[j++])
				WriteErrorTestFile(1, _T("**it != numbers[j++]"));
			++it;
		}
		if (j != 120)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < 120; i += 2)
	{
		if (!list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 10) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if ((i % 2) == 0)
		{
			if (*it != NULL)
				WriteErrorTestFile(1, _T("*it != NULL"));
			if (!!it)
				WriteErrorTestFile(1, _T("!!it"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (**it != numbers[j])
					WriteErrorTestFile(1, _T("**it != numbers[j]"));
				j += 2;
				++it;
			}
			if (j != 121)
				WriteErrorTestFile(1, _T("j != 121"));
		}
	}
	for (i = 1; i < 120; i += 2)
	{
		if (!list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 5) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestSAVLBinaryTree()
{
	OpenTestFile(_T("TestSAVLBinaryTree"));

	dword cnt = 1200;
	CDataSAVLBinaryTree list(__FILE__LINE__ sizeof(unsigned int));
	CDataSAVLBinaryTree::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[1200];

	LoadTestData(_T("TestSAVLBinaryTree"), numbers, cnt);
	for (i = 0; i < cnt; ++i)
		list.InsertSorted(numbers + i, TestSortFuncUInt);

	qsort(numbers, cnt, sizeof(unsigned int), TestCompareSRand);

	WriteTestFile(1, _T("InsertSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (DerefAnyPtr(unsigned int, *it) != numbers[i++])
			WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[i++]"));
		++it;
	}
	if (i != cnt)
		WriteErrorTestFile(1, _T("i != cnt"));
	it = list.Last();
	i = cnt;
	while (it)
	{
		if (DerefAnyPtr(unsigned int, *it) != numbers[--i])
			WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[--i]"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < cnt; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		j = i;
		while (it)
		{
			if (DerefAnyPtr(unsigned int, *it) != numbers[j++])
				WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[j++]"));
			++it;
		}
		if (j != cnt)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < cnt; i += 2)
	{
		if (!list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 10) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	for (i = 0; i < cnt; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if ((i % 2) == 0)
		{
			if (*it != NULL)
				WriteErrorTestFile(1, _T("*it != NULL"));
			if (!!it)
				WriteErrorTestFile(1, _T("!!it"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (DerefAnyPtr(unsigned int, *it) != numbers[j])
					WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[j]"));
				j += 2;
				++it;
			}
			if (j != (cnt + 1))
				WriteErrorTestFile(1, _T("j != (cnt + 1)"));
		}
	}
	for (i = 1; i < cnt; i += 2)
	{
		if (!list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 5) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestSAVLBinaryTreeT()
{
	OpenTestFile(_T("TestSAVLBinaryTreeT"));

	CDataSAVLBinaryTreeT<unsigned int> list __FILE__LINE__0P;
	CDataSAVLBinaryTreeT<unsigned int>::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[120];

	LoadTestData(_T("TestSAVLBinaryTreeT"), numbers, 120);
	for (i = 0; i < 120; ++i)
		list.InsertSorted(numbers + i, TestSortFuncUInt);

	qsort(numbers, 120, sizeof(unsigned int), TestCompareSRand);

	WriteTestFile(1, _T("InsertSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (**it != numbers[i++])
			WriteErrorTestFile(1, _T("**it != numbers[i++]"));
		++it;
	}
	if (i != 120)
		WriteErrorTestFile(1, _T("i != 120"));
	it = list.Last();
	i = 120;
	while (it)
	{
		if (**it != numbers[--i])
			WriteErrorTestFile(1, _T("**it != numbers[--i]"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		j = i;
		while (it)
		{
			if (**it != numbers[j++])
				WriteErrorTestFile(1, _T("**it != numbers[j++]"));
			++it;
		}
		if (j != 120)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < 120; i += 2)
	{
		if (!list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 10) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if ((i % 2) == 0)
		{
			if (*it != NULL)
				WriteErrorTestFile(1, _T("*it != NULL"));
			if (!!it)
				WriteErrorTestFile(1, _T("!!it"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (**it != numbers[j])
					WriteErrorTestFile(1, _T("**it != numbers[j]"));
				j += 2;
				++it;
			}
			if (j != 121)
				WriteErrorTestFile(1, _T("j != 121"));
		}
	}
	for (i = 1; i < 120; i += 2)
	{
		if (!list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 5) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestRBBinaryTree()
{
	OpenTestFile(_T("TestRBBinaryTree"));

	dword cnt = 1200;
	CDataRBBinaryTree list __FILE__LINE__0P;
	CDataRBBinaryTree::Iterator it;
	dword i;
	dword j;
	ULongPointer numbers[1200];
	ULongPointer number;

	LoadTestData64(_T("TestRBBinaryTree"), numbers, cnt);
	for (i = 0; i < cnt; ++i)
	{
		//WriteTestFile(1, _T("InsertSorted numbers[i]=%lu"), numbers[i]);
		it = list.InsertSorted((Pointer)numbers[i], TestSortFunc);
		if (*it == NULL)
			WriteErrorTestFile(1, _T("*it == NULL"));
		if (!it)
			WriteErrorTestFile(1, _T("!it"));
		if (!(*it == (Pointer)(numbers[i])))
			WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[i])"));
		if ((i % 10) == 0)
			WriteTestFile(1, _T("InsertSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
		it = list.Begin();
		j = 0;
		while (it)
		{
			if (j > 0)
			{
				if (number > (ULongPointer)(*it))
					WriteErrorTestFile(1, _T("number > (ULongPointer)(*it)"));
			}
			number = (ULongPointer)(*it);
			// WriteTestFile(1, _T("*it=%lu"), *it);
			++it; ++j;
		}
	}

	qsort(numbers, cnt, sizeof(ULongPointer), TestCompareSRand64);

	WriteTestFile(1, _T("InsertSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (!(*it == (Pointer)(numbers[i++])))
			WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[i++])"));
		++it;
	}
	if (i != cnt)
		WriteErrorTestFile(1, _T("i != cnt"));
	it = list.Last();
	i = cnt;
	while (it)
	{
		if (!(*it == (Pointer)(numbers[--i])))
			WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[--i])"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < cnt; ++i)
	{
		it = list.FindSorted((Pointer)numbers[i], TestSortFunc);
		j = i;
		while (it)
		{
			if (!(*it == (Pointer)(numbers[j++])))
				WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[j++])"));
			++it;
		}
		if (j != cnt)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < cnt; i += 2)
	{
		if (!list.RemoveSorted((Pointer)numbers[i], TestSortFunc, TestDeleteFunc, NULL))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 10) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	for (i = 0; i < cnt; ++i)
	{
		it = list.FindSorted((Pointer)numbers[i], TestSortFunc);
		if ((i % 2) == 0)
		{
			if (*it != NULL)
				WriteErrorTestFile(1, _T("*it != NULL"));
			if (!!it)
				WriteErrorTestFile(1, _T("!!it"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (!(*it == (Pointer)(numbers[j])))
					WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[j])"));
				j += 2;
				++it;
			}
			if (j != (cnt + 1))
				WriteErrorTestFile(1, _T("j != (cnt + 1)"));
		}
	}
	for (i = 1; i < cnt; i += 2)
	{
		if (!list.RemoveSorted((Pointer)numbers[i], TestSortFunc, TestDeleteFunc, NULL))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 5) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestRBBinaryTreeT()
{
	OpenTestFile(_T("TestRBBinaryTreeT"));

	CDataRBBinaryTreeT<unsigned int> list __FILE__LINE__0P;
	CDataRBBinaryTreeT<unsigned int>::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[120];

	LoadTestData(_T("TestRBBinaryTreeT"), numbers, 120);
	qsort(numbers, 120, sizeof(unsigned int), TestCompareSRand);

	for (i = 0; i < 120; ++i)
		list.InsertSorted(numbers + i, TestSortFuncUInt);

	WriteTestFile(1, _T("InsertSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (**it != numbers[i++])
			WriteErrorTestFile(1, _T("**it != numbers[i++]"));
		++it;
	}
	if (i != 120)
		WriteErrorTestFile(1, _T("i != 120"));
	it = list.Last();
	i = 120;
	while (it)
	{
		if (**it != numbers[--i])
			WriteErrorTestFile(1, _T("**it != numbers[--i]"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		j = i;
		while (it)
		{
			if (**it != numbers[j++])
				WriteErrorTestFile(1, _T("**it != numbers[j++]"));
			++it;
		}
		if (j != 120)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < 120; i += 2)
	{
		if (!list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 10) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if ((i % 2) == 0)
		{
			if (*it != NULL)
				WriteErrorTestFile(1, _T("*it != NULL"));
			if (!!it)
				WriteErrorTestFile(1, _T("!!it"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (**it != numbers[j])
					WriteErrorTestFile(1, _T("**it != numbers[j]"));
				j += 2;
				++it;
			}
			if (j != 121)
				WriteErrorTestFile(1, _T("j != 121"));
		}
	}
	for (i = 1; i < 120; i += 2)
	{
		if (!list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 5) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestSRBBinaryTree()
{
	OpenTestFile(_T("TestSRBBinaryTree"));

	dword cnt = 1200;
	CDataSRBBinaryTree list(__FILE__LINE__ sizeof(unsigned int));
	CDataSRBBinaryTree::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[1200];

	LoadTestData(_T("TestSRBBinaryTree"), numbers, cnt);
	for (i = 0; i < cnt; ++i)
		list.InsertSorted(numbers + i, TestSortFuncUInt);

	qsort(numbers, cnt, sizeof(unsigned int), TestCompareSRand);

	WriteTestFile(1, _T("InsertSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (DerefAnyPtr(unsigned int, *it) != numbers[i++])
			WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[i++]"));
		++it;
	}
	if (i != cnt)
		WriteErrorTestFile(1, _T("i != cnt"));
	it = list.Last();
	i = cnt;
	while (it)
	{
		if (DerefAnyPtr(unsigned int, *it) != numbers[--i])
			WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[--i]"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < cnt; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		j = i;
		while (it)
		{
			if (DerefAnyPtr(unsigned int, *it) != numbers[j++])
				WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[j++]"));
			++it;
		}
		if (j != cnt)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < cnt; i += 2)
	{
		if (!list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 10) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	for (i = 0; i < cnt; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if ((i % 2) == 0)
		{
			if (*it != NULL)
				WriteErrorTestFile(1, _T("*it != NULL"));
			if (!!it)
				WriteErrorTestFile(1, _T("!!it"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (DerefAnyPtr(unsigned int, *it) != numbers[j])
					WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[j]"));
				j += 2;
				++it;
			}
			if (j != (cnt + 1))
				WriteErrorTestFile(1, _T("j != (cnt + 1)"));
		}
	}
	for (i = 1; i < cnt; i += 2)
	{
		if (!list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 5) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestSRBBinaryTreeT()
{
	OpenTestFile(_T("TestSRBBinaryTreeT"));

	CDataSRBBinaryTreeT<unsigned int> list __FILE__LINE__0P;
	CDataSRBBinaryTreeT<unsigned int>::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[120];

	LoadTestData(_T("TestSRBBinaryTreeT"), numbers, 120);
	for (i = 0; i < 120; ++i)
		list.InsertSorted(numbers + i, TestSortFuncUInt);

	qsort(numbers, 120, sizeof(unsigned int), TestCompareSRand);

	WriteTestFile(1, _T("InsertSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	it = list.Begin();
	i = 0;
	while (it)
	{
		if (**it != numbers[i++])
			WriteErrorTestFile(1, _T("**it != numbers[i++]"));
		++it;
	}
	if (i != 120)
		WriteErrorTestFile(1, _T("i != 120"));
	it = list.Last();
	i = 120;
	while (it)
	{
		if (**it != numbers[--i])
			WriteErrorTestFile(1, _T("**it != numbers[--i]"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		j = i;
		while (it)
		{
			if (**it != numbers[j++])
				WriteErrorTestFile(1, _T("**it != numbers[j++]"));
			++it;
		}
		if (j != 120)
			WriteErrorTestFile(1, _T("j != 120"));
	}
	for (i = 0; i < 120; i += 2)
	{
		if (!list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 10) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if ((i % 2) == 0)
		{
			if (*it != NULL)
				WriteErrorTestFile(1, _T("*it != NULL"));
			if (!!it)
				WriteErrorTestFile(1, _T("!!it"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (**it != numbers[j])
					WriteErrorTestFile(1, _T("**it != numbers[j]"));
				j += 2;
				++it;
			}
			if (j != 121)
				WriteErrorTestFile(1, _T("j != 121"));
		}
	}
	for (i = 1; i < 120; i += 2)
	{
		if (!list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 5) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestBTree()
{
	OpenTestFile(_T("TestBTree"));

	CDataBTree list(__FILE__LINE__ 5);
	CDataBTree::Iterator it;
	dword i;
	dword j;
	ULongPointer numbers[630];

	LoadTestData64(_T("TestBTree"), numbers, 630);
	for (i = 0; i < 630; ++i)
	{
		if ( (i % 100) == 0 )
			WriteTestFile(1, _T("Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
		it = list.InsertSorted((Pointer)numbers[i], TestSortFunc);
		if (*it != (Pointer)(numbers[i]))
			WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[i])"));
	}
	WriteTestFile(1, _T("Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());

	qsort(numbers, 630, sizeof(ULongPointer), TestCompareSRand64);

	it = list.Begin();
	i = 0;
	while (it)
	{
		if (!(*it == (Pointer)(numbers[i++])))
			WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[i++])"));
		++it;
	}
	if (i != 630)
		WriteErrorTestFile(1, _T("i != 630"));
	it = list.Last();
	i = 630;
	while (it)
	{
		if (!(*it == (Pointer)(numbers[--i])))
			WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[--i])"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 630; ++i)
		if (!(list.RemoveSorted((Pointer)numbers[i], TestSortFunc, TestDeleteFunc, NULL)))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
	for (i = 1; i < 630; i += 2)
	{
		it = list.InsertSorted((Pointer)numbers[i], TestSortFunc);
		if (!(*it == (Pointer)(numbers[i])))
			WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[i])"));
	}
	for (i = 0; i < 630; ++i)
	{
		it = list.FindSorted((Pointer)numbers[i], TestSortFunc);
		if ((i % 2) == 0)
		{
			if (*it != NULL)
				WriteErrorTestFile(1, _T("*it != NULL"));
			if (!!it)
				WriteErrorTestFile(1, _T("!!it"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (!(*it == (Pointer)(numbers[j])))
					WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[j])"));
				j += 2;
				++it;
			}
			if (j != 631)
				WriteErrorTestFile(1, _T("j != 631"));
		}
	}
	for (i = 1; i < 630; i += 2)
	{
		it = list.FindSorted((Pointer)numbers[i], TestSortFunc);
		if (!(*it == (Pointer)(numbers[i])))
			WriteErrorTestFile(1, _T("*it != (Pointer)(numbers[i])"));
		list.Remove(it, TestDeleteFunc, NULL);
	}
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestBTreeT()
{
	OpenTestFile(_T("TestBTreeT"));

	CDataBTreeT<unsigned int> list(__FILE__LINE__ 5);
	CDataBTreeT<unsigned int>::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[630];

	LoadTestData(_T("TestBTreeT"), numbers, 630);
	qsort(numbers, 630, sizeof(unsigned int), TestCompareSRand);

	for (i = 0; i < 630; ++i)
	{
		if ((i % 100) == 0)
			WriteTestFile(1, _T("Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
		it = list.InsertSorted(numbers + i, TestSortFuncUInt);
		if (**it != numbers[i])
			WriteErrorTestFile(1, _T("**it != numbers[i]"));
	}
	WriteTestFile(1, _T("Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());

	it = list.Begin();
	i = 0;
	while (it)
	{
		if (**it != numbers[i++])
			WriteErrorTestFile(1, _T("**it != numbers[i++]"));
		++it;
	}
	if (i != 630)
		WriteErrorTestFile(1, _T("i != 630"));
	it = list.Last();
	i = 630;
	while (it)
	{
		if (**it != numbers[--i])
			WriteErrorTestFile(1, _T("**it != numbers[--i]"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 630; ++i)
	if (!(list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL)))
		WriteErrorTestFile(1, _T("!RemoveSorted"));
	for (i = 1; i < 630; i += 2)
	{
		it = list.InsertSorted(numbers + i, TestSortFuncUInt);
		if (**it != numbers[i])
			WriteErrorTestFile(1, _T("**it != numbers[i]"));
	}
	for (i = 0; i < 630; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if ((i % 2) == 0)
		{
			if (*it != NULL)
				WriteErrorTestFile(1, _T("*it != NULL"));
			if (!!it)
				WriteErrorTestFile(1, _T("!!it"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (!(**it == numbers[j]))
					WriteErrorTestFile(1, _T("**it != numbers[j]"));
				j += 2;
				++it;
			}
			if (j != 631)
				WriteErrorTestFile(1, _T("j != 631"));
		}
	}
	for (i = 1; i < 630; i += 2)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if (**it != numbers[i])
			WriteErrorTestFile(1, _T("**it != numbers[i]"));
		list.Remove(it, TestDeleteFunc, NULL);
	}
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestSBTree()
{
	OpenTestFile(_T("TestSBTree"));

	CDataSBTree list(__FILE__LINE__ 5, sizeof(unsigned int));
	CDataSBTree::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[630];

	LoadTestData(_T("TestSBTree"), numbers, 630);
	for (i = 0; i < 630; ++i)
	{
		if ((i % 100) == 0)
			WriteTestFile(1, _T("Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
		it = list.InsertSorted(numbers + i, TestSortFuncUInt);
		if (DerefAnyPtr(unsigned int, *it) != numbers[i])
			WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[i]"));
	}
	WriteTestFile(1, _T("Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());

	qsort(numbers, 630, sizeof(unsigned int), TestCompareSRand);

	it = list.Begin();
	i = 0;
	while (it)
	{
		if (DerefAnyPtr(unsigned int, *it) != numbers[i++])
			WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[i++]"));
		++it;
	}
	if (i != 630)
		WriteErrorTestFile(1, _T("i != 630"));
	it = list.Last();
	i = 630;
	while (it)
	{
		if (DerefAnyPtr(unsigned int, *it) != numbers[--i])
			WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[--i]"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 630; ++i)
	if (!(list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL)))
		WriteErrorTestFile(1, _T("!RemoveSorted"));
	for (i = 1; i < 630; i += 2)
	{
		it = list.InsertSorted(numbers + i, TestSortFuncUInt);
		if (DerefAnyPtr(unsigned int, *it) != numbers[i])
			WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[i]"));
	}
	for (i = 0; i < 630; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if ((i % 2) == 0)
		{
			if (*it != NULL)
				WriteErrorTestFile(1, _T("*it != NULL"));
			if (!!it)
				WriteErrorTestFile(1, _T("!!it"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (DerefAnyPtr(unsigned int, *it) != numbers[j])
					WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[j]"));
				j += 2;
				++it;
			}
			if (j != 631)
				WriteErrorTestFile(1, _T("j != 631"));
		}
	}
	for (i = 1; i < 630; i += 2)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if (DerefAnyPtr(unsigned int, *it) != numbers[i])
			WriteErrorTestFile(1, _T("DerefAnyPtr(unsigned int, *it) != numbers[i]"));
		list.Remove(it, TestDeleteFunc, NULL);
	}
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestSBTreeT()
{
	OpenTestFile(_T("TestSBTreeT"));

	CDataSBTreeT<unsigned int> list(__FILE__LINE__ 5);
	CDataSBTreeT<unsigned int>::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[630];

	LoadTestData(_T("TestSBTreeT"), numbers, 630);
	for (i = 0; i < 630; ++i)
	{
		if ((i % 100) == 0)
			WriteTestFile(1, _T("Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
		it = list.InsertSorted(numbers + i, TestSortFuncUInt);
		if (**it != numbers[i])
			WriteErrorTestFile(1, _T("**it != numbers[i]"));
	}
	WriteTestFile(1, _T("Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());

	qsort(numbers, 630, sizeof(unsigned int), TestCompareSRand);

	it = list.Begin();
	i = 0;
	while (it)
	{
		if (**it != numbers[i++])
			WriteErrorTestFile(1, _T("**it != numbers[i++]"));
		++it;
	}
	if (i != 630)
		WriteErrorTestFile(1, _T("i != 630"));
	it = list.Last();
	i = 630;
	while (it)
	{
		if (**it != numbers[--i])
			WriteErrorTestFile(1, _T("**it != numbers[--i]"));
		--it;
	}
	if (i != 0)
		WriteErrorTestFile(1, _T("i != 0"));
	for (i = 0; i < 630; ++i)
	if (!(list.RemoveSorted(numbers + i, TestSortFuncUInt, TestDeleteFunc, NULL)))
		WriteErrorTestFile(1, _T("!RemoveSorted"));
	for (i = 1; i < 630; i += 2)
	{
		it = list.InsertSorted(numbers + i, TestSortFuncUInt);
		if (**it != numbers[i])
			WriteErrorTestFile(1, _T("**it != numbers[i]"));
	}
	for (i = 0; i < 630; ++i)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if ((i % 2) == 0)
		{
			if (*it != NULL)
				WriteErrorTestFile(1, _T("*it != NULL"));
			if (!!it)
				WriteErrorTestFile(1, _T("!!it"));
		}
		else
		{
			j = i;
			while (it)
			{
				if (!(**it == numbers[j]))
					WriteErrorTestFile(1, _T("**it != numbers[j]"));
				j += 2;
				++it;
			}
			if (j != 631)
				WriteErrorTestFile(1, _T("j != 631"));
		}
	}
	for (i = 1; i < 630; i += 2)
	{
		it = list.FindSorted(numbers + i, TestSortFuncUInt);
		if (**it != numbers[i])
			WriteErrorTestFile(1, _T("**it != numbers[i]"));
		list.Remove(it, TestDeleteFunc, NULL);
	}
	list.Close(TestDeleteFunc, NULL);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

void TestDataStructures()
{
	COUT << _T("********************** TestFlagRegister **********************") << endl;
	TestFlagRegister();
	COUT << _T("********************** TestCBinaryTreeT ***********************") << endl;
	TestCBinaryTreeT();
	COUT << _T("********************** TestCBTreeT *****************************") << endl;
	TestCBTreeT();
	COUT << _T("********************** TestLinkedListT ***********************") << endl;
	TestLinkedListT();
	COUT << _T("********************** TestHashLinkedListT *******************") << endl;
	TestHashLinkedListT();
	COUT << _T("********************** TestHashLinearExploreT ****************") << endl;
	TestHashLinearExploreT();
	COUT << _T("********************** TestDoubleLinkedList ******************") << endl;
	TestDoubleLinkedList();
	COUT << _T("********************** TestDoubleLinkedListT ******************") << endl;
	TestDoubleLinkedListT();
	COUT << _T("********************** TestSDoubleLinkedList *****************") << endl;
	TestSDoubleLinkedList();
	COUT << _T("********************** TestSDoubleLinkedListT *****************") << endl;
	TestSDoubleLinkedListT();
	COUT << _T("********************** TestArray *****************************") << endl;
	TestArray();
	COUT << _T("********************** TestArrayT *****************************") << endl;
	TestArrayT();
	COUT << _T("********************** TestSArray *****************************") << endl;
	TestSArray();
	COUT << _T("********************** TestSArrayT *****************************") << endl;
	TestSArrayT();
	COUT << _T("********************** TestVector ****************************") << endl;
	TestVector();
	COUT << _T("********************** TestVectorT ****************************") << endl;
	TestVectorT();
	COUT << _T("********************** TestSVector ****************************") << endl;
	TestSVector();
	COUT << _T("********************** TestSVectorT ****************************") << endl;
	TestSVectorT();
	COUT << _T("********************** TestAVLBinaryTree ************************") << endl;
	TestAVLBinaryTree();
	COUT << _T("********************** TestAVLBinaryTreeT ************************") << endl;
	TestAVLBinaryTreeT();
	COUT << _T("********************** TestSAVLBinaryTree ************************") << endl;
	TestSAVLBinaryTree();
	COUT << _T("********************** TestSAVLBinaryTreeT ************************") << endl;
	TestSAVLBinaryTreeT();
	COUT << _T("********************** TestRBBinaryTree ************************") << endl;
	TestRBBinaryTree();
	COUT << _T("********************** TestRBBinaryTreeT ************************") << endl;
	TestRBBinaryTreeT();
	COUT << _T("********************** TestSRBBinaryTree ************************") << endl;
	TestSRBBinaryTree();
	COUT << _T("********************** TestSRBBinaryTreeT ************************") << endl;
	TestSRBBinaryTreeT();
	COUT << _T("********************** TestBTree *****************************") << endl;
	TestBTree();
	COUT << _T("********************** TestBTreeT *****************************") << endl;
	TestBTreeT();
	COUT << _T("********************** TestSBTree ****************************") << endl;
	TestSBTree();
	COUT << _T("********************** TestSBTreeT ****************************") << endl;
	TestSBTreeT();
}
