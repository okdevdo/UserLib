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
#include "DataHashtable.h"
#include "DataGraph.h"
#include "DataDoubleLinkedList.h"
#include "DataArray.h"
#include "DataBinaryTree.h"
#include "DataBTree.h"
#include <search.h>

class TestFuncUIntLessFunctor
{
public:
	bool operator()(ConstPtr(unsigned int) r1, ConstPtr(unsigned int) r2) const
	{
		return *r1 < *r2;
	}
};

class TestFuncUIntHashFunctor
{
public:
	TestFuncUIntHashFunctor(sdword max) : hd(max) {}

	sdword operator()(ConstPtr(unsigned int) p1) const
	{
		return hd(*p1);
	}

private:
	HashFunctorDigit hd;
};

typedef CDataVectorT<unsigned int, TestFuncUIntLessFunctor, CCppObjectNullFunctor<unsigned int> > TestFuncUIntVector;
typedef CDataDoubleLinkedListT<unsigned int, TestFuncUIntLessFunctor, CCppObjectNullFunctor<unsigned int> > TestFuncUIntDoubleLinkedList;
typedef CDataArrayT<unsigned int, TestFuncUIntLessFunctor, CCppObjectNullFunctor<unsigned int> > TestFuncUIntArray;
typedef CDataAVLBinaryTreeT<unsigned int, TestFuncUIntLessFunctor, CCppObjectNullFunctor<unsigned int> > TestFuncUIntAVLBinaryTree;
typedef CDataRBBinaryTreeT<unsigned int, TestFuncUIntLessFunctor, CCppObjectNullFunctor<unsigned int> > TestFuncUIntRBBinaryTree;
typedef CDataBTreeT<unsigned int, TestFuncUIntLessFunctor, CCppObjectNullFunctor<unsigned int> > TestFuncUIntBTree;
typedef CDataHashLinkedListT<unsigned int, TestFuncUIntHashFunctor, TestFuncUIntLessFunctor, CCppObjectNullFunctor<unsigned int> > TestFuncUIntHashLinkedList;

static int bsearch_cb(Pointer context, ConstPointer key, ConstPointer arrayitem)
{
	ConstPtr(unsigned int) pkey = CastAnyConstPtr(unsigned int, key);
	ConstPtr(unsigned int) parrayitem = CastAnyConstPtr(unsigned int, arrayitem);

	if (*pkey > *parrayitem)
		return 1;
	if (*pkey < *parrayitem)
		return -1;
	return 0;
}

static int __cdecl TestCompareSRand(const void * pA, const void * pB)
{
	unsigned int* piA = (unsigned int*)pA;
	unsigned int* piB = (unsigned int*)pB;

	if (*piA < *piB)
		return -1;
	if (*piA > *piB)
		return 1;
	return 0;
}

static void LoadTestData(CConstPointer _TestFunction, unsigned int *numbers, unsigned int cnt)
{
	CCppObjectPtr<CFile> _DataFile;

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
			srand((unsigned)time(nullptr));
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
					WriteErrorTestFile(__FILE__, __LINE__, 1, _T("rand_s failed"));
				numbers[i] = number;
#endif
				_DataFile->Write(_T("%u\r\n"), number);
			}
		}
		_DataFile->Close();
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
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

	ASSERTTESTFILE(1, (i == 0), _T("Start of loop"));
	while (i != CFlagRegister::notfound)
	{
		i = r2.findnext(true, i + 1);
		if (i != CFlagRegister::notfound)
		{
			++ix;
			ASSERTTESTFILE(1, (Castdword(i % 16) == 0), _T("Inside loop"));
			ASSERTTESTFILE(1, (Castdword(i / 16) == Castdword(ix)), _T("Inside loop"));
		}
	}
	ASSERTTESTFILE(1, (ix == 63), _T("End of loop"));
	TFfree(p1);
	TFfree(p2);
	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestHashLinkedListT()
{
	OpenTestFile(_T("TestHashLinkedListT"));

	TestFuncUIntHashLinkedList list(__FILE__LINE__ 100, TestFuncUIntHashFunctor(100));
	TestFuncUIntHashLinkedList::Iterator it;
	int i;
	unsigned int numbers[120];

	LoadTestData(_T("TestHashLinkedListT"), numbers, 120);
	for (i = 0; i < 120; ++i)
	{
		it = list.InsertSorted(numbers + i);
		ASSERTTESTFILE(1, (it), _T("Test iterator"));
		if (it)
			ASSERTTESTFILE(1, (**it == numbers[i]), _T("Test iterator value"));
	}
	it = list.Begin();
	i = 0;
	while (it)
	{
		ASSERTTESTFILE(1, (it == list.Index(i)), _T("Test index, forward loop"));
		++i;
		++it;
	}
	ASSERTTESTFILE(1, (i == 120), _T("End of loop"));
	it = list.Last();
	i = 120;
	while (it)
	{
		--i;
		ASSERTTESTFILE(1, (it == list.Index(i)), _T("Test index, backward loop"));
		--it;
	}
	ASSERTTESTFILE(1, (i == 0), _T("End of loop"));
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i);
		ASSERTTESTFILE(1, (it), _T("Test iterator"));
		if (it)
			ASSERTTESTFILE(1, (**it == numbers[i]), _T("Test iterator value"));
	}
	for (i = 0; i < 120; i += 2)
		ASSERTTESTFILE(1, (list.RemoveSorted(numbers + i)), _T("Test RemoveSorted"));
	for (i = 0; i < 120; ++i)
	{
		it = list.FindSorted(numbers + i);
		if ((i % 2) == 0)
		{
			ASSERTTESTFILE(1, (!it), _T("Test iterator"));
		}
		else
		{
			ASSERTTESTFILE(1, (it), _T("Test iterator"));
			if (it)
				ASSERTTESTFILE(1, (**it == numbers[i]), _T("Test iterator value"));
		}
	}
	list.Close();

	WriteSuccessTestFile(1);

	CloseTestFile();
}

template <typename TListe>
static void TestIterateBeginNext(Ref(TListe) list, unsigned int* numbers, dword k)
{
	TListe::Iterator it;
	dword j;

	it = list.Begin();
	j = 0;
	while (it)
	{
		ASSERTTESTFILE(1, (**it == numbers[j]), _T("Test iterator value"));
		++j;
		++it;
	}
	ASSERTTESTFILE(1, (k == j), _T("End of loop"));
	it = list.Last();
	j = k;
	while (it)
	{
		--j;
		ASSERTTESTFILE(1, (**it == numbers[j]), _T("Test iterator value"));
		--it;
	}
	ASSERTTESTFILE(1, (j == 0), _T("End of loop"));
}

template <typename TListe>
static void TestIterateForEachX(Ref(TListe) list, unsigned int* numbers, dword k)
{
	TListe::Iterator it;
	dword j;

	j = k;
	list.ForEach([&j, numbers](Ptr(unsigned int) p) -> bool {
		--j;
		ASSERTTESTFILE(1, (*p == numbers[j]), _T("Test iterator value"));
		return true;
	});
	ASSERTTESTFILE(1, (j == 0), _T("End of loop"));
	j = 0;
	list.ForEach([&j, numbers](Ptr(unsigned int) p) -> bool {
		ASSERTTESTFILE(1, (*p == numbers[j]), _T("Test iterator value"));
		++j;
		return true;
	}, true);
	ASSERTTESTFILE(1, (k == j), _T("End of loop"));
}

template <typename TListe>
static void TestIterateForEach(Ref(TListe) list, unsigned int* numbers1, dword k)
{
	TListe::Iterator it;
	dword j;

	j = 0;
	list.ForEach([&j, numbers1](Ptr(unsigned int) p) -> bool {
		ASSERTTESTFILE(1, (*p == numbers1[j]), _T("Test iterator value"));
		++j;
		return true;
	});
	ASSERTTESTFILE(1, (j == k), _T("End of loop"));
	j = k;
	list.ForEach([&j, numbers1](Ptr(unsigned int) p) -> bool {
		--j;
		ASSERTTESTFILE(1, (*p == numbers1[j]), _T("Test iterator value"));
		return true;
	}, true);
	ASSERTTESTFILE(1, (0 == j), _T("End of loop"));
}

template <typename TListe>
static void TestSortedListSingleKey(Ref(TListe) list, unsigned int* numbers, unsigned int* numbers1, dword k)
{
	TListe::Iterator it;
	TListe::Iterator it1;
	dword i;
	dword j;
	Ptr(unsigned int) pb;

	for (i = 0; i < k; ++i)
	{
		it = list.FindSorted(numbers + i);
		ASSERTTESTFILE(1, (it), _T("FindSorted"));
		pb = (unsigned int*)bsearch_s(numbers + i, numbers1, k, sizeof(unsigned int), bsearch_cb, NULL);
		assert(NotPtrCheck(pb));
		j = pb - numbers1;
		while (it)
		{
			ASSERTTESTFILE(1, (**it == numbers1[j]), _T("Test iterator value"));
			++j;
			++it;
		}
		ASSERTTESTFILE(1, (k == j), _T("End of loop"));
	}
	for (i = 0; i < k; i += 2)
		ASSERTTESTFILE(1, (list.RemoveSorted(numbers + i)), _T("RemoveSorted"));
	for (i = 0; i < k; ++i)
	{
		it = list.FindSorted(numbers + i);
		if ((i % 2) == 0)
		{
			ASSERTTESTFILE(1, (!(it)), _T("FindSorted"));
		}
		else
		{
			ASSERTTESTFILE(1, (it), _T("FindSorted"));
			if (it)
			{
				pb = (unsigned int*)bsearch_s(numbers + i, numbers1, k, sizeof(unsigned int), bsearch_cb, NULL);
				assert(NotPtrCheck(pb));
				j = pb - numbers1;
				ASSERTTESTFILE(1, (**it == numbers1[j]), _T("Test iterator value"));
			}
		}
		it = list.LowerBound(numbers + i);
		it1 = list.UpperBound(numbers + i);
		if ((i % 2) == 0)
		{
			ASSERTTESTFILE(1, (it == it1), _T("it = [%08lx:%03ld], it1 = [%08lx:%03ld]"), _Lnode(Cast(LSearchResultType, it)), _Loffset(Cast(LSearchResultType, it)), _Lnode(Cast(LSearchResultType, it1)), _Loffset(Cast(LSearchResultType, it1)));
			if (it)
				ASSERTTESTFILE(1, (**it > numbers[i]), _T("Test iterator value"));
		}
		else
		{
			ASSERTTESTFILE(1, (it), _T("LowerBound"));
			ASSERTTESTFILE(1, (it != it1), _T("it = [%08lx:%03ld], it1 = [%08lx:%03ld]"), _Lnode(Cast(LSearchResultType, it)), _Loffset(Cast(LSearchResultType, it)), _Lnode(Cast(LSearchResultType, it1)), _Loffset(Cast(LSearchResultType, it1)));
			if (it)
			{
				pb = (unsigned int*)bsearch_s(numbers + i, numbers1, k, sizeof(unsigned int), bsearch_cb, NULL);
				assert(NotPtrCheck(pb));
				j = pb - numbers1;
				ASSERTTESTFILE(1, (**it == numbers1[j]), _T("Test iterator value"));
				++it;
				ASSERTTESTFILE(1, (it == it1), _T("it = [%08lx:%03ld], it1 = [%08lx:%03ld]"), _Lnode(Cast(LSearchResultType, it)), _Loffset(Cast(LSearchResultType, it)), _Lnode(Cast(LSearchResultType, it1)), _Loffset(Cast(LSearchResultType, it1)));
			}
		}
	}
}

template <typename TListe>
static void TestSortedListMultipleKey(Ref(TListe) list, unsigned int* numbers, unsigned int* numbers1, dword k, dword m)
{
	TListe::Iterator it;
	TListe::Iterator it1;
	dword i;
	dword j;
	dword l;
	Ptr(unsigned int) pb;

	for (i = 0; i < k; ++i)
	{
		it = list.FindSorted(numbers + i);
		ASSERTTESTFILE(1, (it), _T("FindSorted"));
		pb = (unsigned int*)bsearch_s(numbers + i, numbers1, k * m, sizeof(unsigned int), bsearch_cb, NULL);
		assert(NotPtrCheck(pb));
		j = pb - numbers1;
		while ((j > 0) && (bsearch_cb(NULL, numbers + i, numbers1 + j) == 0))
			--j;
		if (j > 0)
			++j;
		while (it)
		{
			ASSERTTESTFILE(1, (**it == numbers1[j]), _T("Test iterator value"));
			++j;
			++it;
		}
		ASSERTTESTFILE(1, ((k * m) == j), _T("End of loop"));
	}
	for (i = 0; i < k; i += 2)
		ASSERTTESTFILE(1, (list.RemoveSorted(numbers + i)), _T("RemoveSorted"));
	for (i = 0; i < k; ++i)
	{
		it = list.FindSorted(numbers + i);
		if ((i % 2) == 0)
		{
			ASSERTTESTFILE(1, (!(it)), _T("FindSorted"));
		}
		else
		{
			ASSERTTESTFILE(1, (it), _T("FindSorted"));
			if (it)
			{
				pb = (unsigned int*)bsearch_s(numbers + i, numbers1, k * m, sizeof(unsigned int), bsearch_cb, NULL);
				assert(NotPtrCheck(pb));
				j = pb - numbers1;
				for (l = 0; l < m; ++l)
				{
					ASSERTTESTFILE(1, (**it == numbers1[j]), _T("Test iterator value"));
					++it;
				}
			}
		}
		it = list.LowerBound(numbers + i);
		it1 = list.UpperBound(numbers + i);
		if ((i % 2) == 0)
		{
			ASSERTTESTFILE(1, (it == it1), _T("it = [%08lx:%03ld], it1 = [%08lx:%03ld]"), _Lnode(Cast(LSearchResultType, it)), _Loffset(Cast(LSearchResultType, it)), _Lnode(Cast(LSearchResultType, it1)), _Loffset(Cast(LSearchResultType, it1)));
			if (it)
				ASSERTTESTFILE(1, (**it > numbers[i]), _T("Test iterator value"));
		}
		else
		{
			ASSERTTESTFILE(1, (it), _T("LowerBound"));
			ASSERTTESTFILE(1, (it != it1), _T("it = [%08lx:%03ld], it1 = [%08lx:%03ld]"), _Lnode(Cast(LSearchResultType, it)), _Loffset(Cast(LSearchResultType, it)), _Lnode(Cast(LSearchResultType, it1)), _Loffset(Cast(LSearchResultType, it1)));
			if (it)
			{
				pb = (unsigned int*)bsearch_s(numbers + i, numbers1, k * m, sizeof(unsigned int), bsearch_cb, NULL);
				assert(NotPtrCheck(pb));
				j = pb - numbers1;
				for (l = 0; l < m; ++l)
				{
					ASSERTTESTFILE(1, (**it == numbers1[j]), _T("Test iterator value"));
					++it;
				}
				ASSERTTESTFILE(1, (it == it1), _T("it = [%08lx:%03ld], it1 = [%08lx:%03ld]"), _Lnode(Cast(LSearchResultType, it)), _Loffset(Cast(LSearchResultType, it)), _Lnode(Cast(LSearchResultType, it1)), _Loffset(Cast(LSearchResultType, it1)));
			}
		}
	}
}

template <typename TListe>
static void TestSingleKey(Ref(TListe) list, unsigned int* numbers, size_t max_n)
{
	TListe::Iterator it;
	TListe::Iterator it1;
	dword i;
	dword j;
	dword k;
	Ptr(unsigned int) numbers1;

	/* empty list */
	it = list.Begin();
	ASSERTTESTFILE(1, (!it), _T("empty list, begin"));
	it = list.Last();
	ASSERTTESTFILE(1, (!it), _T("empty list, begin"));
	/* Append */
	for (i = 0; i < max_n; ++i)
	{
		list.Append(numbers + i);
		TestIterateBeginNext(list, numbers, i + 1);
	}
	/* Remove */
	i = 0;
	while (list.Count() > 0)
	{
		list.Remove(list.Begin());
		++i;
		j = i;
		it = list.Begin();
		while (it)
		{
			ASSERTTESTFILE(1, (**it == numbers[j]), _T("Test iterator value"));
			++j;
			++it;
		}
	}
	/* Prepend */
	for (i = 0; i < max_n; ++i)
	{
		list.Prepend(numbers + i);
		TestIterateForEachX(list, numbers, i + 1);
	}
	list.Clear();
	/* InsertBefore */
	list.Append(numbers);
	for (i = 1; i < max_n; ++i)
	{
		list.InsertBefore(list.Begin(), numbers + i);
		TestIterateForEachX(list, numbers, i + 1);
	}
	list.Clear();
	/* InsertAfter */
	list.Append(numbers);
	for (i = 1; i < max_n; ++i)
	{
		list.InsertAfter(list.Last(), numbers + i);
		TestIterateBeginNext(list, numbers, i + 1);
	}
	list.Clear();

	/* Append + Sort */
	numbers1 = new unsigned int[max_n];
	for (k = 0; k < max_n; ++k)
	{
		for (i = 0; i < k; ++i)
		{
			it = list.Append(numbers + i);
			ASSERTTESTFILE(1, (it), _T("Append"));
			if (it)
				ASSERTTESTFILE(1, (**it == numbers[i]), _T("Test iterator value"));
		}

		list.Sort();

		for (i = 0; i < k; ++i)
			numbers1[i] = numbers[i];
		qsort(numbers1, k, sizeof(unsigned int), TestCompareSRand);

		TestIterateForEach(list, numbers1, k);
		TestSortedListSingleKey(list, numbers, numbers1, k);

		list.Clear();
	}
	/* InsertSorted */
	for (k = 0; k < max_n; ++k)
	{
		for (i = 0; i < k; ++i)
			numbers1[i] = numbers[i];
		qsort(numbers1, k, sizeof(unsigned int), TestCompareSRand);

		for (i = 0; i < k; ++i)
		{
			it = list.InsertSorted(numbers + i);
			ASSERTTESTFILE(1, (it), _T("InsertSorted"));
			if (it)
				ASSERTTESTFILE(1, (**it == numbers[i]), _T("Test iterator value"));
		}

		TestIterateForEach(list, numbers1, k);
		TestSortedListSingleKey(list, numbers, numbers1, k);
		list.Clear();
	}
	delete[] numbers1;
	list.Close();
}

template <typename TListe>
static void TestMultipleKey(Ref(TListe) list, unsigned int* numbers, size_t max_n)
{
	TListe::Iterator it;
	TListe::Iterator it1;
	dword i;
	dword j;
	dword k;
	dword l;
	Ptr(unsigned int) numbers1;

	/* InsertSorted + multiple keys */
	numbers1 = new unsigned int[max_n * 5];
	for (k = 0; k < max_n; ++k)
	{
		for (i = 0, j = 0; i < k; ++i)
			for (l = 0; l < 5; ++l, ++j)
				numbers1[j] = numbers[i];
		qsort(numbers1, k * 5, sizeof(unsigned int), TestCompareSRand);

		for (i = 0, j = 0; i < k; ++i)
			for (l = 0; l < 5; ++l, ++j)
			{
				it = list.InsertSorted(numbers + i);
				ASSERTTESTFILE(1, (it), _T("InsertSorted"));
				if (it)
					ASSERTTESTFILE(1, (**it == numbers[i]), _T("Test iterator value"));
			}

		TestIterateForEach(list, numbers1, k * 5);
		TestSortedListMultipleKey(list, numbers, numbers1, k, 5);

		list.Clear();
	}
	delete[] numbers1;
	list.Close();
}

template <typename TListe>
static void TestBinaryTree(Ref(TListe) list, unsigned int* numbers, unsigned int* numbers1, size_t max_n)
{
	TListe::Iterator it;
	TListe::Iterator it1;
	dword i;
	dword j;
	dword l;
	Ptr(unsigned int) pb;

	/* empty list */
	it = list.Begin();
	ASSERTTESTFILE(1, (!it), _T("empty list, begin"));
	it = list.Last();
	ASSERTTESTFILE(1, (!it), _T("empty list, begin"));
	for (i = 0; i < max_n; ++i)
	{
		for (j = 0; j < (i + 1); ++j)
		{
			it = list.InsertSorted(numbers + j);
			ASSERTTESTFILE(1, (it), _T("InsertSorted"));
			if (it)
				ASSERTTESTFILE(1, (**it == numbers[j]), _T("Test iterator value"));
		}
		for (j = 0; j < (i + 1); ++j)
			numbers1[j] = numbers[j];
		qsort(numbers1, i + 1, sizeof(unsigned int), TestCompareSRand);

		TestIterateBeginNext(list, numbers1, i + 1);
		TestIterateForEach(list, numbers1, i + 1);

		for (j = 0; j < (i + 1); ++j)
		{
			it = list.FindSorted(numbers + j);
			pb = (unsigned int*)bsearch_s(numbers + j, numbers1, i + 1, sizeof(unsigned int), bsearch_cb, NULL);
			assert(NotPtrCheck(pb));
			j = pb - numbers1;
			while (it)
			{
				ASSERTTESTFILE(1, (**it == numbers1[j]), _T("Test iterator value"));
				++j;
				++it;
			}
			ASSERTTESTFILE(1, (j == (i + 1)), _T("End of loop"));
		}
		j = 0;
		while (list.Count() > 0)
		{
			ASSERTTESTFILE(1, (list.RemoveSorted(numbers + j)), _T("RemoveSorted"));
			++j;
			ASSERTTESTFILE(1, (j <= (i + 1)), _T("End of loop"));

			for (l = j; l < (i + 1); ++l)
				numbers1[l - j] = numbers[l];
			qsort(numbers1, i + 1 - j, sizeof(unsigned int), TestCompareSRand);
			l = 0;
			it = list.Begin();
			while (it)
			{
				ASSERTTESTFILE(1, (**it == numbers1[l]), _T("Test iterator value"));
				++l;
				++it;
			}
			ASSERTTESTFILE(1, (l == (i + 1 - j)), _T("End of loop"));
		}
	}
	list.Close();
}

static void TestDoubleLinkedListT()
{
	OpenTestFile(_T("TestDoubleLinkedListT"));

	TestFuncUIntDoubleLinkedList list __FILE__LINE__0P;
	unsigned int numbers[120];

	LoadTestData(_T("TestDoubleLinkedListT"), numbers, 120);

	TestSingleKey(list, numbers, 120);
	list.Open(__FILE__LINE__0);
	TestMultipleKey(list, numbers, 120);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestArrayT()
{
	OpenTestFile(_T("TestArrayT"));

	TestFuncUIntArray list(__FILE__LINE__ 120);
	unsigned int numbers[120];

	LoadTestData(_T("TestArrayT"), numbers, 120);

	TestSingleKey(list, numbers, 120);

	TestFuncUIntArray list2(__FILE__LINE__ 600);

	TestMultipleKey(list2, numbers, 120);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestVectorT()
{
	OpenTestFile(_T("TestVectorT"));

	TestFuncUIntVector list(__FILE__LINE__ 16, 16);
	unsigned int numbers[256];

	LoadTestData(_T("TestVectorT"), numbers, 256);
	
	TestSingleKey(list, numbers, 256);
	list.Open(__FILE__LINE__ 16, 16);
	TestMultipleKey(list, numbers, 256);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestAVLBinaryTreeT()
{
	OpenTestFile(_T("TestAVLBinaryTreeT"));

	TestFuncUIntAVLBinaryTree list __FILE__LINE__0P;
	unsigned int numbers[256];
	unsigned int numbers1[256];

	LoadTestData(_T("TestAVLBinaryTreeT"), numbers, 256);

	TestBinaryTree(list, numbers, numbers1, 256);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestRBBinaryTreeT()
{
	OpenTestFile(_T("TestRBBinaryTreeT"));

	TestFuncUIntRBBinaryTree list __FILE__LINE__0P;
	unsigned int numbers[256];
	unsigned int numbers1[256];

	LoadTestData(_T("TestRBBinaryTreeT"), numbers, 256);

	TestBinaryTree(list, numbers, numbers1, 256);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

static void TestBTreeT()
{
	OpenTestFile(_T("TestBTreeT"));

	TestFuncUIntBTree list(__FILE__LINE__ 16);
	unsigned int numbers[1024];

	LoadTestData(_T("TestBTreeT"), numbers, 1024);

	TestSingleKey(list, numbers, 1024);
	list.Open(__FILE__LINE__ 16);
	TestMultipleKey(list, numbers, 1024);

	WriteSuccessTestFile(1);

	CloseTestFile();
}

void TestDataStructures()
{
	COUT << _T("********************** TestDataStructures start **************") << endl;
	COUT << _T("********************** TestFlagRegister **********************") << endl;
	COUT << _T("Tests bit register.") << endl;
	COUT << _T("Standard Test Procedure.") << endl;
	TestFlagRegister();
	COUT << _T("********************** TestHashLinkedListT *******************") << endl;
	COUT << _T("Tests hash linked list.") << endl;
	COUT << _T("Standard Test Procedure.") << endl;
	TestHashLinkedListT();
	COUT << _T("********************** TestDoubleLinkedListT *****************") << endl;
	COUT << _T("Tests double linked list.") << endl;
	COUT << _T("Standard Test Procedure.") << endl;
	TestDoubleLinkedListT();
	COUT << _T("********************** TestArrayT ****************************") << endl;
	COUT << _T("Tests array.") << endl;
	COUT << _T("Standard Test Procedure.") << endl;
	TestArrayT();
	COUT << _T("********************** TestVectorT ***************************") << endl;
	COUT << _T("Tests vector.") << endl;
	COUT << _T("Standard Test Procedure.") << endl;
	TestVectorT();
	COUT << _T("********************** TestAVLBinaryTreeT ********************") << endl;
	COUT << _T("Tests balanced binary tree.") << endl;
	COUT << _T("Standard Test Procedure.") << endl;
	TestAVLBinaryTreeT();
	COUT << _T("********************** TestRBBinaryTreeT *********************") << endl;
	COUT << _T("Tests balanced binary tree.") << endl;
	COUT << _T("Standard Test Procedure.") << endl;
	TestRBBinaryTreeT();
	COUT << _T("********************** TestBTreeT ****************************") << endl;
	COUT << _T("Tests B*-tree.") << endl;
	COUT << _T("Standard Test Procedure.") << endl;
	TestBTreeT();
	COUT << _T("********************** TestDataStructures exit ***************") << endl;
}
