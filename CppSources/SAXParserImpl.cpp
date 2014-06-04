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
#include "CPPS_PCH.H"
#include "SAXParserImpl.h"
#include "SAXParser.h"
#include "File.h"

IMPL_EXCEPTION(CSAXParserException, CBaseException)

static void* xml_alloc(size_t sz)
{
	return TFalloc(sz);
}

static void* xml_realloc(void* p, size_t sz)
{
	return TFrealloc(p, sz);
}

static void xml_free(void* p)
{
	if (p)
		TFfree(p);
}


CSAXParserImpl::CSAXParserImpl():
_parser(nullptr), _parsing(false), _contenthandler(nullptr), _dtdhandler(nullptr)
{
}

CSAXParserImpl::~CSAXParserImpl()
{
	if (_parser)
		XML_ParserFree(_parser);
}

void CSAXParserImpl::Create(Ptr(CSAXParserContentHandler) h, CConstPointer encoding)
{
	if (!_parser)
	{
		XML_Memory_Handling_Suite msuite;

		msuite.malloc_fcn = xml_alloc;
		msuite.realloc_fcn = xml_realloc;
		msuite.free_fcn = xml_free;

		_encoding.SetString(__FILE__LINE__ encoding);
		_parser = XML_ParserCreate_MM(encoding, &msuite, _T('\0'));
		if (!_parser)
			throw OK_NEW_OPERATOR CSAXParserException(__FILE__LINE__ _T("[XML_ParserCreate_MM] failed (FATAL ERROR)"));
		XML_UseParserAsHandlerArg(_parser);
		XML_SetUserData(_parser, CastAny(Pointer, this));

		_contenthandler = h;
		XML_SetXmlDeclHandler(_parser, XmlDeclHandler);
		XML_SetProcessingInstructionHandler(_parser, ProcessingInstructionHandler);
		XML_SetElementHandler(_parser, StartElementHandler, EndElementHandler);
		XML_SetCharacterDataHandler(_parser, CharacterDataHandler);
		XML_SetCommentHandler(_parser, CommentHandler);
		XML_SetCdataSectionHandler(_parser, StartCdataSectionHandler, EndCdataSectionHandler);
		XML_SetDefaultHandler(_parser, DefaultHandler);

		XML_SetNotStandaloneHandler(_parser, NotStandaloneHandler);
		XML_SetDoctypeDeclHandler(_parser, StartDoctypeDeclHandler, EndDoctypeDeclHandler);
		XML_SetElementDeclHandler(_parser, ElementDeclHandler);
		XML_SetAttlistDeclHandler(_parser, AttlistDeclHandler);
		XML_SetEntityDeclHandler(_parser, EntityDeclHandler);
		XML_SetExternalEntityRefHandler(_parser, ExternalEntityRefHandler);
		XML_SetSkippedEntityHandler(_parser, SkippedEntityHandler);
		XML_SetNotationDeclHandler(_parser, NotationDeclHandler);
		XML_SetNamespaceDeclHandler(_parser, StartNamespaceDeclHandler, EndNamespaceDeclHandler);

		XML_SetParamEntityParsing(_parser, XML_PARAM_ENTITY_PARSING_ALWAYS);
	}
}

Ptr(CSAXParserContentHandler) CSAXParserImpl::get_contenthandler() const
{
	return _contenthandler;
}

void CSAXParserImpl::set_contenthandler(Ptr(CSAXParserContentHandler) h)
{
	_contenthandler = h;
}

Ptr(CSAXParserDTDHandler) CSAXParserImpl::get_dtdhandler() const
{
	return _dtdhandler;
}

void CSAXParserImpl::set_dtdhandler(Ptr(CSAXParserDTDHandler) h)
{
	_dtdhandler = h;
}

ConstRef(CStringBuffer) CSAXParserImpl::get_encoding() const
{
	return _encoding;
}

void CSAXParserImpl::set_encoding(CConstPointer encoding)
{
	_encoding.SetString(__FILE__LINE__ encoding);
	if ((_parser != nullptr) && (!_parsing))
		XML_SetEncoding(_parser, encoding);
}

void CSAXParserImpl::Parse(ConstRef(CByteBuffer) xmltext, bool isFinal)
{
	_parsing = true;
	XML_Status vStatus = XML_Parse(_parser, CastAnyPtr(char, xmltext.get_Buffer()), xmltext.get_BufferSize(), isFinal?XML_TRUE:XML_FALSE);
	if (vStatus != XML_STATUS_OK)
	{
		const XML_LChar* pErrStr = XML_ErrorString(XML_GetErrorCode(_parser));

		if (pErrStr)
			throw OK_NEW_OPERATOR CSAXParserException(__FILE__LINE__ _T("[XML_Parse] failed (%s)"), pErrStr);
		throw OK_NEW_OPERATOR CSAXParserException(__FILE__LINE__ _T("[XML_Parse] failed (unknown)"));
	}
	if (isFinal)
	{
		XML_ParserFree(_parser);
		_parser = nullptr;
		_parsing = false;
	}
}

#define GetParser(p,u) XML_Parser (p) = (u) ? CastAny(XML_Parser, (u)) : nullptr; if (!(p)) return
#define GetUserData(c,p) Ptr(CSAXParserImpl) c = CastAnyPtr(CSAXParserImpl, XML_GetUserData(p)); if (!(c)) return
#define GetContentHandler(h,c)  Ptr(CSAXParserContentHandler) (h) = (c)->get_contenthandler(); if (!(h)) return
#define GetDTDHandler(h,c)  Ptr(CSAXParserDTDHandler) (h) = (c)->get_dtdhandler(); if (!(h)) return

void XMLCALL CSAXParserImpl::XmlDeclHandler(void *userData,
	const XML_Char *version,
	const XML_Char *encoding,
	int             standalone)
{
	GetParser(parser, userData);
	GetUserData(c, parser);
	GetContentHandler(h, c);

	CStringBuffer vversion(__FILE__LINE__ version);
	CStringBuffer vencoding(__FILE__LINE__ encoding);

	c->set_encoding(encoding);
	h->XmlDeclHandler(vversion, vencoding, standalone);
}

void XMLCALL CSAXParserImpl::ProcessingInstructionHandler(void *userData,
	const XML_Char *target,
	const XML_Char *data)
{
	GetParser(parser, userData);
	GetUserData(c, parser);
	GetContentHandler(h, c);

	CStringBuffer vtarget(__FILE__LINE__ target);
	CStringBuffer vdata(__FILE__LINE__ data);

	h->ProcessingInstructionHandler(vtarget, vdata);
}

void XMLCALL CSAXParserImpl::StartElementHandler(void *userData,
	const XML_Char *name,
	const XML_Char **atts)
{
	GetParser(parser, userData);
	GetUserData(c, parser);
	GetContentHandler(h, c);

	CStringBuffer vname(__FILE__LINE__ name);
	CSAXParserAttributes vatts __FILE__LINE__0P;
	int n = XML_GetSpecifiedAttributeCount(parser);
	int i = 0;

	while (i < n)
	{
		CStringBuffer name(__FILE__LINE__ atts[i++]);
		CStringBuffer value(__FILE__LINE__ atts[i++]);
		Ptr(CSAXParserAttribute) namevalue = OK_NEW_OPERATOR CSAXParserAttribute(name, value);

		vatts.InsertSorted(namevalue);
	}
	h->StartElementHandler(vname, vatts);
}

void XMLCALL CSAXParserImpl::EndElementHandler(void *userData,
	const XML_Char *name)
{
	GetParser(parser, userData);
	GetUserData(c, parser);
	GetContentHandler(h, c);

	CStringBuffer vname(__FILE__LINE__ name);

	h->EndElementHandler(vname);
}

void XMLCALL CSAXParserImpl::CharacterDataHandler(void *userData,
	const XML_Char *s,
	int len)
{
	GetParser(parser, userData);
	GetUserData(c, parser);
	GetContentHandler(h, c);

	CStringBuffer vtext(__FILE__LINE__ s, len);

	h->CharacterDataHandler(vtext);
}

void XMLCALL CSAXParserImpl::CommentHandler(void *userData,
	const XML_Char *data)
{
	GetParser(parser, userData);
	GetUserData(c, parser);
	GetContentHandler(h, c);

	CStringBuffer vtext(__FILE__LINE__ data);

	h->CommentHandler(vtext);
}

void XMLCALL CSAXParserImpl::StartCdataSectionHandler(void *userData)
{
	GetParser(parser, userData);
	GetUserData(c, parser);
	GetContentHandler(h, c);

	h->StartCdataSectionHandler();
}

void XMLCALL CSAXParserImpl::EndCdataSectionHandler(void *userData)
{
	GetParser(parser, userData);
	GetUserData(c, parser);
	GetContentHandler(h, c);

	h->EndCdataSectionHandler();
}

void XMLCALL CSAXParserImpl::DefaultHandler(void *userData,
	const XML_Char *s,
	int len)
{
	GetParser(parser, userData);
	GetUserData(c, parser);
	GetContentHandler(h, c);

	CStringBuffer vtext(__FILE__LINE__ s, len);

	h->DefaultHandler(vtext);
}

int XMLCALL CSAXParserImpl::NotStandaloneHandler(void *userData)
{
	GetParser(parser, userData) XML_STATUS_ERROR;
	GetUserData(c, parser) XML_STATUS_ERROR;
	GetDTDHandler(h, c) XML_STATUS_ERROR;

	return h->NotStandaloneHandler();
}

void XMLCALL CSAXParserImpl::StartDoctypeDeclHandler(
	void *userData,
	const XML_Char *doctypeName,
	const XML_Char *sysid,
	const XML_Char *pubid,
	int has_internal_subset)
{
	GetParser(parser, userData);
	GetUserData(c, parser);
	GetDTDHandler(h, c);

	CStringBuffer vdoctypeName(__FILE__LINE__ doctypeName);
	CStringBuffer vsysid(__FILE__LINE__ sysid);
	CStringBuffer vpubid(__FILE__LINE__ pubid);

	h->StartDoctypeDeclHandler(vdoctypeName, vsysid, vpubid, has_internal_subset);
}

void XMLCALL CSAXParserImpl::EndDoctypeDeclHandler(void *userData)
{
	GetParser(parser, userData);
	GetUserData(c, parser);
	GetDTDHandler(h, c);

	h->EndDoctypeDeclHandler();
}

static void CopyElementDefinition(XML_Content *model, Ptr(CSAXParserElementDefinition) pDef)
{
	assert(model != nullptr);
	assert(pDef != nullptr);

	pDef->set_contentType(model->type);
	pDef->set_contentQuant(model->quant);
	pDef->set_name(model->name);

	for (unsigned int i = 0; i < model->numchildren; ++i)
	{
		Ptr(CSAXParserElementDefinition) pDefChild = OK_NEW_OPERATOR CSAXParserElementDefinition;

		CopyElementDefinition(model->children + i, pDefChild);
		pDef->AddChild(pDefChild);
	}
}

void XMLCALL CSAXParserImpl::ElementDeclHandler(void *userData,
	const XML_Char *name,
	XML_Content *model)
{
	GetParser(parser, userData);
	GetUserData(c, parser);
	GetDTDHandler(h, c);

	CStringBuffer vname(__FILE__LINE__ name);
	Ptr(CSAXParserElementDefinition) pdef = nullptr;

	if (model)
	{
		pdef = OK_NEW_OPERATOR CSAXParserElementDefinition;
		CopyElementDefinition(model, pdef);
		XML_FreeContentModel(parser, model);
	}

	h->ElementDeclHandler(vname, pdef);
	if (pdef)
		pdef->release();
}

void XMLCALL CSAXParserImpl::AttlistDeclHandler(
	void            *userData,
	const XML_Char  *elname,
	const XML_Char  *attname,
	const XML_Char  *att_type,
	const XML_Char  *dflt,
	int              isrequired)
{
	GetParser(parser, userData);
	GetUserData(c, parser);
	GetDTDHandler(h, c);

	CStringBuffer velname(__FILE__LINE__ elname);
	CStringBuffer vattname(__FILE__LINE__ attname);
	CStringBuffer vatt_type(__FILE__LINE__ att_type);
	CStringBuffer vdflt(__FILE__LINE__ dflt);

	h->AttlistDeclHandler(velname, vattname, vatt_type, vdflt, isrequired);
}

void XMLCALL CSAXParserImpl::EntityDeclHandler(
	void *userData,
	const XML_Char *entityName,
	int is_parameter_entity,
	const XML_Char *value,
	int value_length,
	const XML_Char *base,
	const XML_Char *systemId,
	const XML_Char *publicId,
	const XML_Char *notationName)
{
	GetParser(parser, userData);
	GetUserData(c, parser);
	GetDTDHandler(h, c);

	CStringBuffer ventityName(__FILE__LINE__ entityName);
	CStringBuffer vvalue(__FILE__LINE__ value, value_length);
	CStringBuffer vbase(__FILE__LINE__ base);
	CStringBuffer vsystemId(__FILE__LINE__ systemId);
	CStringBuffer vpublicId(__FILE__LINE__ publicId);
	CStringBuffer vnotationName(__FILE__LINE__ notationName);

	h->EntityDeclHandler(ventityName, is_parameter_entity, vvalue, vbase, vsystemId, vpublicId, vnotationName);
}

int XMLCALL CSAXParserImpl::ExternalEntityRefHandler(
	XML_Parser parser,
	const XML_Char *context,
	const XML_Char *base,
	const XML_Char *systemId,
	const XML_Char *publicId)
{
	GetUserData(c, parser) XML_STATUS_ERROR;
	GetDTDHandler(h, c) XML_STATUS_ERROR;

	CStringBuffer vcontext(__FILE__LINE__ context);
	CStringBuffer vbase(__FILE__LINE__ base);
	CStringBuffer vsystemId(__FILE__LINE__ systemId);
	CStringBuffer vpublicId(__FILE__LINE__ publicId);

	XML_Parser extParser = XML_ExternalEntityParserCreate(parser, context, nullptr);

	CFilePath path(vsystemId);
	Ptr(CDiskFile) f = OK_NEW_OPERATOR CDiskFile(path);
	CByteBuffer b(__FILE__LINE__ 1024);

	f->Read(b);
	while (b.get_BufferSize() > 0)
	{
		XML_Parse(extParser, CastAnyPtr(char, b.get_Buffer()), b.get_BufferSize(), XML_FALSE);
		b.set_BufferSize(__FILE__LINE__ 1024);
		f->Read(b);
	}
	XML_Parse(extParser, CastAnyPtr(char, b.get_Buffer()), b.get_BufferSize(), XML_TRUE);
	f->Close();
	f->release();

	XML_ParserFree(extParser);

	return h->ExternalEntityRefHandler(vcontext, vbase, vsystemId, vpublicId);
}

void XMLCALL CSAXParserImpl::SkippedEntityHandler(
	void *userData,
	const XML_Char *entityName,
	int is_parameter_entity)
{
	GetParser(parser, userData);
	GetUserData(c, parser);
	GetDTDHandler(h, c);

	CStringBuffer ventityName(__FILE__LINE__ entityName);

	h->SkippedEntityHandler(ventityName, is_parameter_entity);
}

void XMLCALL CSAXParserImpl::NotationDeclHandler(
	void *userData,
	const XML_Char *notationName,
	const XML_Char *base,
	const XML_Char *systemId,
	const XML_Char *publicId)
{
	GetParser(parser, userData);
	GetUserData(c, parser);
	GetDTDHandler(h, c);

	CStringBuffer vnotationName(__FILE__LINE__ notationName);
	CStringBuffer vbase(__FILE__LINE__ base);
	CStringBuffer vsystemId(__FILE__LINE__ systemId);
	CStringBuffer vpublicId(__FILE__LINE__ publicId);

	h->NotationDeclHandler(vnotationName, vbase, vsystemId, vpublicId);
}

void XMLCALL CSAXParserImpl::StartNamespaceDeclHandler(
	void *userData,
	const XML_Char *prefix,
	const XML_Char *uri)
{
	GetParser(parser, userData);
	GetUserData(c, parser);
	GetDTDHandler(h, c);

	CStringBuffer vprefix(__FILE__LINE__ prefix);
	CStringBuffer vuri(__FILE__LINE__ uri);

	h->StartNamespaceDeclHandler(vprefix, vuri);
}

void XMLCALL CSAXParserImpl::EndNamespaceDeclHandler(
	void *userData,
	const XML_Char *prefix)
{
	GetParser(parser, userData);
	GetUserData(c, parser);
	GetDTDHandler(h, c);

	CStringBuffer vprefix(__FILE__LINE__ prefix);

	h->EndNamespaceDeclHandler(vprefix);
}

