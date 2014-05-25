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
 *  \file SAXParser.h
 *  \brief XML parsing engine interface classes supported by expat.
 */
#pragma once

#include "CppSources.h"
#include "BaseException.h"
#include "FilePath.h"
#include "DataVector.h"

class CPPSOURCES_API CSAXParserAttribute : public CCppObject
{
public:
	CSAXParserAttribute();
	CSAXParserAttribute(ConstRef(CStringBuffer) name, ConstRef(CStringBuffer) value);
	virtual ~CSAXParserAttribute();

	__inline ConstRef(CStringBuffer) get_name() const { return _name; }
	__inline ConstRef(CStringBuffer) get_value() const { return _value; }

protected:
	CStringBuffer _name;
	CStringBuffer _value;
};

class CPPSOURCES_API CSAXParserAttributes : public CDataVectorT<CSAXParserAttribute>
{
public:
	typedef CDataVectorT<CSAXParserAttribute> super;

	CSAXParserAttributes(DECL_FILE_LINE0);
	~CSAXParserAttributes();
};

class CPPSOURCES_API CSAXParserContentHandler : public CCppObject
{
public:
	CSAXParserContentHandler();
	virtual ~CSAXParserContentHandler();

	virtual void XmlDeclHandler(ConstRef(CStringBuffer) version, ConstRef(CStringBuffer) encoding, int standalone);
	virtual void ProcessingInstructionHandler(ConstRef(CStringBuffer) target, ConstRef(CStringBuffer) data);
	virtual void StartElementHandler(ConstRef(CStringBuffer) name, ConstRef(CSAXParserAttributes) attributes);
	virtual void EndElementHandler(ConstRef(CStringBuffer) name);
	virtual void CharacterDataHandler(ConstRef(CStringBuffer) text);
	virtual void CommentHandler(ConstRef(CStringBuffer) text);
	virtual void StartCdataSectionHandler();
	virtual void EndCdataSectionHandler();
	virtual void DefaultHandler(ConstRef(CStringBuffer) text);
};

class CPPSOURCES_API CSAXParserElementDefinition : public CCppObject
{
public:
	enum XML_Content_Type {
		XML_CTYPE_EMPTY = 1,
		XML_CTYPE_ANY,
		XML_CTYPE_MIXED,
		XML_CTYPE_NAME,
		XML_CTYPE_CHOICE,
		XML_CTYPE_SEQ
	};

	enum XML_Content_Quant {
		XML_CQUANT_NONE,
		XML_CQUANT_OPT,
		XML_CQUANT_REP,
		XML_CQUANT_PLUS
	};

	typedef CDataVectorT<CSAXParserElementDefinition> TListe;

	CSAXParserElementDefinition(XML_Content_Type contentType = XML_CTYPE_EMPTY, XML_Content_Quant contentQuant = XML_CQUANT_NONE, CConstPointer name = NULL);
	virtual ~CSAXParserElementDefinition();

	__inline XML_Content_Type get_contentType() const { return _contentType; }
	__inline void set_contentType(XML_Content_Type t) { _contentType = t; }
	__inline void set_contentType(int t) { _contentType = Cast(XML_Content_Type, t); }
	__inline XML_Content_Quant get_contentQuant() const { return _contentQuant; }
	__inline void set_contentQuant(XML_Content_Quant q) { _contentQuant = q; }
	__inline void set_contentQuant(int q) { _contentQuant = Cast(XML_Content_Quant, q); }
	__inline ConstRef(CStringBuffer) get_name() const { return _name; }
	__inline void set_name(CConstPointer p) { _name.SetString(__FILE__LINE__ p); }

	__inline void AddChild(ConstPtr(CSAXParserElementDefinition) pDef) { _children.Append(pDef); }
	WBool ForEach(TForEachFunc func, Pointer context) const;

protected:
	XML_Content_Type _contentType;
	XML_Content_Quant _contentQuant;
	CStringBuffer _name;
	TListe _children;
};

class CPPSOURCES_API CSAXParserDTDHandler : public CCppObject
{
public:
	CSAXParserDTDHandler();
	virtual ~CSAXParserDTDHandler();

	virtual int NotStandaloneHandler();
	virtual void StartDoctypeDeclHandler(ConstRef(CStringBuffer) doctypeName, ConstRef(CStringBuffer) sysid, ConstRef(CStringBuffer) pubid, int has_internal_subset);
	virtual void EndDoctypeDeclHandler();
	virtual void ElementDeclHandler(ConstRef(CStringBuffer) name, ConstPtr(CSAXParserElementDefinition) def);
	virtual void AttlistDeclHandler(ConstRef(CStringBuffer) elname, ConstRef(CStringBuffer) attname, ConstRef(CStringBuffer) att_type, ConstRef(CStringBuffer) dflt, int isrequired);
	virtual void EntityDeclHandler(ConstRef(CStringBuffer) entityName, int is_parameter_entity, ConstRef(CStringBuffer) value, ConstRef(CStringBuffer) base, ConstRef(CStringBuffer) systemId,
		ConstRef(CStringBuffer) publicId, ConstRef(CStringBuffer) notationName);
	virtual int ExternalEntityRefHandler(ConstRef(CStringBuffer) context, ConstRef(CStringBuffer) base, ConstRef(CStringBuffer) systemId, ConstRef(CStringBuffer) publicId);
	virtual void SkippedEntityHandler(ConstRef(CStringBuffer) entityName, int is_parameter_entity);
	virtual void NotationDeclHandler(ConstRef(CStringBuffer) notationName, ConstRef(CStringBuffer) base, ConstRef(CStringBuffer) systemId, ConstRef(CStringBuffer) publicId);
	virtual void StartNamespaceDeclHandler(ConstRef(CStringBuffer) prefix, ConstRef(CStringBuffer) uri);
	virtual void EndNamespaceDeclHandler(ConstRef(CStringBuffer) prefix);
};

class CPPSOURCES_LOCAL CSAXParserImpl;
class CPPSOURCES_API CSAXParser : public CCppObject
{
public:
	CSAXParser();
	~CSAXParser();

	void Create(Ptr(CSAXParserContentHandler) h = NULL, CConstPointer encoding = NULL);

	Ptr(CSAXParserContentHandler) get_contenthandler() const;
	void set_contenthandler(Ptr(CSAXParserContentHandler) h);

	Ptr(CSAXParserDTDHandler) get_dtdhandler() const;
	void set_dtdhandler(Ptr(CSAXParserDTDHandler) h);

	ConstRef(CStringBuffer) get_encoding() const;
	void set_encoding(CConstPointer encoding);

	void Parse(ConstRef(CFilePath) xmlfilepath);
	void Parse(ConstRef(CByteBuffer) xmldata, bool isFinal = false);

protected:
	Ptr(CSAXParserImpl) _impl;
};

DECL_EXCEPTION(CPPSOURCES_API, CSAXParserException, CBaseException)