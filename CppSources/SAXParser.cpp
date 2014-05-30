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
#include "SAXParser.h"
#include "SAXParserImpl.h"
#include "File.h"

CSAXParserAttribute::CSAXParserAttribute()
{
}

CSAXParserAttribute::CSAXParserAttribute(ConstRef(CStringBuffer) name, ConstRef(CStringBuffer) value) :
_name(name), _value(value)
{
}

CSAXParserAttribute::~CSAXParserAttribute()
{
}

static void __stdcall SAXParserAttributeDeleteFunc(ConstPointer data, Pointer context)
{
	Ptr(CSAXParserAttribute) pAttribute = CastAnyPtr(CSAXParserAttribute, CastMutable(Pointer, data));

	pAttribute->release();
}

static sword __stdcall SAXParserAttributeSearchAndSortFunc(ConstPointer ArrayItem, ConstPointer DataItem)
{
	Ptr(CSAXParserAttribute) pArrayItem = CastAnyPtr(CSAXParserAttribute, CastMutable(Pointer, ArrayItem));
	Ptr(CSAXParserAttribute) pDataItem = CastAnyPtr(CSAXParserAttribute, CastMutable(Pointer, DataItem));

	return pArrayItem->get_Name().Compare(pDataItem->get_Name());
}

CSAXParserAttributes::CSAXParserAttributes(DECL_FILE_LINE0) :
	super(ARGS_FILE_LINE 16, 16)
{
}

CSAXParserAttributes::~CSAXParserAttributes()
{
}

CSAXParserContentHandler::CSAXParserContentHandler() {}
CSAXParserContentHandler::~CSAXParserContentHandler() {}

void CSAXParserContentHandler::XmlDeclHandler(ConstRef(CStringBuffer) version, ConstRef(CStringBuffer) encoding, int standalone) {}
void CSAXParserContentHandler::ProcessingInstructionHandler(ConstRef(CStringBuffer) target, ConstRef(CStringBuffer) data) {}
void CSAXParserContentHandler::StartElementHandler(ConstRef(CStringBuffer) name, ConstRef(CSAXParserAttributes) attributes) {}
void CSAXParserContentHandler::EndElementHandler(ConstRef(CStringBuffer) name) {}
void CSAXParserContentHandler::CharacterDataHandler(ConstRef(CStringBuffer) text) {}
void CSAXParserContentHandler::CommentHandler(ConstRef(CStringBuffer) text) {}
void CSAXParserContentHandler::StartCdataSectionHandler() {}
void CSAXParserContentHandler::EndCdataSectionHandler() {}
void CSAXParserContentHandler::DefaultHandler(ConstRef(CStringBuffer) text) {}

static void __stdcall SAXParseElementDefinitionDeleteFunc(ConstPointer data, Pointer context)
{
	Ptr(CSAXParserElementDefinition) pDefinition = CastAnyPtr(CSAXParserElementDefinition, CastMutable(Pointer, data));

	pDefinition->release();
}

CSAXParserElementDefinition::CSAXParserElementDefinition(XML_Content_Type contentType, XML_Content_Quant contentQuant, CConstPointer name) :
_contentType(contentType), _contentQuant(contentQuant), _name(__FILE__LINE__ name), _children(__FILE__LINE__ 16, 32)
{}
CSAXParserElementDefinition::~CSAXParserElementDefinition() {}

WBool CSAXParserElementDefinition::ForEach(TForEachFunc func, Pointer context) const
{
	TListe::Iterator it = _children.Begin();

	while (it)
	{
		if (!func(*it, context))
			return false;
		++it;
	}
	return true;
}

CSAXParserDTDHandler::CSAXParserDTDHandler(){}
CSAXParserDTDHandler::~CSAXParserDTDHandler(){}

int CSAXParserDTDHandler::NotStandaloneHandler(){ return XML_STATUS_ERROR; }
void CSAXParserDTDHandler::StartDoctypeDeclHandler(ConstRef(CStringBuffer) doctypeName, ConstRef(CStringBuffer) sysid, ConstRef(CStringBuffer) pubid, int has_internal_subset){}
void CSAXParserDTDHandler::EndDoctypeDeclHandler(){}
void CSAXParserDTDHandler::ElementDeclHandler(ConstRef(CStringBuffer) name, ConstPtr(CSAXParserElementDefinition) def){}
void CSAXParserDTDHandler::AttlistDeclHandler(ConstRef(CStringBuffer) elname, ConstRef(CStringBuffer) attname, ConstRef(CStringBuffer) att_type, ConstRef(CStringBuffer) dflt, int isrequired){}
void CSAXParserDTDHandler::EntityDeclHandler(ConstRef(CStringBuffer) entityName, int is_parameter_entity, ConstRef(CStringBuffer) value, ConstRef(CStringBuffer) base, ConstRef(CStringBuffer) systemId,
	ConstRef(CStringBuffer) publicId, ConstRef(CStringBuffer) notationName){}
int CSAXParserDTDHandler::ExternalEntityRefHandler(ConstRef(CStringBuffer) context, ConstRef(CStringBuffer) base, ConstRef(CStringBuffer) systemId, ConstRef(CStringBuffer) publicId){ return XML_STATUS_ERROR; }
void CSAXParserDTDHandler::SkippedEntityHandler(ConstRef(CStringBuffer) entityName, int is_parameter_entity){}
void CSAXParserDTDHandler::NotationDeclHandler(ConstRef(CStringBuffer) notationName, ConstRef(CStringBuffer) base, ConstRef(CStringBuffer) systemId, ConstRef(CStringBuffer) publicId){}
void CSAXParserDTDHandler::StartNamespaceDeclHandler(ConstRef(CStringBuffer) prefix, ConstRef(CStringBuffer) uri){}
void CSAXParserDTDHandler::EndNamespaceDeclHandler(ConstRef(CStringBuffer) prefix){}

CSAXParser::CSAXParser() :
_impl(NULL)
{
}

CSAXParser::~CSAXParser()
{
	if (_impl)
		delete _impl;
}

void CSAXParser::Create(Ptr(CSAXParserContentHandler) h, CConstPointer encoding)
{
	if (PtrCheck(_impl))
		_impl = OK_NEW_OPERATOR CSAXParserImpl();
	_impl->Create(h, encoding);
}

Ptr(CSAXParserContentHandler) CSAXParser::get_contenthandler() const
{
	if (_impl)
		return _impl->get_contenthandler();
	return NULL;
}

void CSAXParser::set_contenthandler(Ptr(CSAXParserContentHandler) h)
{
	if (_impl)
		_impl->set_contenthandler(h);
}

Ptr(CSAXParserDTDHandler) CSAXParser::get_dtdhandler() const
{
	if (_impl)
		return _impl->get_dtdhandler();
	return NULL;
}

void CSAXParser::set_dtdhandler(Ptr(CSAXParserDTDHandler) h)
{
	if (_impl)
		_impl->set_dtdhandler(h);
}

ConstRef(CStringBuffer) CSAXParser::get_encoding() const
{
	if (_impl)
		return _impl->get_encoding();
	return CStringBuffer::null();
}

void CSAXParser::set_encoding(CConstPointer encoding)
{
	if (_impl)
		_impl->set_encoding(encoding);
}

void CSAXParser::Parse(ConstRef(CFilePath) xmlfilepath)
{
	if (PtrCheck(_impl))
		return;

	Ptr(CDiskFile) f = OK_NEW_OPERATOR CDiskFile(xmlfilepath);
	CByteBuffer b(__FILE__LINE__ 1024);

	f->Read(b);
	while (b.get_BufferSize() > 0)
	{
		_impl->Parse(b);
		b.set_BufferSize(__FILE__LINE__ 1024);
		f->Read(b);
	}
	_impl->Parse(b, true);
	f->Close();
	f->release();
}

void CSAXParser::Parse(ConstRef(CByteBuffer) xmldata, bool isFinal)
{
	if (_impl)
		_impl->Parse(xmldata, isFinal);
}

