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
 *  \file SAXParserImpl.h
 *  \brief XML parsing engine implementation classes.
 */
#pragma once

#include "CppSources.h"
#include "expat.h"

class CPPSOURCES_API CSAXParserContentHandler;
class CPPSOURCES_API CSAXParserDTDHandler;
class CPPSOURCES_LOCAL CSAXParserImpl : public CCppObject
{
public:
	CSAXParserImpl();
	~CSAXParserImpl();

	void Create(Ptr(CSAXParserContentHandler) h = NULL, CConstPointer encoding = NULL);

	Ptr(CSAXParserContentHandler) get_contenthandler() const;
	void set_contenthandler(Ptr(CSAXParserContentHandler) h);

	Ptr(CSAXParserDTDHandler) get_dtdhandler() const;
	void set_dtdhandler(Ptr(CSAXParserDTDHandler) h);

	ConstRef(CStringBuffer) get_encoding() const;
	void set_encoding(CConstPointer encoding);

	__inline XML_Parser get_parser() const { return _parser; }

	void Parse(ConstRef(CByteBuffer) xmltext, bool isFinal = false);

protected:
	static void XMLCALL XmlDeclHandler(void *userData,
		const XML_Char *version,
		const XML_Char *encoding,
		int             standalone);

	static void XMLCALL ProcessingInstructionHandler (void *userData,
		const XML_Char *target,
		const XML_Char *data);

	static void XMLCALL StartElementHandler(void *userData,
		const XML_Char *name,
		const XML_Char **atts);

	static void XMLCALL EndElementHandler(void *userData,
		const XML_Char *name);

	static void XMLCALL CharacterDataHandler(void *userData,
		const XML_Char *s,
		int len);

	static void XMLCALL CommentHandler(void *userData,
		const XML_Char *data);

	static void XMLCALL StartCdataSectionHandler(void *userData);

	static void XMLCALL EndCdataSectionHandler(void *userData);

	static void XMLCALL DefaultHandler(void *userData,
		const XML_Char *s,
		int len);

	static int XMLCALL NotStandaloneHandler(void *userData);

	static void XMLCALL StartDoctypeDeclHandler(
		void *userData,
		const XML_Char *doctypeName,
		const XML_Char *sysid,
		const XML_Char *pubid,
		int has_internal_subset);

	static void XMLCALL EndDoctypeDeclHandler(void *userData);

	static void XMLCALL ElementDeclHandler(void *userData,
		const XML_Char *name,
		XML_Content *model);

	static void XMLCALL AttlistDeclHandler(
		void            *userData,
		const XML_Char  *elname,
		const XML_Char  *attname,
		const XML_Char  *att_type,
		const XML_Char  *dflt,
		int              isrequired);

	static void XMLCALL EntityDeclHandler(
		void *userData,
		const XML_Char *entityName,
		int is_parameter_entity,
		const XML_Char *value,
		int value_length,
		const XML_Char *base,
		const XML_Char *systemId,
		const XML_Char *publicId,
		const XML_Char *notationName);

	static int XMLCALL ExternalEntityRefHandler(
		XML_Parser parser,
		const XML_Char *context,
		const XML_Char *base,
		const XML_Char *systemId,
		const XML_Char *publicId);

	static void XMLCALL SkippedEntityHandler(
		void *userData,
		const XML_Char *entityName,
		int is_parameter_entity);

	static void XMLCALL NotationDeclHandler(
		void *userData,
		const XML_Char *notationName,
		const XML_Char *base,
		const XML_Char *systemId,
		const XML_Char *publicId);

	static void XMLCALL StartNamespaceDeclHandler(
		void *userData,
		const XML_Char *prefix,
		const XML_Char *uri);

	static void XMLCALL EndNamespaceDeclHandler (
		void *userData,
		const XML_Char *prefix);

	XML_Parser _parser;
	WBool _parsing;
	CStringBuffer _encoding;
	Ptr(CSAXParserContentHandler) _contenthandler;
	Ptr(CSAXParserDTDHandler) _dtdhandler;
};

