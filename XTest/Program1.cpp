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

static void TestDoubleLinkedListT()
{
	OpenTestFile(_T("TestDoubleLinkedListT"));

	TestFuncUIntDoubleLinkedList list __FILE__LINE__0P;
	TestFuncUIntDoubleLinkedList::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[120];

	LoadTestData(_T("TestDoubleLinkedListT"), numbers, 120);
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
	list.Close();

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestArrayT()
{
	OpenTestFile(_T("TestArrayT"));

	TestFuncUIntArray list(__FILE__LINE__ 120);
	TestFuncUIntArray::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[120];

	LoadTestData(_T("TestArrayT"), numbers, 120);
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
	list.Close();

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

static void TestAVLBinaryTreeT()
{
	OpenTestFile(_T("TestAVLBinaryTreeT"));

	TestFuncUIntAVLBinaryTree list __FILE__LINE__0P;
	TestFuncUIntAVLBinaryTree::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[120];

	LoadTestData(_T("TestAVLBinaryTreeT"), numbers, 120);
	qsort(numbers, 120, sizeof(unsigned int), TestCompareSRand);

	for (i = 0; i < 120; ++i)
		list.InsertSorted(numbers + i);

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
	{
		if (!list.RemoveSorted(numbers + i))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 10) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i);
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
		if (!list.RemoveSorted(numbers + i))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 5) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	list.Close();

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestRBBinaryTreeT()
{
	OpenTestFile(_T("TestRBBinaryTreeT"));

	TestFuncUIntRBBinaryTree list __FILE__LINE__0P;
	TestFuncUIntRBBinaryTree::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[120];

	LoadTestData(_T("TestRBBinaryTreeT"), numbers, 120);
	qsort(numbers, 120, sizeof(unsigned int), TestCompareSRand);

	for (i = 0; i < 120; ++i)
		list.InsertSorted(numbers + i);

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
	{
		if (!list.RemoveSorted(numbers + i))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 10) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i);
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
		if (!list.RemoveSorted(numbers + i))
			WriteErrorTestFile(1, _T("!RemoveSorted"));
		if ((i % 5) == 0)
			WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	}
	WriteTestFile(1, _T("RemoveSorted: Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
	list.Close();

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestBTreeT()
{
	OpenTestFile(_T("TestBTreeT"));

	TestFuncUIntBTree list(__FILE__LINE__ 5);
	TestFuncUIntBTree::Iterator it;
	dword i;
	dword j;
	unsigned int numbers[630];

	LoadTestData(_T("TestBTreeT"), numbers, 630);
	qsort(numbers, 630, sizeof(unsigned int), TestCompareSRand);

	for (i = 0; i < 630; ++i)
	{
		if ((i % 100) == 0)
			WriteTestFile(1, _T("Height of Tree = %d, Count of nodes = %d"), list.Height(), list.Count());
		it = list.InsertSorted(numbers + i);
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
	if (!(list.RemoveSorted(numbers + i)))
		WriteErrorTestFile(1, _T("!RemoveSorted"));
	for (i = 1; i < 630; i += 2)
	{
		it = list.InsertSorted(numbers + i);
		if (**it != numbers[i])
			WriteErrorTestFile(1, _T("**it != numbers[i]"));
	}
	for (i = 0; i < 630; ++i)
	{
		it = list.FindSorted(numbers + i);
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
		it = list.FindSorted(numbers + i);
		if (**it != numbers[i])
			WriteErrorTestFile(1, _T("**it != numbers[i]"));
		list.Remove(it);
	}
	list.Close();

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
	COUT << _T("********************** TestDoubleLinkedListT ******************") << endl;
	TestDoubleLinkedListT();
	COUT << _T("********************** TestArrayT *****************************") << endl;
	TestArrayT();
	COUT << _T("********************** TestVectorT ****************************") << endl;
	TestVectorT();
	COUT << _T("********************** TestAVLBinaryTreeT ************************") << endl;
	TestAVLBinaryTreeT();
	COUT << _T("********************** TestRBBinaryTreeT ************************") << endl;
	TestRBBinaryTreeT();
	COUT << _T("********************** TestBTreeT *****************************") << endl;
	TestBTreeT();
}
