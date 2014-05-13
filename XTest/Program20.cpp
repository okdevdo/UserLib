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
#include "DataHashtable.h"
#include "Application.h"
#include "Configuration.h"
#include "SAXParser.h"
#include <Rpc.h>

static CStringBuffer create_uuid()
{
	CStringBuffer ret;
	UUID uuid;
#ifdef OK_COMP_MSC
#ifdef _UNICODE
	RPC_WSTR str;
#else
	RPC_STR str;
#endif
#endif
#ifdef OK_COMP_GNUC
#ifdef _UNICODE
	unsigned short* str;
#else
	unsigned char* str;
#endif
#endif

	UuidCreateSequential(&uuid);
	UuidToString(&uuid, &str);
	ret.SetString(__FILE__LINE__ CastAny(CPointer, str));
	RpcStringFree(&str);
	return ret;
}

static void AddFiles(ConstRef(CFilePath) folder, Ptr(CFile) ffile, ConstRef(CStringBuffer) fsubdir = CStringBuffer::null())
{
	CFilePath fname(folder);
	CDirectoryIterator it;
	CStringBuffer tmp;

	if (fsubdir.IsEmpty())
		ffile->Write(_T("\t\t<ProjectFiles>\n"));
	fname.MakeDirectory();
	fname.set_Filename(_T("*"));
	it.Open(fname);
	while (it)
	{
		if (it.is_SubDir())
		{
			fname = folder;
			fname.MakeDirectory();
			fname.set_Filename(it.get_Name());
			tmp = fsubdir;
			tmp.AppendString(it.get_Name());
			tmp.AppendString(CDirectoryIterator::DefaultPathSeparatorString());
			AddFiles(fname, ffile, tmp);
		}
		else if (s_strxcmp(it.get_Name(), _T("*.h")))
		{
			tmp.FormatString(__FILE__LINE__ _T("\t\t\t<ProjectFile Name=\"%s%s\" type=\"Header\" />\n"), fsubdir.IsEmpty() ? _T("") : fsubdir.GetString(), it.get_Name().GetString());
			ffile->Write(tmp);
		}
		else if (s_strxcmp(it.get_Name(), _T("*.c")))
		{
			tmp.FormatString(__FILE__LINE__ _T("\t\t\t<ProjectFile Name=\"%s%s\" type=\"CSource\" />\n"), fsubdir.IsEmpty() ? _T("") : fsubdir.GetString(), it.get_Name().GetString());
			ffile->Write(tmp);
		}
		else if (s_strxcmp(it.get_Name(), _T("*.cpp")))
		{
			tmp.FormatString(__FILE__LINE__ _T("\t\t\t<ProjectFile Name=\"%s%s\" type=\"CppSource\" />\n"), fsubdir.IsEmpty() ? _T("") : fsubdir.GetString(), it.get_Name().GetString());
			ffile->Write(tmp);
		}
		else if (s_strxcmp(it.get_Name(), _T("*.rc")))
		{
			tmp.FormatString(__FILE__LINE__ _T("\t\t\t<ProjectFile Name=\"%s%s\" type=\"ResourceCompiler\" />\n"), fsubdir.IsEmpty() ? _T("") : fsubdir.GetString(), it.get_Name().GetString());
			ffile->Write(tmp);
		}
		else if (s_strxcmp(it.get_Name(), _T("*.bmp")) || s_strxcmp(it.get_Name(), _T("*.ico")))
		{
			tmp.FormatString(__FILE__LINE__ _T("\t\t\t<ProjectFile Name=\"%s%s\" type=\"Image\" />\n"), fsubdir.IsEmpty() ? _T("") : fsubdir.GetString(), it.get_Name().GetString());
			ffile->Write(tmp);
		}
		else if (s_strxcmp(it.get_Name(), _T("*.man")) || s_strxcmp(it.get_Name(), _T("*.mc")))
		{
			tmp.FormatString(__FILE__LINE__ _T("\t\t\t<ProjectFile Name=\"%s%s\" type=\"MessageCompiler\" />\n"), fsubdir.IsEmpty() ? _T("") : fsubdir.GetString(), it.get_Name().GetString());
			ffile->Write(tmp);
		}
		++it;
	}
	if (fsubdir.IsEmpty())
		ffile->Write(_T("\t\t</ProjectFiles>\n"));
}


class CMetaProjectDependency
{
public:
	CMetaProjectDependency() : m_name(), m_guid(), m_liste() {}
	CMetaProjectDependency(CConstPointer name, CConstPointer liste) : m_name(__FILE__LINE__ name), m_guid(), m_liste(__FILE__LINE__ liste) {}
	CMetaProjectDependency(ConstRef(CStringBuffer) name, ConstRef(CStringBuffer) liste) : m_name(name), m_guid(), m_liste(liste) {}
	~CMetaProjectDependency() {}

	ConstRef(CStringBuffer) key() const { return m_name; }

	CStringBuffer m_name;
	CStringBuffer m_guid;
	CStringBuffer m_liste;
};

typedef CHashLinkedListT<CMetaProjectDependency, CStringBuffer, HashFunctorString> CMetaProjectDependencies;

static void AddDependencies(ConstRef(CMetaProjectDependencies) gdeplist, ConstRef(CMetaProjectDependency) dep, Ptr(CFile) ffile)
{
	CDataVectorT<CStringBuffer> deplist(__FILE__LINE__ 16, 16);
	CDataVectorT<CStringBuffer>::Iterator it;
	CStringBuffer tmp;

	ffile->Write(_T("\t\t<ProjectDependencies>\n"));
	deplist.Split(dep.m_liste, _T(", "));
	it = deplist.Begin();
	while (it)
	{
		tmp = *it;
		tmp.Trim();

		CMetaProjectDependency gdep(gdeplist.search(tmp));

		tmp.FormatString(__FILE__LINE__ _T("\t\t\t<ProjectDependency Name=\"%s\" guid=\"{%s}\" />\n"), gdep.m_name.GetString(), gdep.m_guid.GetString());
		ffile->Write(tmp);
		++it;
	}
	ffile->Write(_T("\t\t</ProjectDependencies>\n"));
}

static void InitDependencies(Ref(CMetaProjectDependencies) ProjDep, ConstRef(CAbstractConfiguration::Values) v)
{
	CAbstractConfiguration::Values::Iterator vIt = v.Begin();
	CStringBuffer tmp;

	while (vIt)
	{
		tmp.SetString(__FILE__LINE__ _T("MetaProject.Dependencies."));
		tmp.AppendString(*vIt);
		tmp = theApp->config()->GetUserValue(tmp, _T("XTest"));

		if (tmp.IsEmpty())
			ProjDep.insert(CMetaProjectDependency(*vIt, CStringBuffer(__FILE__LINE__ _T(""))));
		else
			ProjDep.insert(CMetaProjectDependency(*vIt, tmp));
		++vIt;
	}
}

class CMetaProjectInfo
{
public:
	CMetaProjectInfo() {}
	CMetaProjectInfo(CConstPointer name) : m_name(__FILE__LINE__ name) {}
	CMetaProjectInfo(ConstRef(CStringBuffer) name) : m_name(name) {}
	~CMetaProjectInfo() {}

	__inline ConstRef(CStringBuffer) key() const { return m_name; }

	CStringBuffer m_name;
	CStringBuffer m_Culture;
	CStringBuffer m_Command;
	CStringBuffer m_OutPuts;
	CStringBuffer m_AdditionalInputs;
	CStringBuffer m_PreprocessorDefinitions;
	CStringBuffer m_PrecompiledHeaderFile;
	CStringBuffer m_AdditionalIncludeDirectories;
	CStringBuffer m_AdditionalDependencies;

	__inline bool isAllEmpty() const
	{
		return m_Culture.IsEmpty() &&
			m_Command.IsEmpty() &&
			m_OutPuts.IsEmpty() &&
			m_AdditionalInputs.IsEmpty() &&
			m_PreprocessorDefinitions.IsEmpty() &&
			m_PrecompiledHeaderFile.IsEmpty() &&
			m_AdditionalIncludeDirectories.IsEmpty() &&
			m_AdditionalDependencies.IsEmpty();
	}

};

typedef CHashLinkedListT<CMetaProjectInfo, CStringBuffer, HashFunctorString> CMetaProjectInfos;

static void AddInfo(ConstRef(CMetaProjectInfo) info, Ptr(CFile) ffile)
{
	if (info.isAllEmpty())
		return;

	CDataVectorT<CStringBuffer>::Iterator it;
	CStringBuffer tmp;
	CStringBuffer tmp1;

	ffile->Write(_T("\t\t<ProjectInfos>\n"));
	if (!(info.m_Culture.IsEmpty()))
		ffile->Write(_T("\t\t\t<Culture>%s</Culture>\n"), info.m_Culture.GetString());
	if (!(info.m_Command.IsEmpty()))
		ffile->Write(_T("\t\t\t<Command>%s</Command>\n"), info.m_Command.GetString());
	if (!(info.m_OutPuts.IsEmpty()))
	{
		CDataVectorT<CStringBuffer> liste(__FILE__LINE__ 16, 16);

		ffile->Write(_T("\t\t\t<Outputs>\n"));
		liste.Split(info.m_OutPuts, _T(", "));
		it = liste.Begin();
		while (it)
		{
			tmp = *it;
			tmp.Trim();
			tmp1.FormatString(__FILE__LINE__ _T("\t\t\t\t<Output>%s</Output>\n"), tmp.GetString());
			ffile->Write(tmp1);
			++it;
		}
		ffile->Write(_T("\t\t\t</Outputs>\n"));
	}
	if (!(info.m_AdditionalInputs.IsEmpty()))
	{
		CDataVectorT<CStringBuffer> liste(__FILE__LINE__ 16, 16);

		ffile->Write(_T("\t\t\t<AdditionalInputs>\n"));
		liste.Split(info.m_AdditionalInputs, _T(", "));
		it = liste.Begin();
		while (it)
		{
			tmp = *it;
			tmp.Trim();
			tmp1.FormatString(__FILE__LINE__ _T("\t\t\t\t<AdditionalInput>%s</AdditionalInput>\n"), tmp.GetString());
			ffile->Write(tmp1);
			++it;
		}
		ffile->Write(_T("\t\t\t</AdditionalInputs>\n"));
	}
	if (!(info.m_PreprocessorDefinitions.IsEmpty()))
	{
		CDataVectorT<CStringBuffer> liste(__FILE__LINE__ 16, 16);

		ffile->Write(_T("\t\t\t<PreprocessorDefinitions>\n"));
		liste.Split(info.m_PreprocessorDefinitions, _T(", "));
		it = liste.Begin();
		while (it)
		{
			tmp = *it;
			tmp.Trim();
			tmp1.FormatString(__FILE__LINE__ _T("\t\t\t\t<PreprocessorDefinition>%s</PreprocessorDefinition>\n"), tmp.GetString());
			ffile->Write(tmp1);
			++it;
		}
		ffile->Write(_T("\t\t\t</PreprocessorDefinitions>\n"));
	}
	if (!(info.m_PrecompiledHeaderFile.IsEmpty()))
		ffile->Write(_T("\t\t\t<PrecompiledHeaderFile>%s</PrecompiledHeaderFile>\n"), info.m_PrecompiledHeaderFile.GetString());
	if (!(info.m_AdditionalIncludeDirectories.IsEmpty()))
	{
		CDataVectorT<CStringBuffer> liste(__FILE__LINE__ 16, 16);

		ffile->Write(_T("\t\t\t<AdditionalIncludeDirectories>\n"));
		liste.Split(info.m_AdditionalIncludeDirectories, _T(", "));
		it = liste.Begin();
		while (it)
		{
			tmp = *it;
			tmp.Trim();
			tmp1.FormatString(__FILE__LINE__ _T("\t\t\t\t<AdditionalIncludeDirectory>%s</AdditionalIncludeDirectory>\n"), tmp.GetString());
			ffile->Write(tmp1);
			++it;
		}
		ffile->Write(_T("\t\t\t</AdditionalIncludeDirectories>\n"));
	}
	if (!(info.m_AdditionalDependencies.IsEmpty()))
	{
		CDataVectorT<CStringBuffer> liste(__FILE__LINE__ 16, 16);

		ffile->Write(_T("\t\t\t<AdditionalDependencies>\n"));
		liste.Split(info.m_AdditionalDependencies, _T(", "));
		it = liste.Begin();
		while (it)
		{
			tmp = *it;
			tmp.Trim();
			tmp1.FormatString(__FILE__LINE__ _T("\t\t\t\t<AdditionalDependency>%s</AdditionalDependency>\n"), tmp.GetString());
			ffile->Write(tmp1);
			++it;
		}
		ffile->Write(_T("\t\t\t</AdditionalDependencies>\n"));
	}
	ffile->Write(_T("\t\t</ProjectInfos>\n"));
}

static void InitInfos(Ref(CMetaProjectInfos) ProjInfo, ConstRef(CAbstractConfiguration::Values) v)
{
	CAbstractConfiguration::Values::Iterator vIt = v.Begin();
	CStringBuffer tmp;

	while (vIt)
	{
		CMetaProjectInfo info(*vIt);

		tmp.SetString(__FILE__LINE__ _T("MetaProject.Culture."));
		tmp.AppendString(*vIt);
		tmp = theApp->config()->GetUserValue(tmp, _T("XTest"));

		if (!(tmp.IsEmpty()))
			info.m_Culture = tmp;

		tmp.SetString(__FILE__LINE__ _T("MetaProject.Command."));
		tmp.AppendString(*vIt);
		tmp = theApp->config()->GetUserValue(tmp, _T("XTest"));

		if (!(tmp.IsEmpty()))
			info.m_Command = tmp;

		tmp.SetString(__FILE__LINE__ _T("MetaProject.Outputs."));
		tmp.AppendString(*vIt);
		tmp = theApp->config()->GetUserValue(tmp, _T("XTest"));

		if (!(tmp.IsEmpty()))
			info.m_OutPuts = tmp;

		tmp.SetString(__FILE__LINE__ _T("MetaProject.AdditionalInputs."));
		tmp.AppendString(*vIt);
		tmp = theApp->config()->GetUserValue(tmp, _T("XTest"));

		if (!(tmp.IsEmpty()))
			info.m_AdditionalInputs = tmp;

		tmp.SetString(__FILE__LINE__ _T("MetaProject.PreprocessorDefinitions."));
		tmp.AppendString(*vIt);
		tmp = theApp->config()->GetUserValue(tmp, _T("XTest"));

		if (!(tmp.IsEmpty()))
			info.m_PreprocessorDefinitions = tmp;

		tmp.SetString(__FILE__LINE__ _T("MetaProject.PrecompiledHeaderFile."));
		tmp.AppendString(*vIt);
		tmp = theApp->config()->GetUserValue(tmp, _T("XTest"));

		if (!(tmp.IsEmpty()))
			info.m_PrecompiledHeaderFile = tmp;

		tmp.SetString(__FILE__LINE__ _T("MetaProject.AdditionalIncludeDirectories."));
		tmp.AppendString(*vIt);
		tmp = theApp->config()->GetUserValue(tmp, _T("XTest"));

		if (!(tmp.IsEmpty()))
			info.m_AdditionalIncludeDirectories = tmp;

		tmp.SetString(__FILE__LINE__ _T("MetaProject.AdditionalDependencies."));
		tmp.AppendString(*vIt);
		tmp = theApp->config()->GetUserValue(tmp, _T("XTest"));

		if (!(tmp.IsEmpty()))
			info.m_AdditionalDependencies = tmp;

		ProjInfo.insert(info);
		++vIt;
	}
}

void MetaProjectCreateProjectFilesXML()
{
	CFilePath fname(__FILE__LINE__ _T("ProjectFiles.xml"));
	Ptr(CFile) ffile = OK_NEW_OPERATOR CStreamFile;
	CDirectoryIterator it;
	CStringBuffer tmp;
	CStringBuffer tmp1;
	CAbstractConfiguration::Values Libs(__FILE__LINE__ 16, 16);
	CAbstractConfiguration::Values NoEntryLibs(__FILE__LINE__ 16, 16);
	CAbstractConfiguration::Values GuiExe(__FILE__LINE__ 16, 16);
	CAbstractConfiguration::Values ConsoleExe(__FILE__LINE__ 16, 16);
	CAbstractConfiguration::Values::Iterator vIt;
	CMetaProjectDependencies ProjDep(200);
	CMetaProjectInfos ProjInfo(200);

	if (!(theApp->config()->HasUserValue(_T("MetaProject.Libraries"), _T("XTest"))))
	{
		CERR << _T("Configuration 'MetaProject.Libraries' not found.") << endl;
		return;
	}
	theApp->config()->GetUserValues(_T("MetaProject.Libraries"), _T("XTest"), Libs);
	Libs.Sort();
	InitDependencies(ProjDep, Libs);
	InitInfos(ProjInfo, Libs);
	if (!(theApp->config()->HasUserValue(_T("MetaProject.NoEntryLibraries"), _T("XTest"))))
	{
		CERR << _T("Configuration 'MetaProject.NoEntryLibraries' not found.") << endl;
		return;
	}
	theApp->config()->GetUserValues(_T("MetaProject.NoEntryLibraries"), _T("XTest"), NoEntryLibs);
	NoEntryLibs.Sort();
	InitDependencies(ProjDep, NoEntryLibs);
	InitInfos(ProjInfo, NoEntryLibs);
	if (!(theApp->config()->HasUserValue(_T("MetaProject.GuiExecutable"), _T("XTest"))))
	{
		CERR << _T("Configuration 'MetaProject.GuiExecutable' not found.") << endl;
		return;
	}
	theApp->config()->GetUserValues(_T("MetaProject.GuiExecutable"), _T("XTest"), GuiExe);
	GuiExe.Sort();
	InitDependencies(ProjDep, GuiExe);
	InitInfos(ProjInfo, GuiExe);
	if (!(theApp->config()->HasUserValue(_T("MetaProject.ConsoleExecutable"), _T("XTest"))))
	{
		CERR << _T("Configuration 'MetaProject.ConsoleExecutable' not found.") << endl;
		return;
	}
	theApp->config()->GetUserValues(_T("MetaProject.ConsoleExecutable"), _T("XTest"), ConsoleExe);
	ConsoleExe.Sort();
	InitDependencies(ProjDep, ConsoleExe);
	InitInfos(ProjInfo, ConsoleExe);

	ffile->Create(fname, false, CFile::UTF_8_Encoding);
	ffile->Write(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
	tmp.FormatString(__FILE__LINE__ _T("<Projects Guid=\"{%s}\">\n"), create_uuid().GetString());
	ffile->Write(tmp);
	it.Open(CFilePath(__FILE__LINE__ _T("*")));
	while (it)
	{
		if (it.is_SubDir())
		{
			tmp = it.get_Name();
			vIt = Libs.FindSorted(tmp);
			if (vIt && ((*vIt) == tmp))
			{
				CMetaProjectDependency dep(ProjDep.search(tmp));

				assert(!(dep.m_name.IsEmpty()));
				dep.m_guid = create_uuid();
				ProjDep.remove(tmp);
				ProjDep.insert(dep);
				++it;
				continue;
			}
			vIt = NoEntryLibs.FindSorted(tmp);
			if (vIt && ((*vIt) == tmp))
			{
				CMetaProjectDependency dep(ProjDep.search(tmp));

				assert(!(dep.m_name.IsEmpty()));
				dep.m_guid = create_uuid();
				ProjDep.remove(tmp);
				ProjDep.insert(dep);
				++it;
				continue;
			}
			vIt = GuiExe.FindSorted(tmp);
			if (vIt && ((*vIt) == tmp))
			{
				CMetaProjectDependency dep(ProjDep.search(tmp));

				assert(!(dep.m_name.IsEmpty()));
				dep.m_guid = create_uuid();
				ProjDep.remove(tmp);
				ProjDep.insert(dep);
				++it;
				continue;
			}
			vIt = ConsoleExe.FindSorted(tmp);
			if (vIt && ((*vIt) == tmp))
			{
				CMetaProjectDependency dep(ProjDep.search(tmp));

				assert(!(dep.m_name.IsEmpty()));
				dep.m_guid = create_uuid();
				ProjDep.remove(tmp);
				ProjDep.insert(dep);
				++it;
				continue;
			}
		}
		++it;
	}
	it.Open(CFilePath(__FILE__LINE__ _T("*")));
	while (it)
	{
		if (it.is_SubDir())
		{
			tmp = it.get_Name();
			vIt = Libs.FindSorted(tmp);
			if (vIt && ((*vIt) == tmp))
			{
				CMetaProjectDependency dep(ProjDep.search(tmp));
				CMetaProjectInfo info(ProjInfo.search(tmp));

				tmp1.FormatString(__FILE__LINE__ _T("\t<Project Name=\"%s\" Guid=\"{%s}\" Folder=\"%s\" type=\"Library\">\n"), dep.m_name.GetString(), dep.m_guid.GetString(), dep.m_name.GetString());
				ffile->Write(tmp1);
				AddInfo(info, ffile);
				AddFiles(tmp, ffile);
				AddDependencies(ProjDep, dep, ffile);
				ffile->Write(_T("\t</Project>\n"));
				++it;
				continue;
			}
			vIt = NoEntryLibs.FindSorted(tmp);
			if (vIt && ((*vIt) == tmp))
			{
				CMetaProjectDependency dep(ProjDep.search(tmp));
				CMetaProjectInfo info(ProjInfo.search(tmp));

				tmp1.FormatString(__FILE__LINE__ _T("\t<Project Name=\"%s\" Guid=\"{%s}\" Folder=\"%s\" type=\"NoEntryLibrary\">\n"), dep.m_name.GetString(), dep.m_guid.GetString(), dep.m_name.GetString());
				ffile->Write(tmp1);
				AddInfo(info, ffile);
				AddFiles(tmp, ffile);
				AddDependencies(ProjDep, dep, ffile);
				ffile->Write(_T("\t</Project>\n"));
				++it;
				continue;
			}
			vIt = GuiExe.FindSorted(tmp);
			if (vIt && ((*vIt) == tmp))
			{
				CMetaProjectDependency dep(ProjDep.search(tmp));
				CMetaProjectInfo info(ProjInfo.search(tmp));

				tmp1.FormatString(__FILE__LINE__ _T("\t<Project Name=\"%s\" Guid=\"{%s}\" Folder=\"%s\" type=\"GuiExe\">\n"), dep.m_name.GetString(), dep.m_guid.GetString(), dep.m_name.GetString());
				ffile->Write(tmp1);
				AddInfo(info, ffile);
				AddFiles(tmp, ffile);
				AddDependencies(ProjDep, dep, ffile);
				ffile->Write(_T("\t</Project>\n"));
				++it;
				continue;
			}
			vIt = ConsoleExe.FindSorted(tmp);
			if (vIt && ((*vIt) == tmp))
			{
				CMetaProjectDependency dep(ProjDep.search(tmp));
				CMetaProjectInfo info(ProjInfo.search(tmp));

				tmp1.FormatString(__FILE__LINE__ _T("\t<Project Name=\"%s\" Guid=\"{%s}\" Folder=\"%s\" type=\"ConsoleExe\">\n"), dep.m_name.GetString(), dep.m_guid.GetString(), dep.m_name.GetString());
				ffile->Write(tmp1);
				AddInfo(info, ffile);
				AddFiles(tmp, ffile);
				AddDependencies(ProjDep, dep, ffile);
				ffile->Write(_T("\t</Project>\n"));
				++it;
				continue;
			}
		}
		++it;
	}
	ffile->Write(_T("</Projects>"));
	ffile->Close();
	ffile->release();
}



class CProjectDepInfo : public CCppObject
{
public:
	CProjectDepInfo() {}
	virtual ~CProjectDepInfo() {}

	CStringBuffer m_Name;
	CStringBuffer m_Guid;
};

sword __stdcall CProjectDepInfoSortAndSearchFunc(ConstPointer pa, ConstPointer pb)
{
	CProjectDepInfo* ppa = CastAnyPtr(CProjectDepInfo, CastMutable(Pointer, pa));
	CProjectDepInfo* ppb = CastAnyPtr(CProjectDepInfo, CastMutable(Pointer, pb));

	return ppa->m_Name.Compare(ppb->m_Name, 0, CStringLiteral::cIgnoreCase);
}

void __stdcall CProjectDepInfoDeleteFunc(ConstPointer data, Pointer context)
{
	CProjectDepInfo* pdata = CastAnyPtr(CProjectDepInfo, CastMutable(Pointer, data));

	pdata->release();
}

typedef CDataVectorT<CProjectDepInfo> CProjectDepInfos;

class CProjectFileInfo : public CCppObject
{
public:
	CProjectFileInfo() {}
	virtual ~CProjectFileInfo() {}

	CStringBuffer m_Name;
	CStringBuffer m_Type;
};

sword __stdcall CProjectFileInfoSortAndSearchFunc(ConstPointer pa, ConstPointer pb)
{
	CProjectFileInfo* ppa = CastAnyPtr(CProjectFileInfo, CastMutable(Pointer, pa));
	CProjectFileInfo* ppb = CastAnyPtr(CProjectFileInfo, CastMutable(Pointer, pb));

	return ppa->m_Name.Compare(ppb->m_Name, 0, CStringLiteral::cIgnoreCase);
}

void __stdcall CProjectFileInfoDeleteFunc(ConstPointer data, Pointer context)
{
	CProjectFileInfo* pdata = CastAnyPtr(CProjectFileInfo, CastMutable(Pointer, data));

	pdata->release();
}

typedef CDataVectorT<CProjectFileInfo> CProjectFileInfos;

typedef CDataVectorT<CStringBuffer> CProjectPreprocessorDefinitions;

typedef CDataVectorT<CStringBuffer> CProjectOutputs;

typedef CDataVectorT<CStringBuffer> CProjectAdditionalInputs;

class CProjectAdditionalIncludeDirectories : public CDataVectorT<CStringBuffer>
{
public:
	typedef CDataVectorT<CStringBuffer> super;

	CProjectAdditionalIncludeDirectories(DECL_FILE_LINE TListCnt ini, TListCnt exp) : super(ARGS_FILE_LINE ini, exp) {}

	Iterator AppendUnique(ConstRef(CStringBuffer) data) const
	{
		Iterator it = super::Find(data);

		if (!it)
		{
			it = VectorAppend(_liste, data.GetString());
			data.addRef();
		}
		return it;
	}
};

class CProjectAdditionalDependencies : public CDataVectorT<CStringBuffer>
{
public:
	typedef CDataVectorT<CStringBuffer> super;

	CProjectAdditionalDependencies(DECL_FILE_LINE TListCnt ini, TListCnt exp) : super(ARGS_FILE_LINE ini, exp) {}

	Iterator AppendUnique(ConstRef(CStringBuffer) data) const
	{
		Iterator it = super::Find(data);

		if (!it)
		{
			it = VectorAppend(_liste, data.GetString());
			data.addRef();
		}
		return it;
	}
};

class CProjectInfo : public CCppObject
{
public:
	CProjectInfo() :
		m_FileInfos(__FILE__LINE__ 16, 16, CProjectFileInfoDeleteFunc, NULL, CProjectFileInfoSortAndSearchFunc),
		m_DepInfos(__FILE__LINE__ 16, 16, CProjectDepInfoDeleteFunc, NULL, CProjectDepInfoSortAndSearchFunc),
		m_Outputs(__FILE__LINE__ 16, 16),
		m_AdditionalInputs(__FILE__LINE__ 16, 16),
		m_PreprocessorDefinitions(__FILE__LINE__ 16, 16),
		m_AdditionalIncludeDirectories(__FILE__LINE__ 16, 16),
		m_AdditionalDependencies(__FILE__LINE__ 16, 16)
	{
	}
	CProjectInfo(ConstRef(CStringBuffer) name) :
		m_Name(name),
		m_FileInfos(__FILE__LINE__ 16, 16, CProjectFileInfoDeleteFunc, NULL, CProjectFileInfoSortAndSearchFunc),
		m_DepInfos(__FILE__LINE__ 16, 16, CProjectDepInfoDeleteFunc, NULL, CProjectDepInfoSortAndSearchFunc),
		m_Outputs(__FILE__LINE__ 16, 16),
		m_AdditionalInputs(__FILE__LINE__ 16, 16),
		m_PreprocessorDefinitions(__FILE__LINE__ 16, 16),
		m_AdditionalIncludeDirectories(__FILE__LINE__ 16, 16),
		m_AdditionalDependencies(__FILE__LINE__ 16, 16)
	{
	}
	virtual ~CProjectInfo() {}

	CStringBuffer m_Name;
	CStringBuffer m_Guid;
	CStringBuffer m_Folder;
	CStringBuffer m_Type;
	CProjectFileInfos m_FileInfos;
	CProjectDepInfos m_DepInfos;
	CStringBuffer m_Culture;
	CStringBuffer m_Command;
	CProjectOutputs m_Outputs;
	CProjectAdditionalInputs m_AdditionalInputs;
	CProjectPreprocessorDefinitions m_PreprocessorDefinitions;
	CStringBuffer m_PrecompiledHeaderFile;
	CProjectAdditionalIncludeDirectories m_AdditionalIncludeDirectories;
	CProjectAdditionalDependencies m_AdditionalDependencies;
};

sword __stdcall CProjectInfoSortAndSearchFunc(ConstPointer pa, ConstPointer pb)
{
	CProjectInfo* ppa = CastAnyPtr(CProjectInfo, CastMutable(Pointer, pa));
	CProjectInfo* ppb = CastAnyPtr(CProjectInfo, CastMutable(Pointer, pb));

	return ppa->m_Name.Compare(ppb->m_Name, 0, CStringLiteral::cIgnoreCase);
}

void __stdcall CProjectInfoDeleteFunc(ConstPointer data, Pointer context)
{
	CProjectInfo* pdata = CastAnyPtr(CProjectInfo, CastMutable(Pointer, data));

	pdata->release();
}

class CProjectInfos : public CDataVectorT<CProjectInfo>
{
	typedef CDataVectorT<CProjectInfo> super;

public:
	CProjectInfos(DECL_FILE_LINE TListCnt init, TListCnt exp) : super(ARGS_FILE_LINE init, exp, CProjectInfoDeleteFunc, NULL, CProjectInfoSortAndSearchFunc) {}

	void CollectDeps(ConstRef(CStringBuffer) name, Ref(super::Iterator) it2Collect)
	{
		CStringBuffer tmp;
		CProjectInfo vdata(name);
		super::Iterator it = FindSorted(&vdata);

		if (it && (*it) && (CProjectInfoSortAndSearchFunc(*it, &vdata) == 0))
		{
			tmp = (*it)->m_Folder;
			tmp.PrependString(_T("..\\"));
			tmp.AppendString(_T("\\"));
			(*it2Collect)->m_AdditionalIncludeDirectories.AppendUnique(tmp);
			if ((*it)->m_Type.Compare(CStringLiteral(_T("NoEntryLibrary")), 0, CStringLiteral::cIgnoreCase) != 0)
			{
				tmp = (*it)->m_Name;
				tmp.AppendString(_T(".lib"));
				(*it2Collect)->m_AdditionalDependencies.AppendUnique(tmp);
			}

			CProjectDepInfos::Iterator it1 = (*it)->m_DepInfos.Begin();

			while (it1)
			{
				CollectDeps((*it1)->m_Name, it2Collect);
				++it1;
			}
		}
	}

	void Finish()
	{
		CConstPointer pDir = _T("..\\..\\");
		super::Iterator it;
		CStringBuffer tmp;

		Sort();
		it = Begin();
		while (it)
		{
			CProjectDepInfos::Iterator it1 = (*it)->m_DepInfos.Begin();

			while (it1)
			{
				CollectDeps((*it1)->m_Name, it);
				++it1;
			}
			++it;
		}
		it = Begin();
		while (it)
		{
			CProjectAdditionalIncludeDirectories::Iterator it0 = (*it)->m_AdditionalIncludeDirectories.Begin();

			while (it0)
			{
				tmp = *it0;

				tmp.PrependString(pDir);
				(*it)->m_AdditionalIncludeDirectories.SetData(it0, tmp);
				++it0;
			}
			++it;
		}
	}
};

class CXMLContentHandlers : public CSAXParserContentHandler
{
public:
	CXMLContentHandlers() : 
		CSAXParserContentHandler(), 
		m_ProjectInfos(__FILE__LINE__ 16, 16),
		m_CurrentProjectInfo(NULL),
		m_state(0)
	{
	}
	virtual ~CXMLContentHandlers() {}

	virtual void XmlDeclHandler(ConstRef(CStringBuffer) version, ConstRef(CStringBuffer) encoding, int standalone)
	{
	}

	virtual void ProcessingInstructionHandler(ConstRef(CStringBuffer) target, ConstRef(CStringBuffer) data)
	{
	}

	virtual void StartElementHandler(ConstRef(CStringBuffer) name, ConstRef(CSAXParserAttributes) attributes)
	{
		if (name.Compare(CStringLiteral(_T("Projects")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			CSAXParserAttributes::Iterator it = attributes.Begin();

			while (it)
			{
				if ((*it)->get_name().Compare(CStringLiteral(_T("Guid")), 0, CStringLiteral::cIgnoreCase) == 0)
					m_ProjectsGuid = (*it)->get_value();
				++it;
			}
			return;
		}
		if (name.Compare(CStringLiteral(_T("Project")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			if (m_CurrentProjectInfo)
				m_ProjectInfos.Append(m_CurrentProjectInfo);
			m_CurrentProjectInfo = OK_NEW_OPERATOR CProjectInfo;

			CSAXParserAttributes::Iterator it = attributes.Begin();

			while (it)
			{
				if ((*it)->get_name().Compare(CStringLiteral(_T("Name")), 0, CStringLiteral::cIgnoreCase) == 0)
					m_CurrentProjectInfo->m_Name = (*it)->get_value();
				if ((*it)->get_name().Compare(CStringLiteral(_T("Guid")), 0, CStringLiteral::cIgnoreCase) == 0)
					m_CurrentProjectInfo->m_Guid = (*it)->get_value();
				if ((*it)->get_name().Compare(CStringLiteral(_T("Folder")), 0, CStringLiteral::cIgnoreCase) == 0)
					m_CurrentProjectInfo->m_Folder = (*it)->get_value();
				if ((*it)->get_name().Compare(CStringLiteral(_T("Type")), 0, CStringLiteral::cIgnoreCase) == 0)
					m_CurrentProjectInfo->m_Type = (*it)->get_value();
				++it;
			}
			return;
		}
		if (name.Compare(CStringLiteral(_T("ProjectFile")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			CSAXParserAttributes::Iterator it = attributes.Begin();
			Ptr(CProjectFileInfo) pFileInfo = OK_NEW_OPERATOR CProjectFileInfo;

			while (it)
			{
				if ((*it)->get_name().Compare(CStringLiteral(_T("Name")), 0, CStringLiteral::cIgnoreCase) == 0)
					pFileInfo->m_Name = (*it)->get_value();
				if ((*it)->get_name().Compare(CStringLiteral(_T("Type")), 0, CStringLiteral::cIgnoreCase) == 0)
					pFileInfo->m_Type = (*it)->get_value();
				++it;
			}
			m_CurrentProjectInfo->m_FileInfos.Append(pFileInfo);
			return;
		}
		if (name.Compare(CStringLiteral(_T("ProjectDependency")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			CSAXParserAttributes::Iterator it = attributes.Begin();
			Ptr(CProjectDepInfo) pDepInfo = OK_NEW_OPERATOR CProjectDepInfo;

			while (it)
			{
				if ((*it)->get_name().Compare(CStringLiteral(_T("Name")), 0, CStringLiteral::cIgnoreCase) == 0)
					pDepInfo->m_Name = (*it)->get_value();
				if ((*it)->get_name().Compare(CStringLiteral(_T("Guid")), 0, CStringLiteral::cIgnoreCase) == 0)
					pDepInfo->m_Guid = (*it)->get_value();
				++it;
			}
			m_CurrentProjectInfo->m_DepInfos.Append(pDepInfo);
			return;
		}
		if (name.Compare(CStringLiteral(_T("PreprocessorDefinition")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			m_state = 1;
			return;
		}
		if (name.Compare(CStringLiteral(_T("PrecompiledHeaderFile")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			m_state = 2;
			return;
		}
		if (name.Compare(CStringLiteral(_T("AdditionalDependency")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			m_state = 3;
			return;
		}
		if (name.Compare(CStringLiteral(_T("AdditionalIncludeDirectory")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			m_state = 4;
			return;
		}
		if (name.Compare(CStringLiteral(_T("Command")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			m_state = 5;
			return;
		}
		if (name.Compare(CStringLiteral(_T("Output")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			m_state = 6;
			return;
		}
		if (name.Compare(CStringLiteral(_T("AdditionalInput")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			m_state = 7;
			return;
		}
		if (name.Compare(CStringLiteral(_T("Culture")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			m_state = 8;
			return;
		}
	}

	virtual void EndElementHandler(ConstRef(CStringBuffer) name)
	{
		if (name.Compare(CStringLiteral(_T("Project")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			if (m_CurrentProjectInfo)
			{
				m_ProjectInfos.Append(m_CurrentProjectInfo);
				m_CurrentProjectInfo = NULL;
			}
			return;
		}
		m_charData.Trim();
		if (name.Compare(CStringLiteral(_T("Culture")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			m_CurrentProjectInfo->m_Culture = m_charData;
			m_charData.Clear();
			m_state = 0;
			return;
		}
		if (name.Compare(CStringLiteral(_T("Command")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			m_CurrentProjectInfo->m_Command = m_charData;
			m_charData.Clear();
			m_state = 0;
			return;
		}
		if (name.Compare(CStringLiteral(_T("Output")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			m_CurrentProjectInfo->m_Outputs.Append(m_charData);
			m_charData.Clear();
			m_state = 0;
			return;
		}
		if (name.Compare(CStringLiteral(_T("AdditionalInput")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			m_CurrentProjectInfo->m_AdditionalInputs.Append(m_charData);
			m_charData.Clear();
			m_state = 0;
			return;
		}
		if (name.Compare(CStringLiteral(_T("PreprocessorDefinition")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			m_CurrentProjectInfo->m_PreprocessorDefinitions.Append(m_charData);
			m_charData.Clear();
			m_state = 0;
			return;
		}
		if (name.Compare(CStringLiteral(_T("PrecompiledHeaderFile")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			m_CurrentProjectInfo->m_PrecompiledHeaderFile = m_charData;
			m_charData.Clear();
			m_state = 0;
			return;
		}
		if (name.Compare(CStringLiteral(_T("AdditionalDependency")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			m_CurrentProjectInfo->m_AdditionalDependencies.Append(m_charData);
			m_charData.Clear();
			m_state = 0;
			return;
		}
		if (name.Compare(CStringLiteral(_T("AdditionalIncludeDirectory")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			m_CurrentProjectInfo->m_AdditionalIncludeDirectories.Append(m_charData);
			m_charData.Clear();
			m_state = 0;
			return;
		}
	}

	virtual void CharacterDataHandler(ConstRef(CStringBuffer) text)
	{
		switch (m_state)
		{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			m_charData.AppendString(text);
			break;
		default:
			break;
		}
	}

	virtual void CommentHandler(ConstRef(CStringBuffer) text)
	{
	}

	virtual void StartCdataSectionHandler()
	{
	}

	virtual void EndCdataSectionHandler()
	{
	}

	virtual void DefaultHandler(ConstRef(CStringBuffer) text)
	{
	}

	CStringBuffer m_ProjectsGuid;
	CProjectInfos m_ProjectInfos;
	Ptr(CProjectInfo) m_CurrentProjectInfo;
	word m_state;
	CStringBuffer m_charData;
};

static CConstPointer SLNStart =
_T("\r\nMicrosoft Visual Studio Solution File, Format Version 12.00\r\n")
_T("# Visual Studio Express 2013 for Windows Desktop\r\n")
_T("VisualStudioVersion = 12.0.30110.0\r\n")
_T("MinimumVisualStudioVersion = 10.0.40219.1\r\n");
static CConstPointer SLNGlobalStart = 
_T("Global\r\n")
_T("\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\r\n")
_T("\t\tDebug|Win32 = Debug|Win32\r\n")
_T("\t\tDebug|x64 = Debug|x64\r\n")
_T("\t\tRelease|Win32 = Release|Win32\r\n")
_T("\t\tRelease|x64 = Release|x64\r\n")
_T("\tEndGlobalSection\r\n")
_T("\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\r\n");
static CConstPointer SLNGlobalEnd =
_T("\tEndGlobalSection\r\n")
_T("\tGlobalSection(SolutionProperties) = preSolution\r\n")
_T("\t\tHideSolutionNode = FALSE\r\n")
_T("\tEndGlobalSection\r\n")
_T("EndGlobal\r\n");

static void WriteSolutionFile(ConstRef(CStringBuffer) guid, ConstRef(CProjectInfos) infos)
{
	CFilePath fname(__FILE__LINE__ _T("UserLib.sln"));
	Ptr(CFile) ffile = OK_NEW_OPERATOR CStreamFile;
	CProjectInfos::Iterator it;
	CProjectDepInfos::Iterator it1;

	ffile->Create(fname, false, CFile::UTF_8_Encoding);
	ffile->Write(SLNStart);
	it = infos.Begin();
	while (it)
	{
		// Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "CSources", "CSources\CSources.vcxproj", "{87327898-F5E1-474A-8DF8-3762A90BFBAC}"
		ffile->Write(_T("Project(\"%s\") = \"%s\", \"%s\\%s.vcxproj\", \"%s\"\r\n"), 
			guid.GetString(), (*it)->m_Name.GetString(), (*it)->m_Folder.GetString(), (*it)->m_Name.GetString(), (*it)->m_Guid.GetString());
		if ((*it)->m_DepInfos.Count() > 0)
		{
			ffile->Write(_T("\tProjectSection(ProjectDependencies) = postProject\r\n"));
			it1 = (*it)->m_DepInfos.Begin();
			while (it1)
			{
				ffile->Write(_T("\t\t%s = %s\r\n"), (*it1)->m_Guid.GetString(), (*it1)->m_Guid.GetString());
				++it1;
			}
			ffile->Write(_T("\tEndProjectSection\r\n"));
		}
		ffile->Write(_T("EndProject\r\n"));
		++it;
	}
	ffile->Write(SLNGlobalStart);
	it = infos.Begin();
	while (it)
	{
/*		{87327898-F5E1-474A-8DF8-3762A90BFBAC}.Debug|Win32.ActiveCfg = Debug|Win32
		{87327898-F5E1-474A-8DF8-3762A90BFBAC}.Debug|Win32.Build.0 = Debug|Win32
		{87327898-F5E1-474A-8DF8-3762A90BFBAC}.Debug|x64.ActiveCfg = Debug|x64
		{87327898-F5E1-474A-8DF8-3762A90BFBAC}.Debug|x64.Build.0 = Debug|x64
		{87327898-F5E1-474A-8DF8-3762A90BFBAC}.Release|Win32.ActiveCfg = Release|Win32
		{87327898-F5E1-474A-8DF8-3762A90BFBAC}.Release|Win32.Build.0 = Release|Win32
		{87327898-F5E1-474A-8DF8-3762A90BFBAC}.Release|x64.ActiveCfg = Release|x64
		{87327898-F5E1-474A-8DF8-3762A90BFBAC}.Release|x64.Build.0 = Release|x64
*/
		ffile->Write(_T("\t\t%s.Debug|Win32.ActiveCfg = Debug|Win32\r\n"), (*it)->m_Guid.GetString());
		ffile->Write(_T("\t\t%s.Debug|Win32.Build.0 = Debug|Win32\r\n"), (*it)->m_Guid.GetString());
		ffile->Write(_T("\t\t%s.Debug|x64.ActiveCfg = Debug|x64\r\n"), (*it)->m_Guid.GetString());
		ffile->Write(_T("\t\t%s.Debug|x64.Build.0 = Debug|x64\r\n"), (*it)->m_Guid.GetString());
		ffile->Write(_T("\t\t%s.Release|Win32.ActiveCfg = Release|Win32\r\n"), (*it)->m_Guid.GetString());
		ffile->Write(_T("\t\t%s.Release|Win32.Build.0 = Release|Win32\r\n"), (*it)->m_Guid.GetString());
		ffile->Write(_T("\t\t%s.Release|x64.ActiveCfg = Release|x64\r\n"), (*it)->m_Guid.GetString());
		ffile->Write(_T("\t\t%s.Release|x64.Build.0 = Release|x64\r\n"), (*it)->m_Guid.GetString());
		++it;
	}
	ffile->Write(SLNGlobalEnd);
	ffile->Close();
	ffile->release();
}

static CConstPointer VCXPROJStart =
	_T("<Project DefaultTargets=\"Build\" ToolsVersion=\"%s\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\r\n")
	_T("  <ItemGroup Label=\"ProjectConfigurations\">\r\n")
	_T("    <ProjectConfiguration Include=\"Debug|Win32\">\r\n")
	_T("      <Configuration>Debug</Configuration>\r\n")
	_T("      <Platform>Win32</Platform>\r\n")
	_T("    </ProjectConfiguration>\r\n")
	_T("    <ProjectConfiguration Include=\"Debug|x64\">\r\n")
	_T("      <Configuration>Debug</Configuration>\r\n")
	_T("      <Platform>x64</Platform>\r\n")
	_T("    </ProjectConfiguration>\r\n")
	_T("    <ProjectConfiguration Include=\"Release|Win32\">\r\n")
	_T("      <Configuration>Release</Configuration>\r\n")
	_T("      <Platform>Win32</Platform>\r\n")
	_T("    </ProjectConfiguration>\r\n")
	_T("    <ProjectConfiguration Include=\"Release|x64\">\r\n")
	_T("      <Configuration>Release</Configuration>\r\n")
	_T("      <Platform>x64</Platform>\r\n")
	_T("    </ProjectConfiguration>\r\n")
	_T("  </ItemGroup>\r\n")
	_T("  <PropertyGroup Label=\"Globals\">\r\n")
	_T("    <ProjectGuid>%s</ProjectGuid>\r\n")
	_T("    <RootNamespace>%s</RootNamespace>\r\n")
	_T("  </PropertyGroup>\r\n")
	_T("  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />\r\n");

static CConstPointer VCXPROJLOOP1[4] = {
	_T("  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\" Label=\"Configuration\">\r\n")
	_T("    <ConfigurationType>%s</ConfigurationType>\r\n")
	_T("    <UseDebugLibraries>true</UseDebugLibraries>\r\n")
	_T("    <CharacterSet>Unicode</CharacterSet>\r\n")
	_T("    <PlatformToolset>%s</PlatformToolset>\r\n")
	_T("  </PropertyGroup>\r\n"),
	_T("  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\" Label=\"Configuration\">\r\n")
	_T("    <ConfigurationType>%s</ConfigurationType>\r\n")
	_T("    <UseDebugLibraries>true</UseDebugLibraries>\r\n")
	_T("    <CharacterSet>Unicode</CharacterSet>\r\n")
	_T("    <PlatformToolset>%s</PlatformToolset>\r\n")
	_T("  </PropertyGroup>\r\n"),
	_T("  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\" Label=\"Configuration\">\r\n")
	_T("    <ConfigurationType>%s</ConfigurationType>\r\n")
	_T("    <UseDebugLibraries>false</UseDebugLibraries>\r\n")
	_T("    <WholeProgramOptimization>true</WholeProgramOptimization>\r\n")
	_T("    <CharacterSet>Unicode</CharacterSet>\r\n")
	_T("    <PlatformToolset>%s</PlatformToolset>\r\n")
	_T("  </PropertyGroup>\r\n"),
	_T("  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\" Label=\"Configuration\">\r\n")
	_T("    <ConfigurationType>%s</ConfigurationType>\r\n")
	_T("    <UseDebugLibraries>false</UseDebugLibraries>\r\n")
	_T("    <WholeProgramOptimization>true</WholeProgramOptimization>\r\n")
	_T("    <CharacterSet>Unicode</CharacterSet>\r\n")
	_T("    <PlatformToolset>%s</PlatformToolset>\r\n")
	_T("  </PropertyGroup>\r\n")
};
static CConstPointer VCXPROJPart2 =
	_T("  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\"/>\r\n")
	_T("  <ImportGroup Label=\"ExtensionSettings\">\r\n")
	_T("  </ImportGroup>\r\n")
	_T("  <ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">\r\n")
	_T("    <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\"/>\r\n")
	_T("  </ImportGroup>\r\n")
	_T("  <ImportGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\" Label=\"PropertySheets\">\r\n")
	_T("    <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\"/>\r\n")
	_T("  </ImportGroup>\r\n")
	_T("  <ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">\r\n")
	_T("    <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\"/>\r\n")
	_T("  </ImportGroup>\r\n")
	_T("  <ImportGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\" Label=\"PropertySheets\">\r\n")
	_T("    <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\"/>\r\n")
	_T("  </ImportGroup>\r\n")
	_T("  <PropertyGroup Label=\"UserMacros\"/>\r\n");

static CConstPointer VCXPROJPart3[] = {
	// 0
	_T("  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">\r\n")
	_T("    <ClCompile>\r\n")
	_T("      <WarningLevel>Level3</WarningLevel>\r\n")
	_T("      <Optimization>Disabled</Optimization>\r\n")
	_T("      <PrecompiledHeader>Create</PrecompiledHeader>\r\n"),
	// 1
	_T("  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">\r\n")
	_T("    <ClCompile>\r\n")
	_T("      <WarningLevel>Level3</WarningLevel>\r\n")
	_T("      <Optimization>Disabled</Optimization>\r\n"),
	// 2
	_T("  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">\r\n")
	_T("    <ClCompile>\r\n")
	_T("      <WarningLevel>Level3</WarningLevel>\r\n")
	_T("      <Optimization>MaxSpeed</Optimization>\r\n")
	_T("      <FunctionLevelLinking>true</FunctionLevelLinking>\r\n")
	_T("      <IntrinsicFunctions>true</IntrinsicFunctions>\r\n")
	_T("      <PrecompiledHeader>NotUsing</PrecompiledHeader>\r\n"),
	// 3
	_T("  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">\r\n")
	_T("    <ClCompile>\r\n")
	_T("      <WarningLevel>Level3</WarningLevel>\r\n")
	_T("      <Optimization>MaxSpeed</Optimization>\r\n")
	_T("      <FunctionLevelLinking>true</FunctionLevelLinking>\r\n")
	_T("      <IntrinsicFunctions>true</IntrinsicFunctions>\r\n")
	_T("      <PrecompiledHeader>NotUsing</PrecompiledHeader>\r\n"),
	// 4
	_T("      <AdditionalIncludeDirectories>%s</AdditionalIncludeDirectories>\r\n"),
	// 5
	_T("      <PrecompiledHeaderFile>%s</PrecompiledHeaderFile>\r\n"),
	// 6
	_T("      <PreprocessorDefinitions>_DEBUG; %s %%(PreprocessorDefinitions)</PreprocessorDefinitions>\r\n"),
	// 7
	_T("    </ClCompile>\r\n")
	_T("    <Link>\r\n")
	_T("      <GenerateDebugInformation>true</GenerateDebugInformation>\r\n"),
	// 8
	_T("    </ClCompile>\r\n")
	_T("    <Link>\r\n")
	_T("      <GenerateDebugInformation>true</GenerateDebugInformation>\r\n")
	_T("      <EnableCOMDATFolding>true</EnableCOMDATFolding>\r\n")
	_T("      <OptimizeReferences>true</OptimizeReferences>\r\n"),
	// 9
	_T("      <SubSystem>%s</SubSystem>\r\n"),
	// 10
	_T("      <AdditionalDependencies>%s %%(AdditionalDependencies)</AdditionalDependencies>\r\n"),
	// 11
	_T("      <AdditionalLibraryDirectories>..\\Debug</AdditionalLibraryDirectories>\r\n"),
	// 12
	_T("      <AdditionalLibraryDirectories>..\\Release</AdditionalLibraryDirectories>\r\n"),
	// 13
	_T("      <AdditionalLibraryDirectories>..\\x64\\Debug</AdditionalLibraryDirectories>\r\n"),
	// 14
	_T("      <AdditionalLibraryDirectories>..\\x64\\Release</AdditionalLibraryDirectories>\r\n"),
	// 15
	_T("    </Link>\r\n")
	_T("  </ItemDefinitionGroup>\r\n"),
	// 16
	_T("      <NoEntryPoint>true</NoEntryPoint>\r\n")
	_T("    </Link>\r\n")
	_T("  </ItemDefinitionGroup>\r\n"),
	// 17
	_T("      <PreprocessorDefinitions>NDEBUG; %s %%(PreprocessorDefinitions)</PreprocessorDefinitions>\r\n"),
	// 18
	_T("    </Link>\r\n")
	_T("     <ResourceCompile>\r\n")
	_T("       <Culture>%s</Culture>\r\n")
	_T("     </ResourceCompile>\r\n")
	_T("  </ItemDefinitionGroup>\r\n"),
	// 19
	_T("    <IgnoreSpecificDefaultLibraries>msvcrt.lib</IgnoreSpecificDefaultLibraries>\r\n"),
};

static CConstPointer VCXPROJPart4[] = {
	// 0
	_T("  <ItemGroup>\r\n"),
	// 1
	_T("    <ClInclude Include=\"%s\"/>\r\n"),
	// 2
	_T("  </ItemGroup>\r\n"),
	// 3
	_T("    <ClCompile Include=\"%s\"/>\r\n"),
	// 4
	_T("    <Image Include=\"%s\"/>\r\n"),
	// 5
	_T("    <ResourceCompile Include=\"%s\"/>\r\n"),
	// 6
	_T("    <CustomBuild Include=\"%s\">\r\n"),
	// 7
	_T("    </CustomBuild>\r\n"),
	// 8
	_T("      <FileType>Document</FileType>\r\n")
	_T("      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">false</ExcludedFromBuild>\r\n")
	_T("      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">false</ExcludedFromBuild>\r\n")
	_T("      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">false</ExcludedFromBuild>\r\n")
	_T("      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">false</ExcludedFromBuild>\r\n")
	_T("      <TreatOutputAsContent Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">true</TreatOutputAsContent>\r\n")
	_T("      <TreatOutputAsContent Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">true</TreatOutputAsContent>\r\n")
	_T("      <TreatOutputAsContent Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">true</TreatOutputAsContent>\r\n")
	_T("      <TreatOutputAsContent Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">true</TreatOutputAsContent>\r\n")
	_T("      <Command Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">%s</Command>\r\n")
	_T("      <Command Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">%s</Command>\r\n")
	_T("      <Command Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">%s</Command>\r\n")
	_T("      <Command Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">%s</Command>\r\n"),
	// 9
	_T("      <Outputs Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">%s</Outputs>\r\n")
	_T("      <Outputs Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">%s</Outputs>\r\n")
	_T("      <Outputs Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">%s</Outputs>\r\n")
	_T("      <Outputs Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">%s</Outputs>\r\n"),
	// 10
	_T("      <AdditionalInputs Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\">%s</AdditionalInputs>\r\n")
	_T("      <AdditionalInputs Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">%s</AdditionalInputs>\r\n")
	_T("      <AdditionalInputs Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">%s</AdditionalInputs>\r\n")
	_T("      <AdditionalInputs Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">%s</AdditionalInputs>\r\n"),

};

static CConstPointer VCXPROJEnde =
	_T("  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\"/>\r\n")
	_T("  <ImportGroup Label=\"ExtensionTargets\">\r\n")
	_T("  </ImportGroup>\r\n")
	_T("</Project>\r\n");

static CConstPointer VCXFILTStart =
	_T("<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\r\n");

static CConstPointer VCXFILTPart1[] = {
	// 0
	_T("  <ItemGroup>\r\n"),
	// 1
	_T("  </ItemGroup>\r\n"),
	// 2
	_T("    <ClCompile Include=\"%s\">\r\n")
	_T("      <Filter>Quelldateien</Filter>\r\n")
	_T("    </ClCompile>\r\n"),
	// 3
	_T("    <ClInclude Include=\"%s\">\r\n")
	_T("      <Filter>Headerdateien</Filter>\r\n")
	_T("    </ClInclude>\r\n"),
	// 4
	_T("    <ResourceCompile Include=\"%s\">\r\n")
	_T("      <Filter>Ressourcendateien</Filter>\r\n")
	_T("    </ResourceCompile>\r\n"),
	// 5
	_T("    <Image Include=\"%s\">\r\n")
	_T("      <Filter>Ressourcendateien</Filter>\r\n")
	_T("    </Image>\r\n"),
};

static CConstPointer VCXFILTEnde[] = {
	// 0
	_T("  <ItemGroup>\r\n"),
	// 1
	_T("    <Filter Include=\"Ressourcendateien\">\r\n")
	_T("      <UniqueIdentifier>{%s}</UniqueIdentifier>\r\n")
	_T("      <Extensions>rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx;tiff;tif;png;wav;mfcribbon-ms</Extensions>\r\n")
	_T("    </Filter>\r\n"),
	// 2
	_T("    <Filter Include=\"Headerdateien\">\r\n")
	_T("      <UniqueIdentifier>{%s}</UniqueIdentifier>\r\n")
	_T("      <Extensions>h;hh;hpp;hxx;hm;inl;inc;xsd</Extensions>\r\n")
	_T("    </Filter>\r\n"),
	// 3
	_T("    <Filter Include=\"Quelldateien\">\r\n")
	_T("      <UniqueIdentifier>{%s}</UniqueIdentifier>\r\n")
	_T("      <Extensions>cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx</Extensions>\r\n")
	_T("    </Filter>\r\n"),
	// 4
	_T("  </ItemGroup>\r\n"),
	// 5
	_T("</Project>\r\n"),
};

static void WriteProjectFile(CConstPointer pVersion, Ptr(CProjectInfo) info)
{
	CFilePath fname;
	Ptr(CFile) fprojfile = OK_NEW_OPERATOR CStreamFile;
	Ptr(CFile) ffilterfile = OK_NEW_OPERATOR CStreamFile;
	CConstPointer vVersion = NULL;
	CConstPointer vConfigurationType = NULL;
	CStringBuffer tmp;

	fname.set_Path(info->m_Name);
	fname.set_Extension(_T(".vcxproj"));
	fprojfile->Create(fname, false, CFile::UTF_8_Encoding);
	fprojfile->Write(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"));
	fname.set_FullExtension(_T(".vcxproj.filters"));
	ffilterfile->Create(fname, false, CFile::UTF_8_Encoding);
	ffilterfile->Write(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"));
	ffilterfile->Write(VCXFILTStart);

	if (s_strcmp(pVersion, _T("v100")) == 0)
		vVersion = _T("4.0");
	else
		vVersion = _T("12.0");

	if (info->m_Type.Compare(CStringLiteral(_T("Library")), 0, CStringLiteral::cIgnoreCase) == 0)
		vConfigurationType = _T("StaticLibrary");
	else if (info->m_Type.Compare(CStringLiteral(_T("NoEntryLibrary")), 0, CStringLiteral::cIgnoreCase) == 0)
		vConfigurationType = _T("DynamicLibrary");
	else if (info->m_Type.Compare(CStringLiteral(_T("ConsoleExe")), 0, CStringLiteral::cIgnoreCase) == 0)
		vConfigurationType = _T("Application");
	else if (info->m_Type.Compare(CStringLiteral(_T("GuiExe")), 0, CStringLiteral::cIgnoreCase) == 0)
		vConfigurationType = _T("Application");
	fprojfile->Write(VCXPROJStart, vVersion, info->m_Guid.GetString(), info->m_Name.GetString());
	for (int i = 0; i < 4; ++i)
		fprojfile->Write(VCXPROJLOOP1[i], vConfigurationType, pVersion);
	fprojfile->Write(VCXPROJPart2);
	// Win32 Debug
	fprojfile->Write(VCXPROJPart3[0]);
	tmp = info->m_AdditionalIncludeDirectories.Join(_T(";"));
	if (!(tmp.IsEmpty()))
		fprojfile->Write(VCXPROJPart3[4], tmp.GetString());
	if (!(info->m_PrecompiledHeaderFile.IsEmpty()))
		fprojfile->Write(VCXPROJPart3[5], info->m_PrecompiledHeaderFile.GetString());
	tmp = info->m_PreprocessorDefinitions.Join(_T(";"));
	if (!(tmp.IsEmpty()))
		tmp.AppendString(_T(";"));
	fprojfile->Write(VCXPROJPart3[6], tmp.IsEmpty() ? _T("") : tmp.GetString());
	if (info->m_Type.Compare(CStringLiteral(_T("Library")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		fprojfile->Write(VCXPROJPart3[7]);
		tmp = info->m_AdditionalDependencies.Join(_T(";"));
		if (!(tmp.IsEmpty()))
		{
			tmp.AppendString(_T(";"));
			fprojfile->Write(VCXPROJPart3[10], tmp.GetString());
		}
		fprojfile->Write(VCXPROJPart3[11]);
		fprojfile->Write(VCXPROJPart3[19]);
		fprojfile->Write(VCXPROJPart3[15]);
	}
	else if (info->m_Type.Compare(CStringLiteral(_T("NoEntryLibrary")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		fprojfile->Write(VCXPROJPart3[7]);
		fprojfile->Write(VCXPROJPart3[16]);
	}
	else if (info->m_Type.Compare(CStringLiteral(_T("ConsoleExe")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		fprojfile->Write(VCXPROJPart3[7]);
		fprojfile->Write(VCXPROJPart3[9], _T("Console"));
		tmp = info->m_AdditionalDependencies.Join(_T(";"));
		if (!(tmp.IsEmpty()))
		{
			tmp.AppendString(_T(";"));
			fprojfile->Write(VCXPROJPart3[10], tmp.GetString());
		}
		fprojfile->Write(VCXPROJPart3[11]);
		fprojfile->Write(VCXPROJPart3[19]);
		fprojfile->Write(VCXPROJPart3[15]);
	}
	else if (info->m_Type.Compare(CStringLiteral(_T("GuiExe")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		fprojfile->Write(VCXPROJPart3[7]);
		tmp = info->m_AdditionalDependencies.Join(_T(";"));
		if (!(tmp.IsEmpty()))
		{
			tmp.AppendString(_T(";"));
			fprojfile->Write(VCXPROJPart3[10], tmp.GetString());
		}
		fprojfile->Write(VCXPROJPart3[11]);
		fprojfile->Write(VCXPROJPart3[19]);
		if (info->m_Culture.IsEmpty())
			fprojfile->Write(VCXPROJPart3[15]);
		else
			fprojfile->Write(VCXPROJPart3[18], info->m_Culture.GetString());
	}
	// Win32 Release
	fprojfile->Write(VCXPROJPart3[2]);
	tmp = info->m_AdditionalIncludeDirectories.Join(_T(";"));
	if (!(tmp.IsEmpty()))
		fprojfile->Write(VCXPROJPart3[4], tmp.GetString());
	if (!(info->m_PrecompiledHeaderFile.IsEmpty()))
		fprojfile->Write(VCXPROJPart3[5], info->m_PrecompiledHeaderFile.GetString());
	tmp = info->m_PreprocessorDefinitions.Join(_T(";"));
	if (!(tmp.IsEmpty()))
		tmp.AppendString(_T(";"));
	fprojfile->Write(VCXPROJPart3[17], tmp.IsEmpty() ? _T("") : tmp.GetString());
	if (info->m_Type.Compare(CStringLiteral(_T("Library")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		fprojfile->Write(VCXPROJPart3[7]);
		tmp = info->m_AdditionalDependencies.Join(_T(";"));
		if (!(tmp.IsEmpty()))
		{
			tmp.AppendString(_T(";"));
			fprojfile->Write(VCXPROJPart3[10], tmp.GetString());
		}
		fprojfile->Write(VCXPROJPart3[12]);
		fprojfile->Write(VCXPROJPart3[15]);
	}
	else if (info->m_Type.Compare(CStringLiteral(_T("NoEntryLibrary")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		fprojfile->Write(VCXPROJPart3[7]);
		fprojfile->Write(VCXPROJPart3[16]);
	}
	else if (info->m_Type.Compare(CStringLiteral(_T("ConsoleExe")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		fprojfile->Write(VCXPROJPart3[8]);
		fprojfile->Write(VCXPROJPart3[9], _T("Console"));
		tmp = info->m_AdditionalDependencies.Join(_T(";"));
		if (!(tmp.IsEmpty()))
		{
			tmp.AppendString(_T(";"));
			fprojfile->Write(VCXPROJPart3[10], tmp.GetString());
		}
		fprojfile->Write(VCXPROJPart3[12]);
		fprojfile->Write(VCXPROJPart3[15]);
	}
	else if (info->m_Type.Compare(CStringLiteral(_T("GuiExe")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		fprojfile->Write(VCXPROJPart3[8]);
		tmp = info->m_AdditionalDependencies.Join(_T(";"));
		if (!(tmp.IsEmpty()))
		{
			tmp.AppendString(_T(";"));
			fprojfile->Write(VCXPROJPart3[10], tmp.GetString());
		}
		fprojfile->Write(VCXPROJPart3[12]);
		if (info->m_Culture.IsEmpty())
			fprojfile->Write(VCXPROJPart3[15]);
		else
			fprojfile->Write(VCXPROJPart3[18], info->m_Culture.GetString());
	}
	// Win64 Debug
	fprojfile->Write(VCXPROJPart3[1]);
	tmp = info->m_AdditionalIncludeDirectories.Join(_T(";"));
	if (!(tmp.IsEmpty()))
		fprojfile->Write(VCXPROJPart3[4], tmp.GetString());
	if (!(info->m_PrecompiledHeaderFile.IsEmpty()))
		fprojfile->Write(VCXPROJPart3[5], info->m_PrecompiledHeaderFile.GetString());
	tmp = info->m_PreprocessorDefinitions.Join(_T(";"));
	if (!(tmp.IsEmpty()))
		tmp.AppendString(_T(";"));
	fprojfile->Write(VCXPROJPart3[6], tmp.IsEmpty() ? _T("") : tmp.GetString());
	if (info->m_Type.Compare(CStringLiteral(_T("Library")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		fprojfile->Write(VCXPROJPart3[7]);
		tmp = info->m_AdditionalDependencies.Join(_T(";"));
		if (!(tmp.IsEmpty()))
		{
			tmp.AppendString(_T(";"));
			fprojfile->Write(VCXPROJPart3[10], tmp.GetString());
		}
		fprojfile->Write(VCXPROJPart3[13]);
		fprojfile->Write(VCXPROJPart3[19]);
		fprojfile->Write(VCXPROJPart3[15]);
	}
	else if (info->m_Type.Compare(CStringLiteral(_T("NoEntryLibrary")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		fprojfile->Write(VCXPROJPart3[7]);
		fprojfile->Write(VCXPROJPart3[16]);
	}
	else if (info->m_Type.Compare(CStringLiteral(_T("ConsoleExe")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		fprojfile->Write(VCXPROJPart3[7]);
		fprojfile->Write(VCXPROJPart3[9], _T("Console"));
		tmp = info->m_AdditionalDependencies.Join(_T(";"));
		if (!(tmp.IsEmpty()))
		{
			tmp.AppendString(_T(";"));
			fprojfile->Write(VCXPROJPart3[10], tmp.GetString());
		}
		fprojfile->Write(VCXPROJPart3[13]);
		fprojfile->Write(VCXPROJPart3[19]);
		fprojfile->Write(VCXPROJPart3[15]);
	}
	else if (info->m_Type.Compare(CStringLiteral(_T("GuiExe")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		fprojfile->Write(VCXPROJPart3[7]);
		tmp = info->m_AdditionalDependencies.Join(_T(";"));
		if (!(tmp.IsEmpty()))
		{
			tmp.AppendString(_T(";"));
			fprojfile->Write(VCXPROJPart3[10], tmp.GetString());
		}
		fprojfile->Write(VCXPROJPart3[13]);
		fprojfile->Write(VCXPROJPart3[19]);
		if (info->m_Culture.IsEmpty())
			fprojfile->Write(VCXPROJPart3[15]);
		else
			fprojfile->Write(VCXPROJPart3[18], info->m_Culture.GetString());
	}
	// Win64 Release
	fprojfile->Write(VCXPROJPart3[3]);
	tmp = info->m_AdditionalIncludeDirectories.Join(_T(";"));
	if (!(tmp.IsEmpty()))
		fprojfile->Write(VCXPROJPart3[4], tmp.GetString());
	if (!(info->m_PrecompiledHeaderFile.IsEmpty()))
		fprojfile->Write(VCXPROJPart3[5], info->m_PrecompiledHeaderFile.GetString());
	tmp = info->m_PreprocessorDefinitions.Join(_T(";"));
	if (!(tmp.IsEmpty()))
		tmp.AppendString(_T(";"));
	fprojfile->Write(VCXPROJPart3[17], tmp.IsEmpty() ? _T("") : tmp.GetString());
	if (info->m_Type.Compare(CStringLiteral(_T("Library")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		fprojfile->Write(VCXPROJPart3[7]);
		tmp = info->m_AdditionalDependencies.Join(_T(";"));
		if (!(tmp.IsEmpty()))
		{
			tmp.AppendString(_T(";"));
			fprojfile->Write(VCXPROJPart3[10], tmp.GetString());
		}
		fprojfile->Write(VCXPROJPart3[14]);
		fprojfile->Write(VCXPROJPart3[15]);
	}
	else if (info->m_Type.Compare(CStringLiteral(_T("NoEntryLibrary")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		fprojfile->Write(VCXPROJPart3[7]);
		fprojfile->Write(VCXPROJPart3[16]);
	}
	else if (info->m_Type.Compare(CStringLiteral(_T("ConsoleExe")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		fprojfile->Write(VCXPROJPart3[8]);
		fprojfile->Write(VCXPROJPart3[9], _T("Console"));
		tmp = info->m_AdditionalDependencies.Join(_T(";"));
		if (!(tmp.IsEmpty()))
		{
			tmp.AppendString(_T(";"));
			fprojfile->Write(VCXPROJPart3[10], tmp.GetString());
		}
		fprojfile->Write(VCXPROJPart3[14]);
		fprojfile->Write(VCXPROJPart3[15]);
	}
	else if (info->m_Type.Compare(CStringLiteral(_T("GuiExe")), 0, CStringLiteral::cIgnoreCase) == 0)
	{
		fprojfile->Write(VCXPROJPart3[8]);
		tmp = info->m_AdditionalDependencies.Join(_T(";"));
		if (!(tmp.IsEmpty()))
		{
			tmp.AppendString(_T(";"));
			fprojfile->Write(VCXPROJPart3[10], tmp.GetString());
		}
		fprojfile->Write(VCXPROJPart3[14]);
		if (info->m_Culture.IsEmpty())
			fprojfile->Write(VCXPROJPart3[15]);
		else
			fprojfile->Write(VCXPROJPart3[18], info->m_Culture.GetString());
	}

	CProjectFileInfos::Iterator it = info->m_FileInfos.Begin();
	CConstPointer pParentDir = _T("..\\..\\..\\");
	bool hasContent = false;
	bool bFirst = true;
	bool bHasHeader = false;
	bool bHasSource = false;
	bool bHasResource = false;

	while (it)
	{
		if ((*it)->m_Type.Compare(CStringLiteral(_T("Header")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			if (bFirst)
			{
				hasContent = true;
				fprojfile->Write(VCXPROJPart4[0]);
				ffilterfile->Write(VCXFILTPart1[0]);
				bHasHeader = true;
				bFirst = false;
			}	
			tmp = (*it)->m_Name;
			tmp.PrependString(_T("\\"));
			tmp.PrependString(info->m_Folder);
			tmp.PrependString(pParentDir);
			fprojfile->Write(VCXPROJPart4[1], tmp.GetString());
			ffilterfile->Write(VCXFILTPart1[3], tmp.GetString());
		}
		++it;
	}
	if (hasContent)
	{
		fprojfile->Write(VCXPROJPart4[2]);
		ffilterfile->Write(VCXFILTPart1[1]);
	}
	hasContent = false;
	bFirst = true;
	it = info->m_FileInfos.Begin();
	while (it)
	{
		if ((*it)->m_Type.Compare(CStringLiteral(_T("CSource")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			if (bFirst)
			{
				hasContent = true;
				fprojfile->Write(VCXPROJPart4[0]);
				ffilterfile->Write(VCXFILTPart1[0]);
				bHasSource = true;
				bFirst = false;
			}
			tmp = (*it)->m_Name;
			tmp.PrependString(_T("\\"));
			tmp.PrependString(info->m_Folder);
			tmp.PrependString(pParentDir);
			fprojfile->Write(VCXPROJPart4[3], tmp.GetString());
			ffilterfile->Write(VCXFILTPart1[2], tmp.GetString());
		}
		else if ((*it)->m_Type.Compare(CStringLiteral(_T("CppSource")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			if (bFirst)
			{
				hasContent = true;
				fprojfile->Write(VCXPROJPart4[0]);
				ffilterfile->Write(VCXFILTPart1[0]);
				bHasSource = true;
				bFirst = false;
			}
			tmp = (*it)->m_Name;
			tmp.PrependString(_T("\\"));
			tmp.PrependString(info->m_Folder);
			tmp.PrependString(pParentDir);
			fprojfile->Write(VCXPROJPart4[3], tmp.GetString());
			ffilterfile->Write(VCXFILTPart1[2], tmp.GetString());
		}
		++it;
	}
	if (hasContent)
	{
		fprojfile->Write(VCXPROJPart4[2]);
		ffilterfile->Write(VCXFILTPart1[1]);
	}
	hasContent = false;
	bFirst = true;
	it = info->m_FileInfos.Begin();
	while (it)
	{
		if ((*it)->m_Type.Compare(CStringLiteral(_T("MessageCompiler")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			if (bFirst)
			{
				hasContent = true;
				fprojfile->Write(VCXPROJPart4[0]);
				bFirst = false;
			}
			tmp = (*it)->m_Name;
			tmp.PrependString(_T("\\"));
			tmp.PrependString(info->m_Folder);
			tmp.PrependString(pParentDir);
			if (!(tmp.IsEmpty()))
				fprojfile->Write(VCXPROJPart4[6], tmp.GetString());
			if (!(info->m_Command.IsEmpty()))
				fprojfile->Write(VCXPROJPart4[8], info->m_Command.GetString(), info->m_Command.GetString(), info->m_Command.GetString(), info->m_Command.GetString());
			tmp = info->m_Outputs.Join(_T(";"));
			if (!(tmp.IsEmpty()))
				fprojfile->Write(VCXPROJPart4[9], tmp.GetString(), tmp.GetString(), tmp.GetString(), tmp.GetString());
			tmp = info->m_AdditionalInputs.Join(_T(";"));
			if (!(tmp.IsEmpty()))
				fprojfile->Write(VCXPROJPart4[10], tmp.GetString(), tmp.GetString(), tmp.GetString(), tmp.GetString());
			fprojfile->Write(VCXPROJPart4[7]);
		}
		++it;
	}
	if (hasContent)
		fprojfile->Write(VCXPROJPart4[2]);
	hasContent = false;
	bFirst = true;
	it = info->m_FileInfos.Begin();
	while (it)
	{
		if ((*it)->m_Type.Compare(CStringLiteral(_T("Image")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			if (bFirst)
			{
				hasContent = true;
				fprojfile->Write(VCXPROJPart4[0]);
				ffilterfile->Write(VCXFILTPart1[0]);
				bHasResource = true;
				bFirst = false;
			}
			tmp = (*it)->m_Name;
			tmp.PrependString(_T("\\"));
			tmp.PrependString(info->m_Folder);
			tmp.PrependString(pParentDir);
			fprojfile->Write(VCXPROJPart4[4], tmp.GetString());
			ffilterfile->Write(VCXFILTPart1[5], tmp.GetString());
		}
		++it;
	}
	if (hasContent)
	{
		fprojfile->Write(VCXPROJPart4[2]);
		ffilterfile->Write(VCXFILTPart1[1]);
	}
	hasContent = false;
	bFirst = true;
	it = info->m_FileInfos.Begin();
	while (it)
	{
		if ((*it)->m_Type.Compare(CStringLiteral(_T("ResourceCompiler")), 0, CStringLiteral::cIgnoreCase) == 0)
		{
			if (bFirst)
			{
				hasContent = true;
				fprojfile->Write(VCXPROJPart4[0]);
				ffilterfile->Write(VCXFILTPart1[0]);
				bHasResource = true;
				bFirst = false;
			}
			tmp = (*it)->m_Name;
			tmp.PrependString(_T("\\"));
			tmp.PrependString(info->m_Folder);
			tmp.PrependString(pParentDir);
			fprojfile->Write(VCXPROJPart4[5], tmp.GetString());
			ffilterfile->Write(VCXFILTPart1[4], tmp.GetString());
		}
		++it;
	}
	if (hasContent)
	{
		fprojfile->Write(VCXPROJPart4[2]);
		ffilterfile->Write(VCXFILTPart1[1]);
	}

	if (bHasHeader || bHasSource || bHasResource)
		ffilterfile->Write(VCXFILTEnde[0]);

	if (bHasHeader)
		ffilterfile->Write(VCXFILTEnde[2], create_uuid().GetString());
	if (bHasSource)
		ffilterfile->Write(VCXFILTEnde[3], create_uuid().GetString());
	if (bHasResource)
		ffilterfile->Write(VCXFILTEnde[1], create_uuid().GetString());

	if (bHasHeader || bHasSource || bHasResource)
		ffilterfile->Write(VCXFILTEnde[4]);

	ffilterfile->Write(VCXFILTEnde[5]);
	ffilterfile->Close();
	ffilterfile->release();

	fprojfile->Write(VCXPROJEnde);
	fprojfile->Close();
	fprojfile->release();
}

static void WriteProjectFiles(CConstPointer pVersion, ConstRef(CProjectInfos) infos)
{
	CFilePath fcurdir;
	CFilePath fname;
	CProjectInfos::Iterator it = infos.Begin();

	CDirectoryIterator::GetCurrentDirectory(fcurdir);
	while (it)
	{
		fname.set_Path((*it)->m_Folder);
		fname.MakeDirectory();
		if (CDirectoryIterator::DirectoryExists(fname) < 0)
			CDirectoryIterator::MakeDirectory(fname);
		CDirectoryIterator::SetCurrentDirectory(fname);
		WriteProjectFile(pVersion, *it);
		CDirectoryIterator::SetCurrentDirectory(fcurdir);
		++it;
	}
}

void MetaProjectCreateProjectFilesVS(CConstPointer pVersion)
{
	CFilePath fcurdir;
	CFilePath fname(__FILE__LINE__ _T("ProjectFiles.xml"));
	Ptr(CXMLContentHandlers) h = OK_NEW_OPERATOR CXMLContentHandlers();
	Ptr(CSAXParser) pParser = OK_NEW_OPERATOR CSAXParser;

	pParser->Create(h, _T("UTF-8"));
	pParser->Parse(fname);
	h->m_ProjectInfos.Finish();
	CDirectoryIterator::GetCurrentDirectory(fcurdir);
	fname.set_Path(__FILE__LINE__ pVersion);
	fname.MakeDirectory();
	if (CDirectoryIterator::DirectoryExists(fname) < 0)
		CDirectoryIterator::MakeDirectory(fname);
	CDirectoryIterator::SetCurrentDirectory(fname);
	WriteSolutionFile(h->m_ProjectsGuid, h->m_ProjectInfos);
	WriteProjectFiles(pVersion, h->m_ProjectInfos);
	CDirectoryIterator::SetCurrentDirectory(fcurdir);
	pParser->release();
	h->release();
}