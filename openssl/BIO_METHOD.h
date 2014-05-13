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
#pragma once

#include "OpenSSL.h"

class OPENSSL_API CBIO_METHOD : public COpenSSLClass
{
public:
	CBIO_METHOD(ConstPointer method = NULL);
	virtual ~CBIO_METHOD();

	__inline ConstPointer get_method() const { return _raw; }

	static Ptr(CBIO_METHOD) create_f_asn1();
	static Ptr(CBIO_METHOD) create_f_base64();
	static Ptr(CBIO_METHOD) create_f_buffer();
	static Ptr(CBIO_METHOD) create_f_cipher();
	static Ptr(CBIO_METHOD) create_f_reliable();
	static Ptr(CBIO_METHOD) create_f_md();
	static Ptr(CBIO_METHOD) create_f_null();
	static Ptr(CBIO_METHOD) create_f_ssl();

	static Ptr(CBIO_METHOD) create_s_bio_pair();
	static Ptr(CBIO_METHOD) create_s_accept();
	static Ptr(CBIO_METHOD) create_s_connect();
	static Ptr(CBIO_METHOD) create_s_fd();
	static Ptr(CBIO_METHOD) create_s_file();
	static Ptr(CBIO_METHOD) create_s_mem();
	static Ptr(CBIO_METHOD) create_s_null();
	static Ptr(CBIO_METHOD) create_s_socket();
	static Ptr(CBIO_METHOD) create_s_datagram();

};

