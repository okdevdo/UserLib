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
#include "EventLogRecord.h"
#include "STRUTIL.H"
#include "EventLogChannel.h"
#include "EventLogProvider.h"
#include "EventLogEvent.h"
#include "FilePath.h"
#include "File.h"
#include "DirectoryIterator.h"
#include "SAXParser.h"

#include <winevt.h>
#include <objbase.h>
#include <Sddl.h>

class TestEventLogRecordForEachFunctor
{
public:
	bool operator()(Ptr(CEventLogRecord) pRecord)
	{
		pRecord->Print(CEventLogRecord::XMLOutput, xmlfile);
		return true;
	}

	Ptr(CFile) xmlfile;
};

static bool __stdcall TestEventLogRecordForEachFunc(ConstPointer data, Pointer context)
{
	Ptr(CEventLogRecord) pRecord = CastAnyPtr(CEventLogRecord, CastMutable(Pointer, data));

	pRecord->Print(CEventLogRecord::XMLOutput, CastAnyPtr(CFile, context));
	return true;
}

static void TestEventLogRecord()
{
	CEventLogRecords records __FILE__LINE__0P;
	TestEventLogRecordForEachFunctor arg;
	CDiskFile xmlFile;
	CStringBuffer sBuf;
	CByteBuffer bBuf;

	try
	{
		records.Load();
		xmlFile.Create(CFilePath(__FILE__LINE__ _T("C:\\Users\\Oliver\\Documents\\eventlogrecords.xml")), false, CFile::UTF_16LE_Encoding);
		sBuf.convertToUTF16(bBuf);
		xmlFile.Write(bBuf);
		CEventLogRecord::PrintXMLHeader(_T("EventRecords"), &xmlFile);
		arg.xmlfile = &xmlFile;
		records.ForEach<TestEventLogRecordForEachFunctor>(arg);
		CEventLogRecord::PrintXMLFooter(_T("EventRecords"), &xmlFile);
		xmlFile.Close();
	}
	catch (CBaseException* ex)
	{
		xmlFile.Close();
		CERR << ex->GetExceptionMessage() << endl;
	}
}

static void PrintUserData(Ptr(CFile) fileOut, DWORD dwPropCount, PEVT_VARIANT pProp)
{
	CStringBuffer value;

	for (DWORD i = 0; i < dwPropCount; ++i)
	{
		if (pProp[i].Type & EVT_VARIANT_TYPE_ARRAY)
			continue;
		value.SetString(__FILE__LINE__ _T(""));
		switch (pProp[i].Type & EVT_VARIANT_TYPE_MASK)
		{
		case EvtVarTypeNull:
			value.SetString(__FILE__LINE__ _T("(typenull)"));
			break;
		case EvtVarTypeString:
			value.SetString(__FILE__LINE__ pProp[i].StringVal);
			break;
		case EvtVarTypeAnsiString:
			value.FormatString(__FILE__LINE__ _T("%hs"), pProp[i].AnsiStringVal);
			break;
		case EvtVarTypeSByte:
			value.FormatString(__FILE__LINE__ _T("%d"), (sdword)(pProp[i].SByteVal));
			break;
		case EvtVarTypeByte:
			value.FormatString(__FILE__LINE__ _T("%u"), (dword)(pProp[i].SByteVal));
			break;
		case EvtVarTypeInt16:
			value.FormatString(__FILE__LINE__ _T("%d"), (sdword)(pProp[i].Int16Val));
			break;
		case EvtVarTypeUInt16:
			value.FormatString(__FILE__LINE__ _T("%u"), (dword)(pProp[i].UInt16Val));
			break;
		case EvtVarTypeInt32:
			value.FormatString(__FILE__LINE__ _T("%d"), (sdword)(pProp[i].Int32Val));
			break;
		case EvtVarTypeUInt32:
			value.FormatString(__FILE__LINE__ _T("%u"), (dword)(pProp[i].UInt32Val));
			break;
		case EvtVarTypeInt64:
			value.FormatString(__FILE__LINE__ _T("%lld"), (sqword)(pProp[i].Int64Val));
			break;
		case EvtVarTypeUInt64:
			value.FormatString(__FILE__LINE__ _T("%llu"), (qword)(pProp[i].UInt64Val));
			break;
		case EvtVarTypeSingle:
			value.FormatString(__FILE__LINE__ _T("%g"), (double)(pProp[i].SingleVal));
			break;
		case EvtVarTypeDouble:
			value.FormatString(__FILE__LINE__ _T("%g"), (double)(pProp[i].DoubleVal));
			break;
		case EvtVarTypeBoolean:
			value.FormatString(__FILE__LINE__ _T("%s"), (pProp[i].BooleanVal) ? _T("True") : _T("False"));
			break;
		case EvtVarTypeBinary:
			break;
		case EvtVarTypeGuid:
			if (NULL != pProp[i].GuidVal)
			{
				TCHAR wsGuid[50];

				StringFromGUID2(*(pProp[i].GuidVal), wsGuid, sizeof(wsGuid) / sizeof(TCHAR));
				value.SetString(__FILE__LINE__ wsGuid);
			}
			break;
		case EvtVarTypeSizeT:
			break;
		case EvtVarTypeFileTime:
		{
								   ULONGLONG ullTimeStamp = 0;
								   ULONGLONG ullNanoseconds = 0;
								   SYSTEMTIME st;
								   FILETIME ft;

								   ullTimeStamp = pProp[i].FileTimeVal;
								   ft.dwHighDateTime = (DWORD)((ullTimeStamp >> 32) & 0xFFFFFFFF);
								   ft.dwLowDateTime = (DWORD)(ullTimeStamp & 0xFFFFFFFF);

								   FileTimeToSystemTime(&ft, &st);
								   ullNanoseconds = (ullTimeStamp % 10000000) * 100; // Display nanoseconds instead of milliseconds for higher resolution
								   value.FormatString(__FILE__LINE__ _T("%02hd.%02hd.%04hd %02hd:%02hd:%02hd"), st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);
		}
			break;
		case EvtVarTypeSysTime:
			value.FormatString(__FILE__LINE__ _T("%02hd.%02hd.%04hd %02hd:%02hd:%02hd"), pProp[i].SysTimeVal->wDay, pProp[i].SysTimeVal->wMonth, pProp[i].SysTimeVal->wYear, pProp[i].SysTimeVal->wHour, pProp[i].SysTimeVal->wMinute, pProp[i].SysTimeVal->wSecond);
			break;
		case EvtVarTypeSid:
		{
							  LPTSTR pwsSid = NULL;

							  if (ConvertSidToStringSid(pProp[i].SidVal, &pwsSid))
							  {
								  value.SetString(__FILE__LINE__ pwsSid);
								  LocalFree(pwsSid);
							  }
		}
			break;
		case EvtVarTypeHexInt32:
			value.FormatString(__FILE__LINE__ _T("0x%x"), (dword)(pProp[i].UInt32Val));
			break;
		case EvtVarTypeHexInt64:
			value.FormatString(__FILE__LINE__ _T("0x%llx"), (qword)(pProp[i].UInt64Val));
			break;
		default:
			break;
		}
		fileOut->Write(_T("Data%d: %s\n"), i, value.GetString());
	}
}

bool __stdcall CEventLogEventForEachFunc(ConstPointer data, Pointer context)
{
	CEventLogEvent* pInfo = CastAnyPtr(CEventLogEvent, CastMutable(Pointer, data));
	Ptr(CFile) fileOut = CastAnyPtr(CFile, context);

	fileOut->Write(_T("== EventRecordID: %lld ==========\n"), pInfo->get_eventRecordID());
	fileOut->Write(_T("Provider: %s\n"), pInfo->get_providerName());
	fileOut->Write(_T("EventID: %d, (%08x)\n"), pInfo->get_eventID(), pInfo->get_eventID());
	fileOut->Write(_T("Qualifiers: %08x\n"), pInfo->get_qualifiers());
	fileOut->Write(_T("Version: %08x\n"), pInfo->get_version());
	fileOut->Write(_T("Level: %08x, %s, %s\n"), pInfo->get_level(), pInfo->get_levelName().GetString(), pInfo->get_levelMessage().GetString());
	fileOut->Write(_T("Task: %08x, %s, %s, %s\n"), pInfo->get_task(), pInfo->get_taskName().GetString(), pInfo->get_taskMessage().GetString(), pInfo->get_taskEventGuid().GetString());
	fileOut->Write(_T("OpCode: %08x, %s, %s\n"), pInfo->get_opCode(), pInfo->get_opCodeName().GetString(), pInfo->get_opCodeMessage().GetString());
	fileOut->Write(_T("KeyWords: %016llx, %s, %s\n"), pInfo->get_keyWords(), pInfo->get_keyWordsName().GetString(), pInfo->get_keyWordsMessage().GetString());
	fileOut->Write(_T("Template: %s\n"), pInfo->get_template().GetString());
	PrintUserData(fileOut, pInfo->get_userDataCount(), pInfo->get_userData());
	fileOut->Write(_T("Message: %08x, %s\n"), pInfo->get_messageID(), pInfo->get_message().GetString());
	return TRUE;
}

class CMyCSAXParserHandlers : public CSAXParserContentHandler
{
public:
	CMyCSAXParserHandlers() : _info(NULL), _event(NULL), _state(0), _eventRecordID(0LL), _originmessage(), _fileOut(NULL) {}
	virtual ~CMyCSAXParserHandlers() {}

	double Compare(Ref(CStringConstIterator) ita, Ref(CStringConstIterator) itb)
	{
		int result = 0;

		while (!(ita.IsEnd() || itb.IsEnd()))
		{
			if ((itb.GetDistance() > 0) && (itb[0] == _T('?')) && (s_isdigit(itb[1]) || s_isdigit(itb[-1])))
				++itb;
			if (ita[0] != itb[0])
			{

				break;
			}
			++ita; ++itb;
			++result;
		}
		if (ita.IsEnd() && itb.IsEnd())
			return 100.0;
		if (ita.IsEnd())
			return Cast(double, result * 100) / Cast(double, result + itb.GetLength());
		return 0.0;
	}

	void CompareMessage()
	{
		CStringConstIterator ita(_event->get_message());
		CStringConstIterator itb(_originmessage);

		while (!(ita.IsEnd()))
		{
			if (ita[0] == itb[0])
				break;
			++ita;
		}
		_fileOut->Write(_T("\n"));
		if (ita.IsEnd())
		{
			_fileOut->Write(_T("\n******* ERROR: Messages are different **********\n\n"));
			return;
		}

		double cmp = Compare(ita, itb);

		if (cmp < 100.0)
			_fileOut->Write(_T("\n******* ERROR: Messages are different (%g) **********\n\n"), cmp);
	}

	virtual void XmlDeclHandler(ConstRef(CStringBuffer) version, ConstRef(CStringBuffer) encoding, int standalone){}
	virtual void ProcessingInstructionHandler(ConstRef(CStringBuffer) target, ConstRef(CStringBuffer) data){}
	virtual void StartElementHandler(ConstRef(CStringBuffer) name, ConstRef(CSAXParserAttributes) attributes)
	{
		//		_tprintf(_T("%s\n"), name.GetString());
		if (3 == _state)
			CompareMessage();
		_state = 0;
		if (CStringLiteral(_T("EventRecordID")) == name)
			_state = 1;
		else if (CStringLiteral(_T("Message")) == name)
			_state = 2;
	}
	virtual void EndElementHandler(ConstRef(CStringBuffer) name)
	{
		//		_tprintf(_T("%s\n"), name.GetString());
		//		if (CStringLiteral(_T("Event")) == name)
		if (3 == _state)
			CompareMessage();
		_state = 0;
	}
	virtual void CharacterDataHandler(ConstRef(CStringBuffer) text)
	{
		//		_tprintf(_T("%s\n"), text.GetString());
		switch (_state)
		{
		case 0:
			break;
		case 1:
			text.ScanString(_T("%lld"), &_eventRecordID);
			_state = 0;
			break;
		case 2:
		{
				  _state = 0;
				  _event = _info->get_events().FindSorted(_eventRecordID);
				  if (_event && (_event->get_eventRecordID() == _eventRecordID))
				  {
					  CEventLogEventForEachFunc(_event, _fileOut);
					  if (!(text.IsEmpty()))
					  {
						  _fileOut->Write(_T("Origin Message: %s"), text.GetString());
						  _originmessage = text;
						  _state = 3;
					  }
				  }
		}
			break;
		case 3:
			_fileOut->Write(_T("%s"), text.GetString());
			_originmessage.AppendString(text);
			break;
		}
	}
	virtual void CommentHandler(ConstRef(CStringBuffer) text){}
	virtual void StartCdataSectionHandler(){}
	virtual void EndCdataSectionHandler(){}
	virtual void DefaultHandler(ConstRef(CStringBuffer) text){}

	Ptr(CEventLogChannel) _info;
	Ptr(CEventLogEvent) _event;
	int _state;
	DWORD64 _eventRecordID;
	CStringBuffer _originmessage;
	Ptr(CFile) _fileOut;
};

class CEventLogChannelForEachFunctor
{
public:
	bool operator()(Ptr(CEventLogChannel) pInfo)
	{
		CStringBuffer name(pInfo->get_name());
		CFilePath fname;
		CSAXParser parser;
		Ptr(CMyCSAXParserHandlers) handlers = OK_NEW_OPERATOR CMyCSAXParserHandlers;
		CStringBuffer name1(pInfo->get_name());
		CFilePath fname1;
		Ptr(CFile) f = NULL;

		name.ReplaceString(_T("/"), _T("_"));
		name.PrependString(_T("C:\\Users\\Oliver\\Documents\\Visual Studio 2013\\Projects\\EventQueryVista\\wevtutil\\"));
		name.AppendString(_T(".xml"));
		fname.set_Path(name);

		parser.Create(handlers, _T("utf-16"));
		handlers->_info = pInfo;
		try
		{
			if (CDirectoryIterator::FileExists(fname))
			{
				name1.ReplaceString(_T("/"), _T("_"));
				name1.PrependString(_T("C:\\Users\\Oliver\\Documents\\Visual Studio 2013\\Projects\\EventQueryVista\\output\\"));
				name1.AppendString(_T(".log"));
				fname1.set_Path(name1);

				f = OK_NEW_OPERATOR CStreamFile;
				f->Create(fname1, true, CFile::UTF_8_Encoding);
				f->Write(_T("** Channel Name: %s **********\n"), pInfo->get_name().GetString());
				handlers->_fileOut = f;

				pInfo->LoadEvents(providers);
				parser.Parse(fname);
			}
		}
		catch (CFileException*)
		{
		}
		catch (CBaseException* ex)
		{
			CERR << ex->GetExceptionMessage() << endl;
		}
		if (f)
		{
			f->Close();
			f->release();
		}
		delete handlers;

		return true;
	}

	Ptr(CEventLogProviders) providers;
};

bool __stdcall CEventLogChannelForEachFunc(ConstPointer data, Pointer context)
{
	CEventLogChannel* pInfo = CastAnyPtr(CEventLogChannel, CastMutable(Pointer, data));
	CStringBuffer name(pInfo->get_name());
	CFilePath fname;
	CSAXParser parser;
	Ptr(CMyCSAXParserHandlers) handlers = OK_NEW_OPERATOR CMyCSAXParserHandlers;
	CStringBuffer name1(pInfo->get_name());
	CFilePath fname1;
	Ptr(CFile) f = NULL;

	name.ReplaceString(_T("/"), _T("_"));
	name.PrependString(_T("C:\\Users\\Oliver\\Documents\\Visual Studio 2013\\Projects\\EventQueryVista\\wevtutil\\"));
	name.AppendString(_T(".xml"));
	fname.set_Path(name);

	parser.Create(handlers, _T("utf-16"));
	handlers->_info = pInfo;
	try
	{
		if (CDirectoryIterator::FileExists(fname))
		{
			name1.ReplaceString(_T("/"), _T("_"));
			name1.PrependString(_T("C:\\Users\\Oliver\\Documents\\Visual Studio 2013\\Projects\\EventQueryVista\\output\\"));
			name1.AppendString(_T(".log"));
			fname1.set_Path(name1);

			f = OK_NEW_OPERATOR CStreamFile;
			f->Create(fname1, true, CFile::UTF_8_Encoding);
			f->Write(_T("** Channel Name: %s **********\n"), pInfo->get_name().GetString());
			handlers->_fileOut = f;

			pInfo->LoadEvents(CastAnyPtr(CEventLogProviders, context));
			parser.Parse(fname);
		}
	}
	catch (CFileException*)
	{
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
	}
	if (f)
	{
		f->Close();
		f->release();
	}
	delete handlers;

	return true;
}

class CEventLogProviderForEachFunctor
{
public:
	bool operator()(Ptr(CEventLogProvider) pInfo)
	{
		CStringBuffer spath(__FILE__LINE__ _T("C:\\Users\\Oliver\\Documents\\Visual Studio 2013\\Projects\\EventQueryVista\\output\\"));
		CFilePath fpath;
		Ptr(CFile) f = OK_NEW_OPERATOR CStreamFile;
		int i;

		spath.AppendString(pInfo->get_name());
		spath.AppendString(_T(".txt"));
		spath.ReplaceString(_T("/"), _T("_"));
		fpath.set_Path(spath);
		f->Create(fpath, true, CFile::UTF_8_Encoding);

		f->Write(_T("** Provider Name: %s **********\n"), pInfo->get_name().GetString());
		f->Write(_T("Provider Guid: %s\n"), pInfo->get_guid().GetString());
		f->Write(_T("Help link: %s\n"), pInfo->get_helpLink().GetString());
		f->Write(_T("MessageFile: %s\n"), pInfo->get_messageFile().GetString());
		f->Write(_T("ParameterFile: %s\n"), pInfo->get_parameterFile().GetString());
		f->Write(_T("ResourceFile: %s\n"), pInfo->get_resourceFile().GetString());

		CEventLogProviderChannels::Iterator channelListIt = pInfo->get_channelList().Begin();

		i = 0;
		while (channelListIt)
		{
			f->Write(_T("Channel%d: Id=%d, Path=%s, Index=%d, MessageID=%d, Message=%s, Imported=%s\n"), i, (*channelListIt)->get_id(),
				(*channelListIt)->get_path().GetString(), (*channelListIt)->get_index(), (*channelListIt)->get_messageID(), (*channelListIt)->get_message().GetString(), (*channelListIt)->get_imported() ? _T("True") : _T("False"));
			++channelListIt;
			++i;
		}

		CEventLogProviderLevels::Iterator levelListIt = pInfo->get_levelList().Begin();

		i = 0;
		while (levelListIt)
		{
			f->Write(_T("Level%d: Value=0x%08x, Name=%s, MessageID=%d, Message=%s\n"), i, (*levelListIt)->get_value(),
				(*levelListIt)->get_name().GetString(), (*levelListIt)->get_messageID(), (*levelListIt)->get_message().GetString());
			++levelListIt;
			++i;
		}

		CEventLogProviderTasks::Iterator taskListIt = pInfo->get_taskList().Begin();

		i = 0;
		while(taskListIt)
		{
			f->Write(_T("Task%d: Value=0x%08x, Name=%s, EventGuid=%s, MessageID=%d, Message=%s\n"), i, (*taskListIt)->get_value(),
				(*taskListIt)->get_name().GetString(), (*taskListIt)->get_eventGuid().GetString(), (*taskListIt)->get_messageID(), (*taskListIt)->get_message().GetString());
			++taskListIt;
			++i;
		}

		CEventLogProviderOpCodes::Iterator opCodeListIt = pInfo->get_opCodeList().Begin();

		i = 0;
		while (opCodeListIt)
		{
			f->Write(_T("OpCode%d: Value=0x%08x, Name=%s, MessageID=%d, Message=%s\n"), i, (*opCodeListIt)->get_value(),
				(*opCodeListIt)->get_name().GetString(), (*opCodeListIt)->get_messageID(), (*opCodeListIt)->get_message().GetString());
			++opCodeListIt;
			++i;
		}

		CEventLogProviderKeyWords::Iterator keyWordsListIt = pInfo->get_keyWordList().Begin();

		i = 0;
		while (keyWordsListIt)
		{
			f->Write(_T("KeyWords%d: Value=0x%016llx, Name=%s, MessageID=%d, Message=%s\n"), i, (*keyWordsListIt)->get_value(),
				(*keyWordsListIt)->get_name().GetString(), (*keyWordsListIt)->get_messageID(), (*keyWordsListIt)->get_message().GetString());
			++keyWordsListIt;
			++i;
		}

		CEventLogProviderEvents::Iterator eventListIt = pInfo->get_eventList().Begin();

		i = 0;
		while (eventListIt)
		{
			f->Write(_T("Event%d: Id=(%d, 0x%08x), Version=%d, Channel=%d, Level=0x%08x, OpCode=0x%08x, Task=0x%08x, KeyWord=0x%016llx\n"), i, (*eventListIt)->get_id(), (*eventListIt)->get_id(),
				(*eventListIt)->get_version(), (*eventListIt)->get_channelValue(), (*eventListIt)->get_levelValue(), (*eventListIt)->get_opCodeValue(), (*eventListIt)->get_taskValue(), (*eventListIt)->get_keyWordValue());
			f->Write(_T("        MessageId=%08x, Message=%s\n"), (*eventListIt)->get_messageID(), (*eventListIt)->get_message().GetString());
			f->Write(_T("        Template=%s\n"), (*eventListIt)->get_template().GetString());
			++eventListIt;
			++i;
		}
		f->Close();

		return true;
	}
};

bool __stdcall CEventLogProviderForEachFunc(ConstPointer data, Pointer context)
{
	CEventLogProvider* pInfo = CastAnyPtr(CEventLogProvider, CastMutable(Pointer, data));
	CStringBuffer spath(__FILE__LINE__ _T("C:\\Users\\Oliver\\Documents\\Visual Studio 2013\\Projects\\EventQueryVista\\output\\"));
	CFilePath fpath;
	Ptr(CFile) f = OK_NEW_OPERATOR CStreamFile;

	spath.AppendString(pInfo->get_name());
	spath.AppendString(_T(".txt"));
	spath.ReplaceString(_T("/"), _T("_"));
	fpath.set_Path(spath);
	f->Create(fpath, true, CFile::UTF_8_Encoding);

	f->Write(_T("** Provider Name: %s **********\n"), pInfo->get_name().GetString());
	f->Write(_T("Provider Guid: %s\n"), pInfo->get_guid().GetString());
	f->Write(_T("Help link: %s\n"), pInfo->get_helpLink().GetString());
	f->Write(_T("MessageFile: %s\n"), pInfo->get_messageFile().GetString());
	f->Write(_T("ParameterFile: %s\n"), pInfo->get_parameterFile().GetString());
	f->Write(_T("ResourceFile: %s\n"), pInfo->get_resourceFile().GetString());

	CDataDoubleLinkedListT<CEventLogProviderChannel> channelList = pInfo->get_channelList();
	CDataDoubleLinkedListT<CEventLogProviderChannel>::Iterator channelListIt = channelList.Begin();

	for (TListCnt i = 0; i < channelList.Count(); ++i)
	{
		f->Write(_T("Channel%d: Id=%d, Path=%s, Index=%d, MessageID=%d, Message=%s, Imported=%s\n"), i, (*channelListIt)->get_id(),
			(*channelListIt)->get_path().GetString(), (*channelListIt)->get_index(), (*channelListIt)->get_messageID(), (*channelListIt)->get_message().GetString(), (*channelListIt)->get_imported() ? _T("True") : _T("False"));
		++channelListIt;
	}

	CDataDoubleLinkedListT<CEventLogProviderLevel> levelList = pInfo->get_levelList();
	CDataDoubleLinkedListT<CEventLogProviderLevel>::Iterator levelListIt = levelList.Begin();

	for (TListCnt i = 0; i < levelList.Count(); ++i)
	{
		f->Write(_T("Level%d: Value=0x%08x, Name=%s, MessageID=%d, Message=%s\n"), i, (*levelListIt)->get_value(),
			(*levelListIt)->get_name().GetString(), (*levelListIt)->get_messageID(), (*levelListIt)->get_message().GetString());
		++levelListIt;
	}

	CDataDoubleLinkedListT<CEventLogProviderTask> taskList = pInfo->get_taskList();
	CDataDoubleLinkedListT<CEventLogProviderTask>::Iterator taskListIt = taskList.Begin();

	for (TListCnt i = 0; i < taskList.Count(); ++i)
	{
		f->Write(_T("Task%d: Value=0x%08x, Name=%s, EventGuid=%s, MessageID=%d, Message=%s\n"), i, (*taskListIt)->get_value(),
			(*taskListIt)->get_name().GetString(), (*taskListIt)->get_eventGuid().GetString(), (*taskListIt)->get_messageID(), (*taskListIt)->get_message().GetString());
		++taskListIt;
	}

	CDataDoubleLinkedListT<CEventLogProviderOpCode> opCodeList = pInfo->get_opCodeList();
	CDataDoubleLinkedListT<CEventLogProviderOpCode>::Iterator opCodeListIt = opCodeList.Begin();

	for (TListCnt i = 0; i < opCodeList.Count(); ++i)
	{
		f->Write(_T("OpCode%d: Value=0x%08x, Name=%s, MessageID=%d, Message=%s\n"), i, (*opCodeListIt)->get_value(),
			(*opCodeListIt)->get_name().GetString(), (*opCodeListIt)->get_messageID(), (*opCodeListIt)->get_message().GetString());
		++opCodeListIt;
	}

	CDataDoubleLinkedListT<CEventLogProviderKeyWord> keyWordsList = pInfo->get_keyWordList();
	CDataDoubleLinkedListT<CEventLogProviderKeyWord>::Iterator keyWordsListIt = keyWordsList.Begin();

	for (TListCnt i = 0; i < keyWordsList.Count(); ++i)
	{
		f->Write(_T("KeyWords%d: Value=0x%016llx, Name=%s, MessageID=%d, Message=%s\n"), i, (*keyWordsListIt)->get_value(),
			(*keyWordsListIt)->get_name().GetString(), (*keyWordsListIt)->get_messageID(), (*keyWordsListIt)->get_message().GetString());
		++keyWordsListIt;
	}

	CDataDoubleLinkedListT<CEventLogProviderEvent> eventList = pInfo->get_eventList();
	CDataDoubleLinkedListT<CEventLogProviderEvent>::Iterator eventListIt = eventList.Begin();

	for (TListCnt i = 0; i < eventList.Count(); ++i)
	{
		f->Write(_T("Event%d: Id=(%d, 0x%08x), Version=%d, Channel=%d, Level=0x%08x, OpCode=0x%08x, Task=0x%08x, KeyWord=0x%016llx\n"), i, (*eventListIt)->get_id(), (*eventListIt)->get_id(),
			(*eventListIt)->get_version(), (*eventListIt)->get_channelValue(), (*eventListIt)->get_levelValue(), (*eventListIt)->get_opCodeValue(), (*eventListIt)->get_taskValue(), (*eventListIt)->get_keyWordValue());
		f->Write(_T("        MessageId=%08x, Message=%s\n"), (*eventListIt)->get_messageID(), (*eventListIt)->get_message().GetString());
		f->Write(_T("        Template=%s\n"), (*eventListIt)->get_template().GetString());
		++eventListIt;
	}
	f->Close();

	return true;
}

DWORD g_totalevents = 0;
DWORD g_localenotfound = 0;
DWORD g_messagefileempty = 0;

static void TestEventLogProviderChannel(void)
{
	CEventLogProviders providers __FILE__LINE__0P;

	try
	{
		providers.Load();
		providers.ForEach<CEventLogProviderForEachFunctor>();
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
	}

	CEventLogChannels channels __FILE__LINE__0P;
	CEventLogChannelForEachFunctor arg;

	try
	{
		channels.Load();
		arg.providers = &providers;
		channels.ForEach<CEventLogChannelForEachFunctor>(arg);
	}
	catch (CBaseException* ex)
	{
		CERR << ex->GetExceptionMessage() << endl;
	}
}

void TestEventLog()
{
	TestEventLogRecord();
	TestEventLogProviderChannel();
}
