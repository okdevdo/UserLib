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
#include "SAXParser.h"
#include "DirectoryIterator.h"

static bool __stdcall TAttributesForEachFunc(ConstPointer data, Pointer context)
{
	Ptr(CSAXParserAttribute) pAttribute = CastAnyPtr(CSAXParserAttribute, CastMutable(Pointer, data));

	COUT << _T(" ") << pAttribute->get_Name() << _T("=\"") << pAttribute->get_Value() << _T("\"");
	return true;
}

class CSAXParserAttributeForEachFunctor
{
public:
	bool operator()(Ptr(CSAXParserAttribute) pAttribute)
	{
		COUT << _T(" ") << pAttribute->get_Name() << _T("=\"") << pAttribute->get_Value() << _T("\"");
		return true;
	}
};

class TestContentHandler : public CSAXParserContentHandler
{
public:
	TestContentHandler(){}
	virtual ~TestContentHandler(){}

	virtual void XmlDeclHandler(ConstRef(CStringBuffer) version, ConstRef(CStringBuffer) encoding, int standalone)
	{
		COUT << _T("XmlDeclHandler: Version=") << version << _T(", Encoding=") << encoding << _T(", Standalone=") << standalone << endl;
	}
	virtual void ProcessingInstructionHandler(ConstRef(CStringBuffer) target, ConstRef(CStringBuffer) data)
	{
		COUT << _T("ProcessingInstructionHandler: target=") << target << _T(", data=") << data << endl;
	}
	virtual void StartElementHandler(ConstRef(CStringBuffer) name, ConstRef(CSAXParserAttributes) attributes)
	{
		COUT << _T("StartElementHandler: name=") << name;
		attributes.ForEach<CSAXParserAttributeForEachFunctor>();
		COUT << endl;
	}
	virtual void EndElementHandler(ConstRef(CStringBuffer) name)
	{
		COUT << _T("EndElementHandler: name=") << name << endl;
	}
	virtual void CharacterDataHandler(ConstRef(CStringBuffer) text)
	{
		COUT << _T("CharacterDataHandler: text=\"") << text << _T("\"") << endl;
	}
	virtual void CommentHandler(ConstRef(CStringBuffer) text)
	{
		COUT << _T("CommentHandler: text=") << text << endl;
	}
	virtual void StartCdataSectionHandler()
	{
		COUT << _T("StartCdataSectionHandler") << endl;
	}
	virtual void EndCdataSectionHandler()
	{
		COUT << _T("EndCdataSectionHandler") << endl;
	}
	virtual void DefaultHandler(ConstRef(CStringBuffer) text)
	{
		COUT << _T("DefaultHandler: text=\"") << text << _T("\"") << endl;
	}
};

static bool __stdcall TElementDefinitionForEachFunc(ConstPointer data, Pointer context);
static void __stdcall PrintElementDefinition(ConstPtr(CSAXParserElementDefinition) def)
{
	COUT << _T("(");
	switch (def->get_contentType())
	{
	case CSAXParserElementDefinition::XML_CTYPE_EMPTY:
		COUT << _T("XML_CTYPE_EMPTY");
		break;
	case CSAXParserElementDefinition::XML_CTYPE_ANY:
		COUT << _T("XML_CTYPE_ANY");
		break;
	case CSAXParserElementDefinition::XML_CTYPE_MIXED:
		COUT << _T("XML_CTYPE_MIXED");
		break;
	case CSAXParserElementDefinition::XML_CTYPE_NAME:
		COUT << _T("XML_CTYPE_NAME");
		break;
	case CSAXParserElementDefinition::XML_CTYPE_CHOICE:
		COUT << _T("XML_CTYPE_CHOICE");
		break;
	case CSAXParserElementDefinition::XML_CTYPE_SEQ:
		COUT << _T("XML_CTYPE_SEQ");
		break;
	}
	COUT << _T(", ");
	switch (def->get_contentQuant())
	{
	case CSAXParserElementDefinition::XML_CQUANT_NONE:
		COUT << _T("XML_CQUANT_NONE");
		break;
	case CSAXParserElementDefinition::XML_CQUANT_OPT:
		COUT << _T("XML_CQUANT_OPT");
		break;
	case CSAXParserElementDefinition::XML_CQUANT_REP:
		COUT << _T("XML_CQUANT_REP");
		break;
	case CSAXParserElementDefinition::XML_CQUANT_PLUS:
		COUT << _T("XML_CQUANT_PLUS");
		break;
	}
	COUT << _T(", ") << def->get_name();
	def->ForEach(TElementDefinitionForEachFunc, NULL);
	COUT << _T(")");
}

static bool __stdcall TElementDefinitionForEachFunc(ConstPointer data, Pointer context)
{
	Ptr(CSAXParserElementDefinition) pDef = CastAnyPtr(CSAXParserElementDefinition, CastMutable(Pointer, data));

	PrintElementDefinition(pDef);
	return true;
}

class TestDTDHandler : public CSAXParserDTDHandler
{
public:
	TestDTDHandler(){}
	virtual ~TestDTDHandler(){}

	virtual int NotStandaloneHandler() 
	{ 
		COUT << _T("NotStandaloneHandler") << endl;
		return 1; 
	}
	virtual void StartDoctypeDeclHandler(ConstRef(CStringBuffer) doctypeName, ConstRef(CStringBuffer) sysid, ConstRef(CStringBuffer) pubid, int has_internal_subset)
	{
		COUT << _T("StartDoctypeDeclHandler: doctypeName=") << doctypeName << _T(", sysid=") << sysid << _T(", pubid=") << pubid << _T(", has_internal_subset=") << has_internal_subset << endl;
	}
	virtual void EndDoctypeDeclHandler()
	{
		COUT << _T("EndDoctypeDeclHandler") << endl;
	}
	virtual void ElementDeclHandler(ConstRef(CStringBuffer) name, ConstPtr(CSAXParserElementDefinition) def)
	{
		COUT << _T("ElementDeclHandler: name=") << name;
		if (def)
			PrintElementDefinition(def);
		COUT << endl;
	}
	virtual void AttlistDeclHandler(ConstRef(CStringBuffer) elname, ConstRef(CStringBuffer) attname, ConstRef(CStringBuffer) att_type, ConstRef(CStringBuffer) dflt, int isrequired)
	{
		COUT << _T("AttlistDeclHandler: elname=") << elname << _T(", attname=") << attname << _T(", att_type=") << att_type << _T(", dflt=") << dflt << _T(", isrequired=") << isrequired << endl;
	}
	virtual void EntityDeclHandler(ConstRef(CStringBuffer) entityName, int is_parameter_entity, ConstRef(CStringBuffer) value, ConstRef(CStringBuffer) base, ConstRef(CStringBuffer) systemId,
		ConstRef(CStringBuffer) publicId, ConstRef(CStringBuffer) notationName)
	{
		COUT << _T("EntityDeclHandler: entityName=") << entityName << _T(", is_parameter_entity=") << is_parameter_entity << _T(", value=") << value 
			<< _T(", base=") << base << _T(", systemId=") << systemId << _T(", publicId=") << publicId << _T(", notationName=") << notationName << endl;
	}
	virtual int ExternalEntityRefHandler(ConstRef(CStringBuffer) context, ConstRef(CStringBuffer) base, ConstRef(CStringBuffer) systemId, ConstRef(CStringBuffer) publicId) 
	{ 
		COUT << _T("ExternalEntityRefHandler: context=") << context << _T(", base=") << base << _T(", systemId=") << systemId << _T(", publicId=") << publicId << endl;
		return 1; 
	}
	virtual void SkippedEntityHandler(ConstRef(CStringBuffer) entityName, int is_parameter_entity)
	{
		COUT << _T("SkippedEntityHandler: entityName=") << entityName << _T(", is_parameter_entity=") << is_parameter_entity << endl;
	}
	virtual void NotationDeclHandler(ConstRef(CStringBuffer) notationName, ConstRef(CStringBuffer) base, ConstRef(CStringBuffer) systemId, ConstRef(CStringBuffer) publicId)
	{
		COUT << _T("NotationDeclHandler: notationName=") << notationName << _T(", base=") << base << _T(", systemId=") << systemId << _T(", publicId=") << publicId << endl;
	}
	virtual void StartNamespaceDeclHandler(ConstRef(CStringBuffer) prefix, ConstRef(CStringBuffer) uri)
	{
		COUT << _T("StartNamespaceDeclHandler: prefix=") << prefix << _T(", uri=") << uri << endl;
	}
	virtual void EndNamespaceDeclHandler(ConstRef(CStringBuffer) prefix)
	{
		COUT << _T("StartNamespaceDeclHandler: prefix=") << prefix << endl;
	}
};

void TestSAXParser()
{
#ifdef OK_SYS_WINDOWS
	CFilePath dir(__FILE__LINE__ _T("C:\\selfhtml812\\xml\\dtd\\anzeige\\"));
#endif
#ifdef OK_SYS_UNIX
	CFilePath dir(__FILE__LINE__ _T("/home/Oliver"));
#endif
	CDirectoryIterator::SetCurrentDirectory(dir);
//	CFilePath path(_T("personendatenbank.xml"));
	CFilePath path(__FILE__LINE__ _T("buch.xml"));
	CSAXParser parser;
	TestContentHandler contenthandler;
	TestDTDHandler dtdhandler;

	parser.Create();
	parser.set_contenthandler(&contenthandler);
	parser.set_dtdhandler(&dtdhandler);

	parser.Parse(path);
}