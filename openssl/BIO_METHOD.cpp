/******************************************************************************
    
	This file is part of openssl, which is part of UserLib.

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
#include "OS_PCH.H"
#include "BIO_METHOD.h"
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>

CBIO_METHOD::CBIO_METHOD(ConstPointer method) :
COpenSSLClass(method)
{}

CBIO_METHOD::~CBIO_METHOD()
{}

Ptr(CBIO_METHOD) CBIO_METHOD::create_f_asn1()
{
	Ptr(CBIO_METHOD) p = OK_NEW_OPERATOR CBIO_METHOD(BIO_f_asn1());

	p->insert_obj();
	return p;
}

Ptr(CBIO_METHOD) CBIO_METHOD::create_f_base64()
{
	Ptr(CBIO_METHOD) p = OK_NEW_OPERATOR CBIO_METHOD(BIO_f_base64());

	p->insert_obj();
	return p;
}

Ptr(CBIO_METHOD) CBIO_METHOD::create_f_buffer()
{
	Ptr(CBIO_METHOD) p = OK_NEW_OPERATOR CBIO_METHOD(BIO_f_buffer());

	p->insert_obj();
	return p;
}

Ptr(CBIO_METHOD) CBIO_METHOD::create_f_cipher()
{
	Ptr(CBIO_METHOD) p = OK_NEW_OPERATOR CBIO_METHOD(BIO_f_cipher());

	p->insert_obj();
	return p;
}

Ptr(CBIO_METHOD) CBIO_METHOD::create_f_reliable()
{
	Ptr(CBIO_METHOD) p = OK_NEW_OPERATOR CBIO_METHOD(BIO_f_reliable());

	p->insert_obj();
	return p;
}

Ptr(CBIO_METHOD) CBIO_METHOD::create_f_md()
{
	Ptr(CBIO_METHOD) p = OK_NEW_OPERATOR CBIO_METHOD(BIO_f_md());

	p->insert_obj();
	return p;
}

Ptr(CBIO_METHOD) CBIO_METHOD::create_f_null()
{
	Ptr(CBIO_METHOD) p = OK_NEW_OPERATOR CBIO_METHOD(BIO_f_null());

	p->insert_obj();
	return p;
}

Ptr(CBIO_METHOD) CBIO_METHOD::create_f_ssl()
{
	Ptr(CBIO_METHOD) p = OK_NEW_OPERATOR CBIO_METHOD(BIO_f_ssl());

	return p;
}

Ptr(CBIO_METHOD) CBIO_METHOD::create_s_bio_pair()
{
	Ptr(CBIO_METHOD) p = OK_NEW_OPERATOR CBIO_METHOD(BIO_s_bio());

	p->insert_obj();
	return p;
}

Ptr(CBIO_METHOD) CBIO_METHOD::create_s_accept()
{
	Ptr(CBIO_METHOD) p = OK_NEW_OPERATOR CBIO_METHOD(BIO_s_accept());

	p->insert_obj();
	return p;
}

Ptr(CBIO_METHOD) CBIO_METHOD::create_s_connect()
{
	Ptr(CBIO_METHOD) p = OK_NEW_OPERATOR CBIO_METHOD(BIO_s_connect());

	p->insert_obj();
	return p;
}

Ptr(CBIO_METHOD) CBIO_METHOD::create_s_fd()
{
	Ptr(CBIO_METHOD) p = OK_NEW_OPERATOR CBIO_METHOD(BIO_s_fd());

	p->insert_obj();
	return p;
}

Ptr(CBIO_METHOD) CBIO_METHOD::create_s_file()
{
	Ptr(CBIO_METHOD) p = OK_NEW_OPERATOR CBIO_METHOD(BIO_s_file());

	p->insert_obj();
	return p;
}

Ptr(CBIO_METHOD) CBIO_METHOD::create_s_mem()
{
	Ptr(CBIO_METHOD) p = OK_NEW_OPERATOR CBIO_METHOD(BIO_s_mem());

	p->insert_obj();
	return p;
}

Ptr(CBIO_METHOD) CBIO_METHOD::create_s_null()
{
	Ptr(CBIO_METHOD) p = OK_NEW_OPERATOR CBIO_METHOD(BIO_s_null());

	p->insert_obj();
	return p;
}

Ptr(CBIO_METHOD) CBIO_METHOD::create_s_socket()
{
	Ptr(CBIO_METHOD) p = OK_NEW_OPERATOR CBIO_METHOD(BIO_s_socket());

	p->insert_obj();
	return p;
}

Ptr(CBIO_METHOD) CBIO_METHOD::create_s_datagram()
{
	Ptr(CBIO_METHOD) p = OK_NEW_OPERATOR CBIO_METHOD(BIO_s_datagram());

	p->insert_obj();
	return p;
}

