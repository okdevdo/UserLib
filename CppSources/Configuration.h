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
#pragma once

#include "CppSources.h"
#include "DataBinaryTree.h"
#include "DataDoubleLinkedList.h"
#include "DataVector.h"
#include "FilePath.h"

class CPPSOURCES_API CAbstractConfiguration: public CCppObject
{
public:
	typedef CDataVectorT<CStringBuffer> Keys;
	typedef CDataVectorT<CStringBuffer> Values;

	CAbstractConfiguration(void);
	CAbstractConfiguration(ConstRef(CStringLiteral) _prefix, WBool _iswritable = false);
	virtual ~CAbstractConfiguration(void);

	__inline WBool IsWritable() const { return m_writable; }
	__inline WBool IsModified() const { return m_modified; }
	__inline ConstRef(CStringBuffer) GetPrefix() const { return m_prefix; }

	WBool HasValue(ConstRef(CStringLiteral) _name);
	WBool HasUserValue(ConstRef(CStringLiteral) _trailer);
	WBool HasUserValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName);

	CStringBuffer GetValue(ConstRef(CStringLiteral) _name);
	CStringBuffer GetValue(ConstRef(CStringLiteral) _name, ConstRef(CStringLiteral) _default);
	CStringBuffer GetUserValue(ConstRef(CStringLiteral) _trailer);
	CStringBuffer GetUserValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName);
	CStringBuffer GetUserValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName, ConstRef(CStringLiteral) _default);
	void SetValue(ConstRef(CStringLiteral) _name, ConstRef(CStringLiteral) _value);
	void SetValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value);

	dword GetValues(ConstRef(CStringLiteral) _name, Ref(Values) _values);
	dword GetUserValues(ConstRef(CStringLiteral) _trailer, Ref(Values) _values);
	dword GetUserValues(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName, Ref(Values) _values);
	void SetValues(ConstRef(CStringLiteral) _name, ConstRef(Values) _values);

	WBool GetBoolValue(ConstRef(CStringLiteral) _name);
	WBool GetBoolValue(ConstRef(CStringLiteral) _name, WBool _default);
	WBool GetUserBoolValue(ConstRef(CStringLiteral) _trailer);
	WBool GetUserBoolValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName);
	WBool GetUserBoolValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName, WBool _default);
	void SetBoolValue(ConstRef(CStringLiteral) _name, WBool _value);

	WLong GetLongValue(ConstRef(CStringLiteral) _name);
	WLong GetLongValue(ConstRef(CStringLiteral) _name, WLong _default);
	WLong GetUserLongValue(ConstRef(CStringLiteral) _trailer);
	WLong GetUserLongValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName);
	WLong GetUserLongValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName, WLong _default);
	void SetLongValue(ConstRef(CStringLiteral) _name, WLong _value);

	void GetLongValues(ConstRef(CStringLiteral) _name, WLong _cnt, ...);
	void GetUserLongValues(ConstRef(CStringLiteral) _trailer, WLong _cnt, ...);
	void GetUserLongValues(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName, WLong _cnt, ...);
	void SetLongValues(ConstRef(CStringLiteral) _name, WLong _cnt, ...);

	WDouble GetDoubleValue(ConstRef(CStringLiteral) _name);
	WDouble GetDoubleValue(ConstRef(CStringLiteral) _name, WDouble _default);
	WDouble GetUserDoubleValue(ConstRef(CStringLiteral) _trailer);
	WDouble GetUserDoubleValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName);
	WDouble GetUserDoubleValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName, WDouble _default);
	void SetDoubleValue(ConstRef(CStringLiteral) _name, WDouble _value);

	virtual WBool Enumerate(ConstRef(CStringLiteral) _prefix, Ref(Keys) _keys);
	virtual WBool GetRawValue(ConstRef(CStringLiteral) _name, Ref(CStringBuffer) _value);
	virtual WBool SetRawValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value);

protected:
	CStringBuffer m_prefix;
	WBool m_writable;
	WBool m_modified;

	WLong GetLongValues(ConstRef(CStringLiteral) _name, WLong _cnt, va_list argList);
};

class CPPSOURCES_API CMapConfiguration: public CAbstractConfiguration
{
public:
	typedef struct tagMapItem
	{
		CStringBuffer _name;
		CStringBuffer _value;
	} MapItem;

	typedef CDataSAVLBinaryTreeT<MapItem> MapItems;

	CMapConfiguration(DECL_FILE_LINE0);
	CMapConfiguration(DECL_FILE_LINE ConstRef(CStringLiteral) _prefix, WBool _iswritable = false);
	virtual ~CMapConfiguration(void);

	__inline TListCnt Count() const { return m_mapItems.Count(); }

	virtual WBool Enumerate(ConstRef(CStringLiteral) _prefix, Ref(Keys) _keys);
	virtual WBool GetRawValue(ConstRef(CStringLiteral) _name, Ref(CStringBuffer) _value);
	virtual WBool SetRawValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value);

protected:
	WBool AddValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value);

	MapItems m_mapItems;
};

class CPPSOURCES_API CFileConfiguration: public CMapConfiguration
{
public:
	CFileConfiguration(DECL_FILE_LINE0);
	CFileConfiguration(DECL_FILE_LINE ConstRef(CFilePath) _path, WBool bWritable = false);
	virtual ~CFileConfiguration(void);

	__inline ConstRef(CFilePath) get_path() const { return m_path; }

protected:
	WBool LoadConfig();
	WBool SaveConfig();

	CFilePath m_path;
};

class CPPSOURCES_API CSystemConfiguration: public CAbstractConfiguration
{
public:
	CSystemConfiguration(void);
	virtual ~CSystemConfiguration(void);

	virtual WBool Enumerate(ConstRef(CStringLiteral) _prefix, Ref(Keys) _keys);
	virtual WBool GetRawValue(ConstRef(CStringLiteral) _name, Ref(CStringBuffer) _value);
	virtual WBool SetRawValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value);
};

class CPPSOURCES_API CApplicationConfiguration: public CAbstractConfiguration
{
public:
	CApplicationConfiguration(void);
	CApplicationConfiguration(CConstPointer _path);
	CApplicationConfiguration(ConstRef(CFilePath) _path);
	virtual ~CApplicationConfiguration(void);

	virtual WBool Enumerate(ConstRef(CStringLiteral) _prefix, Ref(Keys) _keys);
	virtual WBool GetRawValue(ConstRef(CStringLiteral) _name, Ref(CStringBuffer) _value);
	virtual WBool SetRawValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value);

protected:
	CFilePath m_path;
};

class CPPSOURCES_API CConfigurationList: public CAbstractConfiguration
{
public:
	typedef CDataDoubleLinkedListT<CAbstractConfiguration> ConfigList;
	typedef ConfigList::Iterator Iterator;

	CConfigurationList(DECL_FILE_LINE0);
	virtual ~CConfigurationList(void);

	__inline void Add(CAbstractConfiguration* pConfig) { m_configList.Append(pConfig); }
	__inline Iterator Begin() { return m_configList.Begin(); }
	void Remove(Iterator it);

	virtual WBool Enumerate(ConstRef(CStringLiteral) _prefix, Ref(Keys) _keys);
	virtual WBool GetRawValue(ConstRef(CStringLiteral) _name, Ref(CStringBuffer) _value);
	virtual WBool SetRawValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value);

protected:
	ConfigList m_configList;
};


