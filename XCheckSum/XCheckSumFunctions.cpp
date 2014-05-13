/******************************************************************************
    
	This file is part of XCheckSum, which is part of UserLib.

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
#include "stdafx.h"
#include "SecurityFile.h"
#include "EVP_MD.h"

void f_md4(ConstRef(CFilePath) a, ConstRef(CStringBuffer) r)
{
	Ptr(CSecurityFile) f = OK_NEW_OPERATOR CSecurityFile(a, true, false, CFile::BinaryFile_NoEncoding);
	Ptr(CEVP_MD_CTX) ctx = CEVP_MD_CTX::create();
	CByteBuffer buf(__FILE__LINE__ 1024);
	CStringBuffer digest;
	unsigned int sz;

	ctx->DigestInit(CEVP_MD::create_md4());
	f->Read(buf);
	while (buf.get_BufferSize() > 0)
	{
		ctx->DigestUpdate(buf.get_Buffer(), buf.get_BufferSize());
		f->Read(buf);
	}
	buf.set_BufferSize(__FILE__LINE__ ctx->size());
	ctx->DigestFinal(buf.get_Buffer(), &sz);
	digest.convertToHex(buf);

	if (!(r.IsEmpty()))
	{
		if (r.Compare(digest) == 0)
			COUT << digest << _T(" *") << a.get_Path() << endl;
		else
			CERR << _T("ERROR: Digest mismatch reading '") << a.get_Path() << _T("'") << endl;
	}
	else
		COUT << digest << _T(" *") << a.get_Path() << endl;

	f->Close();
	f->release();
	ctx->destroy();
}

void f_md5(ConstRef(CFilePath) a, ConstRef(CStringBuffer) r)
{
	Ptr(CSecurityFile) f = OK_NEW_OPERATOR CSecurityFile(a, true, false, CFile::BinaryFile_NoEncoding);
	Ptr(CEVP_MD_CTX) ctx = CEVP_MD_CTX::create();
	CByteBuffer buf(__FILE__LINE__ 1024);
	CStringBuffer digest;
	unsigned int sz;

	ctx->DigestInit(CEVP_MD::create_md5());
	f->Read(buf);
	while (buf.get_BufferSize() > 0)
	{
		ctx->DigestUpdate(buf.get_Buffer(), buf.get_BufferSize());
		f->Read(buf);
	}
	buf.set_BufferSize(__FILE__LINE__ ctx->size());
	ctx->DigestFinal(buf.get_Buffer(), &sz);
	digest.convertToHex(buf);

	if (!(r.IsEmpty()))
	{
		if (r.Compare(digest) == 0)
			COUT << digest << _T(" *") << a.get_Path() << endl;
		else
			CERR << _T("ERROR: Digest mismatch reading '") << a.get_Path() << _T("'") << endl;
	}
	else
		COUT << digest << _T(" *") << a.get_Path() << endl;

	f->Close();
	f->release();
	ctx->destroy();
}

void f_sha(ConstRef(CFilePath) a, ConstRef(CStringBuffer) r)
{
	Ptr(CSecurityFile) f = OK_NEW_OPERATOR CSecurityFile(a, true, false, CFile::BinaryFile_NoEncoding);
	Ptr(CEVP_MD_CTX) ctx = CEVP_MD_CTX::create();
	CByteBuffer buf(__FILE__LINE__ 1024);
	CStringBuffer digest;
	unsigned int sz;

	ctx->DigestInit(CEVP_MD::create_sha());
	f->Read(buf);
	while (buf.get_BufferSize() > 0)
	{
		ctx->DigestUpdate(buf.get_Buffer(), buf.get_BufferSize());
		f->Read(buf);
	}
	buf.set_BufferSize(__FILE__LINE__ ctx->size());
	ctx->DigestFinal(buf.get_Buffer(), &sz);
	digest.convertToHex(buf);

	if (!(r.IsEmpty()))
	{
		if (r.Compare(digest) == 0)
			COUT << digest << _T(" *") << a.get_Path() << endl;
		else
			CERR << _T("ERROR: Digest mismatch reading '") << a.get_Path() << _T("'") << endl;
	}
	else
		COUT << digest << _T(" *") << a.get_Path() << endl;

	f->Close();
	f->release();
	ctx->destroy();
}

void f_sha1(ConstRef(CFilePath) a, ConstRef(CStringBuffer) r)
{
	Ptr(CSecurityFile) f = OK_NEW_OPERATOR CSecurityFile(a, true, false, CFile::BinaryFile_NoEncoding);
	Ptr(CEVP_MD_CTX) ctx = CEVP_MD_CTX::create();
	CByteBuffer buf(__FILE__LINE__ 1024);
	CStringBuffer digest;
	unsigned int sz;

	ctx->DigestInit(CEVP_MD::create_sha1());
	f->Read(buf);
	while (buf.get_BufferSize() > 0)
	{
		ctx->DigestUpdate(buf.get_Buffer(), buf.get_BufferSize());
		f->Read(buf);
	}
	buf.set_BufferSize(__FILE__LINE__ ctx->size());
	ctx->DigestFinal(buf.get_Buffer(), &sz);
	digest.convertToHex(buf);

	if (!(r.IsEmpty()))
	{
		if (r.Compare(digest) == 0)
			COUT << digest << _T(" *") << a.get_Path() << endl;
		else
			CERR << _T("ERROR: Digest mismatch reading '") << a.get_Path() << _T("'") << endl;
	}
	else
		COUT << digest << _T(" *") << a.get_Path() << endl;

	f->Close();
	f->release();
	ctx->destroy();
}

void f_dss(ConstRef(CFilePath) a, ConstRef(CStringBuffer) r)
{
	Ptr(CSecurityFile) f = OK_NEW_OPERATOR CSecurityFile(a, true, false, CFile::BinaryFile_NoEncoding);
	Ptr(CEVP_MD_CTX) ctx = CEVP_MD_CTX::create();
	CByteBuffer buf(__FILE__LINE__ 1024);
	CStringBuffer digest;
	unsigned int sz;

	ctx->DigestInit(CEVP_MD::create_dss());
	f->Read(buf);
	while (buf.get_BufferSize() > 0)
	{
		ctx->DigestUpdate(buf.get_Buffer(), buf.get_BufferSize());
		f->Read(buf);
	}
	buf.set_BufferSize(__FILE__LINE__ ctx->size());
	ctx->DigestFinal(buf.get_Buffer(), &sz);
	digest.convertToHex(buf);

	if (!(r.IsEmpty()))
	{
		if (r.Compare(digest) == 0)
			COUT << digest << _T(" *") << a.get_Path() << endl;
		else
			CERR << _T("ERROR: Digest mismatch reading '") << a.get_Path() << _T("'") << endl;
	}
	else
		COUT << digest << _T(" *") << a.get_Path() << endl;

	f->Close();
	f->release();
	ctx->destroy();
}

void f_dss1(ConstRef(CFilePath) a, ConstRef(CStringBuffer) r)
{
	Ptr(CSecurityFile) f = OK_NEW_OPERATOR CSecurityFile(a, true, false, CFile::BinaryFile_NoEncoding);
	Ptr(CEVP_MD_CTX) ctx = CEVP_MD_CTX::create();
	CByteBuffer buf(__FILE__LINE__ 1024);
	CStringBuffer digest;
	unsigned int sz;

	ctx->DigestInit(CEVP_MD::create_dss1());
	f->Read(buf);
	while (buf.get_BufferSize() > 0)
	{
		ctx->DigestUpdate(buf.get_Buffer(), buf.get_BufferSize());
		f->Read(buf);
	}
	buf.set_BufferSize(__FILE__LINE__ ctx->size());
	ctx->DigestFinal(buf.get_Buffer(), &sz);
	digest.convertToHex(buf);

	if (!(r.IsEmpty()))
	{
		if (r.Compare(digest) == 0)
			COUT << digest << _T(" *") << a.get_Path() << endl;
		else
			CERR << _T("ERROR: Digest mismatch reading '") << a.get_Path() << _T("'") << endl;
	}
	else
		COUT << digest << _T(" *") << a.get_Path() << endl;

	f->Close();
	f->release();
	ctx->destroy();
}

void f_mdc2(ConstRef(CFilePath) a, ConstRef(CStringBuffer) r)
{
	Ptr(CSecurityFile) f = OK_NEW_OPERATOR CSecurityFile(a, true, false, CFile::BinaryFile_NoEncoding);
	Ptr(CEVP_MD_CTX) ctx = CEVP_MD_CTX::create();
	CByteBuffer buf(__FILE__LINE__ 1024);
	CStringBuffer digest;
	unsigned int sz;

	ctx->DigestInit(CEVP_MD::create_mdc2());
	f->Read(buf);
	while (buf.get_BufferSize() > 0)
	{
		ctx->DigestUpdate(buf.get_Buffer(), buf.get_BufferSize());
		f->Read(buf);
	}
	buf.set_BufferSize(__FILE__LINE__ ctx->size());
	ctx->DigestFinal(buf.get_Buffer(), &sz);
	digest.convertToHex(buf);

	if (!(r.IsEmpty()))
	{
		if (r.Compare(digest) == 0)
			COUT << digest << _T(" *") << a.get_Path() << endl;
		else
			CERR << _T("ERROR: Digest mismatch reading '") << a.get_Path() << _T("'") << endl;
	}
	else
		COUT << digest << _T(" *") << a.get_Path() << endl;

	f->Close();
	f->release();
	ctx->destroy();
}

void f_ripemd160(ConstRef(CFilePath) a, ConstRef(CStringBuffer) r)
{
	Ptr(CSecurityFile) f = OK_NEW_OPERATOR CSecurityFile(a, true, false, CFile::BinaryFile_NoEncoding);
	Ptr(CEVP_MD_CTX) ctx = CEVP_MD_CTX::create();
	CByteBuffer buf(__FILE__LINE__ 1024);
	CStringBuffer digest;
	unsigned int sz;

	ctx->DigestInit(CEVP_MD::create_ripemd160());
	f->Read(buf);
	while (buf.get_BufferSize() > 0)
	{
		ctx->DigestUpdate(buf.get_Buffer(), buf.get_BufferSize());
		f->Read(buf);
	}
	buf.set_BufferSize(__FILE__LINE__ ctx->size());
	ctx->DigestFinal(buf.get_Buffer(), &sz);
	digest.convertToHex(buf);

	if (!(r.IsEmpty()))
	{
		if (r.Compare(digest) == 0)
			COUT << digest << _T(" *") << a.get_Path() << endl;
		else
			CERR << _T("ERROR: Digest mismatch reading '") << a.get_Path() << _T("'") << endl;
	}
	else
		COUT << digest << _T(" *") << a.get_Path() << endl;

	f->Close();
	f->release();
	ctx->destroy();
}

void f_sha224(ConstRef(CFilePath) a, ConstRef(CStringBuffer) r)
{
	Ptr(CSecurityFile) f = OK_NEW_OPERATOR CSecurityFile(a, true, false, CFile::BinaryFile_NoEncoding);
	Ptr(CEVP_MD_CTX) ctx = CEVP_MD_CTX::create();
	CByteBuffer buf(__FILE__LINE__ 1024);
	CStringBuffer digest;
	unsigned int sz;

	ctx->DigestInit(CEVP_MD::create_sha224());
	f->Read(buf);
	while (buf.get_BufferSize() > 0)
	{
		ctx->DigestUpdate(buf.get_Buffer(), buf.get_BufferSize());
		f->Read(buf);
	}
	buf.set_BufferSize(__FILE__LINE__ ctx->size());
	ctx->DigestFinal(buf.get_Buffer(), &sz);
	digest.convertToHex(buf);

	if (!(r.IsEmpty()))
	{
		if (r.Compare(digest) == 0)
			COUT << digest << _T(" *") << a.get_Path() << endl;
		else
			CERR << _T("ERROR: Digest mismatch reading '") << a.get_Path() << _T("'") << endl;
	}
	else
		COUT << digest << _T(" *") << a.get_Path() << endl;

	f->Close();
	f->release();
	ctx->destroy();
}

void f_sha256(ConstRef(CFilePath) a, ConstRef(CStringBuffer) r)
{
	Ptr(CSecurityFile) f = OK_NEW_OPERATOR CSecurityFile(a, true, false, CFile::BinaryFile_NoEncoding);
	Ptr(CEVP_MD_CTX) ctx = CEVP_MD_CTX::create();
	CByteBuffer buf(__FILE__LINE__ 1024);
	CStringBuffer digest;
	unsigned int sz;

	ctx->DigestInit(CEVP_MD::create_sha256());
	f->Read(buf);
	while (buf.get_BufferSize() > 0)
	{
		ctx->DigestUpdate(buf.get_Buffer(), buf.get_BufferSize());
		f->Read(buf);
	}
	buf.set_BufferSize(__FILE__LINE__ ctx->size());
	ctx->DigestFinal(buf.get_Buffer(), &sz);
	digest.convertToHex(buf);

	if (!(r.IsEmpty()))
	{
		if (r.Compare(digest) == 0)
			COUT << digest << _T(" *") << a.get_Path() << endl;
		else
			CERR << _T("ERROR: Digest mismatch reading '") << a.get_Path() << _T("'") << endl;
	}
	else
		COUT << digest << _T(" *") << a.get_Path() << endl;

	f->Close();
	f->release();
	ctx->destroy();
}

void f_sha384(ConstRef(CFilePath) a, ConstRef(CStringBuffer) r)
{
	Ptr(CSecurityFile) f = OK_NEW_OPERATOR CSecurityFile(a, true, false, CFile::BinaryFile_NoEncoding);
	Ptr(CEVP_MD_CTX) ctx = CEVP_MD_CTX::create();
	CByteBuffer buf(__FILE__LINE__ 1024);
	CStringBuffer digest;
	unsigned int sz;

	ctx->DigestInit(CEVP_MD::create_sha384());
	f->Read(buf);
	while (buf.get_BufferSize() > 0)
	{
		ctx->DigestUpdate(buf.get_Buffer(), buf.get_BufferSize());
		f->Read(buf);
	}
	buf.set_BufferSize(__FILE__LINE__ ctx->size());
	ctx->DigestFinal(buf.get_Buffer(), &sz);
	digest.convertToHex(buf);

	if (!(r.IsEmpty()))
	{
		if (r.Compare(digest) == 0)
			COUT << digest << _T(" *") << a.get_Path() << endl;
		else
			CERR << _T("ERROR: Digest mismatch reading '") << a.get_Path() << _T("'") << endl;
	}
	else
		COUT << digest << _T(" *") << a.get_Path() << endl;

	f->Close();
	f->release();
	ctx->destroy();
}

void f_sha512(ConstRef(CFilePath) a, ConstRef(CStringBuffer) r)
{
	Ptr(CSecurityFile) f = OK_NEW_OPERATOR CSecurityFile(a, true, false, CFile::BinaryFile_NoEncoding);
	Ptr(CEVP_MD_CTX) ctx = CEVP_MD_CTX::create();
	CByteBuffer buf(__FILE__LINE__ 1024);
	CStringBuffer digest;
	unsigned int sz;

	ctx->DigestInit(CEVP_MD::create_sha512());
	f->Read(buf);
	while (buf.get_BufferSize() > 0)
	{
		ctx->DigestUpdate(buf.get_Buffer(), buf.get_BufferSize());
		f->Read(buf);
	}
	buf.set_BufferSize(__FILE__LINE__ ctx->size());
	ctx->DigestFinal(buf.get_Buffer(), &sz);
	digest.convertToHex(buf);

	if (!(r.IsEmpty()))
	{
		if (r.Compare(digest) == 0)
			COUT << digest << _T(" *") << a.get_Path() << endl;
		else
			CERR << _T("ERROR: Digest mismatch reading '") << a.get_Path() << _T("'") << endl;
	}
	else
		COUT << digest << _T(" *") << a.get_Path() << endl;

	f->Close();
	f->release();
	ctx->destroy();
}

