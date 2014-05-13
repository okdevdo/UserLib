/******************************************************************************

This file is part of XSetup, which is part of UserLib.

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
		tmp = theApp->config()->GetUserValue(tmp, APPLICATION_NAME);

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
		tmp = theApp->config()->GetUserValue(tmp, APPLICATION_NAME);

		if (!(tmp.IsEmpty()))
			info.m_Culture = tmp;

		tmp.SetString(__FILE__LINE__ _T("MetaProject.Command."));
		tmp.AppendString(*vIt);
		tmp = theApp->config()->GetUserValue(tmp, APPLICATION_NAME);

		if (!(tmp.IsEmpty()))
			info.m_Command = tmp;

		tmp.SetString(__FILE__LINE__ _T("MetaProject.Outputs."));
		tmp.AppendString(*vIt);
		tmp = theApp->config()->GetUserValue(tmp, APPLICATION_NAME);

		if (!(tmp.IsEmpty()))
			info.m_OutPuts = tmp;

		tmp.SetString(__FILE__LINE__ _T("MetaProject.AdditionalInputs."));
		tmp.AppendString(*vIt);
		tmp = theApp->config()->GetUserValue(tmp, APPLICATION_NAME);

		if (!(tmp.IsEmpty()))
			info.m_AdditionalInputs = tmp;

		tmp.SetString(__FILE__LINE__ _T("MetaProject.PreprocessorDefinitions."));
		tmp.AppendString(*vIt);
		tmp = theApp->config()->GetUserValue(tmp, APPLICATION_NAME);

		if (!(tmp.IsEmpty()))
			info.m_PreprocessorDefinitions = tmp;

		tmp.SetString(__FILE__LINE__ _T("MetaProject.PrecompiledHeaderFile."));
		tmp.AppendString(*vIt);
		tmp = theApp->config()->GetUserValue(tmp, APPLICATION_NAME);

		if (!(tmp.IsEmpty()))
			info.m_PrecompiledHeaderFile = tmp;

		tmp.SetString(__FILE__LINE__ _T("MetaProject.AdditionalIncludeDirectories."));
		tmp.AppendString(*vIt);
		tmp = theApp->config()->GetUserValue(tmp, APPLICATION_NAME);

		if (!(tmp.IsEmpty()))
			info.m_AdditionalIncludeDirectories = tmp;

		tmp.SetString(__FILE__LINE__ _T("MetaProject.AdditionalDependencies."));
		tmp.AppendString(*vIt);
		tmp = theApp->config()->GetUserValue(tmp, APPLICATION_NAME);

		if (!(tmp.IsEmpty()))
			info.m_AdditionalDependencies = tmp;

		ProjInfo.insert(info);
		++vIt;
	}
}

void MetaProjectCreateProjectFilesXML(CConstPointer xml_file)
{
	CFilePath fname(__FILE__LINE__ xml_file);
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

	if (!(theApp->config()->HasUserValue(_T("MetaProject.Libraries"), APPLICATION_NAME)))
	{
		CERR << _T("Configuration 'MetaProject.Libraries' not found.") << endl;
		return;
	}
	theApp->config()->GetUserValues(_T("MetaProject.Libraries"), APPLICATION_NAME, Libs);
	Libs.Sort();
	InitDependencies(ProjDep, Libs);
	InitInfos(ProjInfo, Libs);
	if (!(theApp->config()->HasUserValue(_T("MetaProject.NoEntryLibraries"), APPLICATION_NAME)))
	{
		CERR << _T("Configuration 'MetaProject.NoEntryLibraries' not found.") << endl;
		return;
	}
	theApp->config()->GetUserValues(_T("MetaProject.NoEntryLibraries"), APPLICATION_NAME, NoEntryLibs);
	NoEntryLibs.Sort();
	InitDependencies(ProjDep, NoEntryLibs);
	InitInfos(ProjInfo, NoEntryLibs);
	if (!(theApp->config()->HasUserValue(_T("MetaProject.GuiExecutable"), APPLICATION_NAME)))
	{
		CERR << _T("Configuration 'MetaProject.GuiExecutable' not found.") << endl;
		return;
	}
	theApp->config()->GetUserValues(_T("MetaProject.GuiExecutable"), APPLICATION_NAME, GuiExe);
	GuiExe.Sort();
	InitDependencies(ProjDep, GuiExe);
	InitInfos(ProjInfo, GuiExe);
	if (!(theApp->config()->HasUserValue(_T("MetaProject.ConsoleExecutable"), APPLICATION_NAME)))
	{
		CERR << _T("Configuration 'MetaProject.ConsoleExecutable' not found.") << endl;
		return;
	}
	theApp->config()->GetUserValues(_T("MetaProject.ConsoleExecutable"), APPLICATION_NAME, ConsoleExe);
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
