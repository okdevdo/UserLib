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
 *  \file Configuration.h
 *  \brief Application's properties. There are various types of properties.
 */
#pragma once

#include "CppSources.h"
#include "DataBinaryTree.h"
#include "DataDoubleLinkedList.h"
#include "DataVector.h"
#include "FilePath.h"

/**
 *  \class CAbstractConfiguration
 *  \brief Base class of all configuration classes.
 */
class CPPSOURCES_API CAbstractConfiguration: public CCppObject
{
public:
	typedef CDataVectorT<CStringBuffer> Keys;
	typedef CDataVectorT<CStringBuffer> Values;

	/**
	 *  \brief Standard Constructor
	 *  
	 *  \details Standard Constructor. Initializes the configuration object.
	 */
	CAbstractConfiguration(void);
	/**
	 *  \brief Constructor
	 *  
	 *  \param [in] _prefix value, which makes the paths to the configuration objects unique.
	 *  \param [in] _iswritable flag, whether this configuration object is writeable.
	 *  
	 *  \details Constructor. Initializes the configuration object.
	 */
	CAbstractConfiguration(ConstRef(CStringLiteral) _prefix, WBool _iswritable = false);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor. Releases allocated memory.
	 */
	virtual ~CAbstractConfiguration(void);

	/**
	 *  \brief Getter flag writeable
	 *  
	 *  \return WBool
	 *  
	 *  \details Getter flag writeable.
	 */
	__inline WBool IsWritable() const { return m_writable; }
	/**
	 *  \brief Getter flag modified
	 *  
	 *  \return WBool
	 *  
	 *  \details Getter flag modified.
	 */
	__inline WBool IsModified() const { return m_modified; }
	/**
	 *  \brief Getter prefix
	 *  
	 *  \return ConstRef(CStringBuffer)
	 *  
	 *  \details Getter prefix.
	 */
	__inline ConstRef(CStringBuffer) GetPrefix() const { return m_prefix; }

	/**
	 *  \brief Test on the existence of a configuration path
	 *  
	 *  \param [in] _name path to be searched
	 *  \return WBool
	 *  
	 *  \details Test on the existence of a configuration path.
	 */
	WBool HasValue(ConstRef(CStringLiteral) _name);
	/**
	 *  \brief Test on the existence of a configuration path
	 *  
	 *  \param [in] _trailer path to be searched, but without the prefix, which is the application name
	 *  \return WBool
	 *  
	 *  \details Test on the existence of a configuration path. Tests first, whether the user property file in the user folder defines the value, tests then the application property file in the executable's folder.
	 */
	WBool HasUserValue(ConstRef(CStringLiteral) _trailer);
	/**
	 *  \brief Test on the existence of a configuration path
	 *  
	 *  \param [in] _trailer path to be searched, but without the prefix, which is the application name
	 *  \param [in] _defaultAppName default application name
	 *  \return WBool
	 *  
	 *  \details Test on the existence of a configuration path. Tests first, whether the user property file in the user folder defines the value, tests then the application property file in the executable's folder.
	 */
	WBool HasUserValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName);

	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _name path to be searched
	 *  \return CStringBuffer
	 *  
	 *  \details Getter Property Value. Searches for path _name, which must be fully prefixed, and returns the associated value, if any.
	 */
	CStringBuffer GetValue(ConstRef(CStringLiteral) _name);
	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [in] _default value to be returned, if path does not exist.
	 *  \return CStringBuffer
	 *  
	 *  \details Getter Property Value. Searches for path _name, which must be fully prefixed, and returns the associated value, if any, else returns the default value.
	 */	
	CStringBuffer GetValue(ConstRef(CStringLiteral) _name, ConstRef(CStringLiteral) _default);
	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _trailer path to be searched, but without the prefix, which is the application name
	 *  \return CStringBuffer
	 *  
	 *  \details Getter Property Value. Searches for path _trailer, which must not be fully prefixed, and returns the associated value, if any. Searches first the user property file in the user folder for the defined value, searches then the application property file in the executable's folder.
	 */
	CStringBuffer GetUserValue(ConstRef(CStringLiteral) _trailer);
	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _trailer path to be searched, but without the prefix, which is the application name
	 *  \param [in] _defaultAppName default application name
	 *  \return CStringBuffer
	 *  
	 *  \details Getter Property Value. Searches for path _trailer, which must not be fully prefixed, and returns the associated value, if any. Searches first the user property file in the user folder for the defined value, searches then the application property file in the executable's folder.
	 */
	CStringBuffer GetUserValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName);
	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _trailer path to be searched, but without the prefix, which is the application name
	 *  \param [in] _defaultAppName default application name
	 *  \param [in] _default value to be returned, if path does not exist.
	 *  \return CStringBuffer
	 *  
	 *  \details Getter Property Value. Searches for path _trailer, which must not be fully prefixed, and returns the associated value, if any, else returns the default value. Searches first the user property file in the user folder for the defined value, searches then the application property file in the executable's folder.
	 */
	CStringBuffer GetUserValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName, ConstRef(CStringLiteral) _default);
	/**
	 *  \brief Setter Property Value
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [in] _value value to be stored
	 *  \return void.
	 *  
	 *  \details Setter Property Value. Searches for path _name, which must be fully prefixed, and stores value _value.
	 */
	void SetValue(ConstRef(CStringLiteral) _name, ConstRef(CStringLiteral) _value);
	/**
	 *  \brief Setter Property Value
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [in] _value value to be stored
	 *  \return void.
	 *  
	 *  \details Setter Property Value. Searches for path _name, which must be fully prefixed, and stores value _value.
	 */
	void SetValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value);

	/**
	 *  \brief Getter Property Values
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [out] _values list of values to be retrieved
	 *  \return dword length of the returned list
	 *  
	 *  \details Getter Property Values. Searches for path _name, which must be fully prefixed, and returns the associated value list, if any.
	 */
	dword GetValues(ConstRef(CStringLiteral) _name, Ref(Values) _values);
	/**
	 *  \brief Getter Property Values
	 *  
	 *  \param [in] _trailer path to be searched, but without the prefix, which is the application name
	 *  \param [out] _values list of values to be retrieved
	 *  \return dword length of the returned list
	 *  
	 *  \details Getter Property Values. Searches for path _trailer, which must not be fully prefixed, and returns the associated value list, if any. Searches first the user property file in the user folder for the defined values, searches then the application property file in the executable's folder.
	 */
	dword GetUserValues(ConstRef(CStringLiteral) _trailer, Ref(Values) _values);
	/**
	 *  \brief Getter Property Values
	 *  
	 *  \param [in] _trailer path to be searched, but without the prefix, which is the application name
	 *  \param [in] _defaultAppName default application name
	 *  \param [out] _values list of values to be retrieved
	 *  \return dword length of the returned list
	 *  
	 *  \details Getter Property Values. Searches for path _trailer, which must not be fully prefixed, and returns the associated value list, if any. Searches first the user property file in the user folder for the defined values, searches then the application property file in the executable's folder.
	 */
	dword GetUserValues(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName, Ref(Values) _values);
	/**
	 *  \brief Setter Property Value
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [in] _values values to be stored
	 *  \return void.
	 *  
	 *  \details Setter Property Value. Searches for path _name, which must be fully prefixed, and stores the list of values _values.
	 */
	void SetValues(ConstRef(CStringLiteral) _name, ConstRef(Values) _values);

	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _name path to be searched
	 *  \return WBool
	 *  
	 *  \details Getter Property Value. Searches for path _name, which must be fully prefixed, and returns the associated value, if any.
	 */
	WBool GetBoolValue(ConstRef(CStringLiteral) _name);
	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [in] _default value to be returned, if path does not exist.
	 *  \return WBool
	 *  
	 *  \details Getter Property Value. Searches for path _name, which must be fully prefixed, and returns the associated value, if any, else returns the default value.
	 */	
	WBool GetBoolValue(ConstRef(CStringLiteral) _name, WBool _default);
	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _trailer path to be searched, but without the prefix, which is the application name
	 *  \return WBool
	 *  
	 *  \details Getter Property Value. Searches for path _trailer, which must not be fully prefixed, and returns the associated value, if any. Searches first the user property file in the user folder for the defined value, searches then the application property file in the executable's folder.
	 */
	WBool GetUserBoolValue(ConstRef(CStringLiteral) _trailer);
	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _trailer path to be searched, but without the prefix, which is the application name
	 *  \param [in] _defaultAppName default application name
	 *  \return WBool
	 *  
	 *  \details Getter Property Value. Searches for path _trailer, which must not be fully prefixed, and returns the associated value, if any. Searches first the user property file in the user folder for the defined value, searches then the application property file in the executable's folder.
	 */
	WBool GetUserBoolValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName);
	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _trailer path to be searched, but without the prefix, which is the application name
	 *  \param [in] _defaultAppName default application name
	 *  \param [in] _default value to be returned, if path does not exist.
	 *  \return WBool
	 *  
	 *  \details Getter Property Value. Searches for path _trailer, which must not be fully prefixed, and returns the associated value, if any, else returns the default value. Searches first the user property file in the user folder for the defined value, searches then the application property file in the executable's folder.
	 */
	WBool GetUserBoolValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName, WBool _default);
	/**
	 *  \brief Setter Property Value
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [in] _value value to be stored
	 *  \return void.
	 *  
	 *  \details Setter Property Value. Searches for path _name, which must be fully prefixed, and stores value _value.
	 */
	void SetBoolValue(ConstRef(CStringLiteral) _name, WBool _value);

	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _name path to be searched
	 *  \return WLong
	 *  
	 *  \details Getter Property Value. Searches for path _name, which must be fully prefixed, and returns the associated value, if any.
	 */
	WLong GetLongValue(ConstRef(CStringLiteral) _name);
	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [in] _default value to be returned, if path does not exist.
	 *  \return WLong
	 *  
	 *  \details Getter Property Value. Searches for path _name, which must be fully prefixed, and returns the associated value, if any, else returns the default value.
	 */	
	WLong GetLongValue(ConstRef(CStringLiteral) _name, WLong _default);
	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _trailer path to be searched, but without the prefix, which is the application name
	 *  \return WLong
	 *  
	 *  \details Getter Property Value. Searches for path _trailer, which must not be fully prefixed, and returns the associated value, if any. Searches first the user property file in the user folder for the defined value, searches then the application property file in the executable's folder.
	 */
	WLong GetUserLongValue(ConstRef(CStringLiteral) _trailer);
	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _trailer path to be searched, but without the prefix, which is the application name
	 *  \param [in] _defaultAppName default application name
	 *  \return WLong
	 *  
	 *  \details Getter Property Value. Searches for path _trailer, which must not be fully prefixed, and returns the associated value, if any. Searches first the user property file in the user folder for the defined value, searches then the application property file in the executable's folder.
	 */
	WLong GetUserLongValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName);
	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _trailer path to be searched, but without the prefix, which is the application name
	 *  \param [in] _defaultAppName default application name
	 *  \param [in] _default value to be returned, if path does not exist.
	 *  \return WLong
	 *  
	 *  \details Getter Property Value. Searches for path _trailer, which must not be fully prefixed, and returns the associated value, if any, else returns the default value. Searches first the user property file in the user folder for the defined value, searches then the application property file in the executable's folder.
	 */
	WLong GetUserLongValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName, WLong _default);
	/**
	 *  \brief Setter Property Value
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [in] _value value to be stored
	 *  \return void.
	 *  
	 *  \details Setter Property Value. Searches for path _name, which must be fully prefixed, and stores value _value.
	 */
	void SetLongValue(ConstRef(CStringLiteral) _name, WLong _value);

	/**
	 *  \brief Getter Property Values
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [in] _cnt number of WLong pointers, following in the variable parameter list
	 *  \return void
	 *  
	 *  \details Getter Property Values. Searches for path _name, which must be fully prefixed, and returns the associated value list, if any.
	 */
	void GetLongValues(ConstRef(CStringLiteral) _name, WLong _cnt, ...);
	/**
	 *  \brief Getter Property Values
	 *  
	 *  \param [in] _trailer path to be searched, but without the prefix, which is the application name
	 *  \param [in] _cnt number of WLong pointers, following in the variable parameter list
	 *  \return void
	 *  
	 *  \details Getter Property Values. Searches for path _trailer, which must not be fully prefixed, and returns the associated value list, if any. Searches first the user property file in the user folder for the defined values, searches then the application property file in the executable's folder.
	 */
	void GetUserLongValues(ConstRef(CStringLiteral) _trailer, WLong _cnt, ...);
	/**
	 *  \brief Getter Property Values
	 *  
	 *  \param [in] _trailer path to be searched, but without the prefix, which is the application name
	 *  \param [in] _defaultAppName default application name
	 *  \param [in] _cnt number of WLong pointers, following in the variable parameter list
	 *  \return void
	 *  
	 *  \details Getter Property Values. Searches for path _trailer, which must not be fully prefixed, and returns the associated value list, if any. Searches first the user property file in the user folder for the defined values, searches then the application property file in the executable's folder.
	 */
	void GetUserLongValues(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName, WLong _cnt, ...);
	/**
	 *  \brief Setter Property Value
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [in] _cnt number of WLong pointers, following in the variable parameter list
	 *  \return void.
	 *  
	 *  \details Setter Property Value. Searches for path _name, which must be fully prefixed, and stores the list of values.
	 */
	void SetLongValues(ConstRef(CStringLiteral) _name, WLong _cnt, ...);

	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _name path to be searched
	 *  \return WDouble
	 *  
	 *  \details Getter Property Value. Searches for path _name, which must be fully prefixed, and returns the associated value, if any.
	 */
	WDouble GetDoubleValue(ConstRef(CStringLiteral) _name);
	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [in] _default value to be returned, if path does not exist.
	 *  \return WDouble
	 *  
	 *  \details Getter Property Value. Searches for path _name, which must be fully prefixed, and returns the associated value, if any, else returns the default value.
	 */	
	WDouble GetDoubleValue(ConstRef(CStringLiteral) _name, WDouble _default);
	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _trailer path to be searched, but without the prefix, which is the application name
	 *  \return WDouble
	 *  
	 *  \details Getter Property Value. Searches for path _trailer, which must not be fully prefixed, and returns the associated value, if any. Searches first the user property file in the user folder for the defined value, searches then the application property file in the executable's folder.
	 */
	WDouble GetUserDoubleValue(ConstRef(CStringLiteral) _trailer);
	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _trailer path to be searched, but without the prefix, which is the application name
	 *  \param [in] _defaultAppName default application name
	 *  \return WDouble
	 *  
	 *  \details Getter Property Value. Searches for path _trailer, which must not be fully prefixed, and returns the associated value, if any. Searches first the user property file in the user folder for the defined value, searches then the application property file in the executable's folder.
	 */
	WDouble GetUserDoubleValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName);
	/**
	 *  \brief Getter Property Value
	 *  
	 *  \param [in] _trailer path to be searched, but without the prefix, which is the application name
	 *  \param [in] _defaultAppName default application name
	 *  \param [in] _default value to be returned, if path does not exist.
	 *  \return WDouble
	 *  
	 *  \details Getter Property Value. Searches for path _trailer, which must not be fully prefixed, and returns the associated value, if any, else returns the default value. Searches first the user property file in the user folder for the defined value, searches then the application property file in the executable's folder.
	 */
	WDouble GetUserDoubleValue(ConstRef(CStringLiteral) _trailer, ConstRef(CStringLiteral) _defaultAppName, WDouble _default);
	/**
	 *  \brief Setter Property Value
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [in] _value value to be stored
	 *  \return void.
	 *  
	 *  \details Setter Property Value. Searches for path _name, which must be fully prefixed, and stores value _value.
	 */
	void SetDoubleValue(ConstRef(CStringLiteral) _name, WDouble _value);

	/**
	 *  \brief virtual overload to enumerate all keys
	 *  
	 *  \param [in] _prefix path to be searched
	 *  \param [out] _keys enumerated keys
	 *  \return WBool true on Success
	 *  
	 *  \details Virtual overload to enumerate all keys.
	 */
	virtual WBool Enumerate(ConstRef(CStringLiteral) _prefix, Ref(Keys) _keys);
	/**
	 *  \brief virtual overload of a getter of raw property data
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [out] _value value to be retrieved
	 *  \return WBool true on Success
	 *  
	 *  \details Virtual overload of a getter of raw property data.
	 */
	virtual WBool GetRawValue(ConstRef(CStringLiteral) _name, Ref(CStringBuffer) _value);
	/**
	 *  \brief virtual overload of a setter of raw property data
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [in] _value value to be stored
	 *  \return WBool true on Success
	 *  
	 *  \details Virtual overload of a setter of raw property data.
	 */
	virtual WBool SetRawValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value);

protected:
	CStringBuffer m_prefix;
	WBool m_writable;
	WBool m_modified;

	WLong GetLongValues(ConstRef(CStringLiteral) _name, WLong _cnt, va_list argList);
};

/**
 *  \class CMapConfiguration
 *  \brief Stores configuration items in a binary tree.
 */
class CPPSOURCES_API CMapConfiguration: public CAbstractConfiguration
{
public:
	typedef struct tagMapItem
	{
		CStringBuffer _name;
		CStringBuffer _value;
	} MapItem;

	typedef CDataSAVLBinaryTreeT<MapItem> MapItems;

	/**
	 *  \brief Standard Constructor
	 *  
	 *  \details Standard Constructor. Initializes the configuration object.
	 */
	CMapConfiguration(DECL_FILE_LINE0);
	/**
	 *  \brief Constructor
	 *  
	 *  \param [in] _prefix value, which makes the paths to the configuration objects unique.
	 *  \param [in] _iswritable flag, whether this configuration object is writeable.
	 *  
	 *  \details Constructor. Initializes the configuration object.
	 */
	CMapConfiguration(DECL_FILE_LINE ConstRef(CStringLiteral) _prefix, WBool _iswritable = false);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor. Releases allocated memory.
	 */
	virtual ~CMapConfiguration(void);

	/**
	 *  \brief Number of stored property values
	 *  
	 *  \return TListCnt
	 *  
	 *  \details Number of stored property values.
	 */
	__inline TListCnt Count() const { return m_mapItems.Count(); }

	/**
	 *  \brief virtual overload to enumerate all keys
	 *  
	 *  \param [in] _prefix path to be searched
	 *  \param [out] _keys enumerated keys
	 *  \return WBool true on Success
	 *  
	 *  \details Virtual overload to enumerate all keys.
	 */
	virtual WBool Enumerate(ConstRef(CStringLiteral) _prefix, Ref(Keys) _keys);
	/**
	 *  \brief virtual overload of a getter of raw property data
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [out] _value value to be retrieved
	 *  \return WBool true on Success
	 *  
	 *  \details Virtual overload of a getter of raw property data.
	 */
	virtual WBool GetRawValue(ConstRef(CStringLiteral) _name, Ref(CStringBuffer) _value);
	/**
	 *  \brief virtual overload of a setter of raw property data
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [in] _value value to be stored
	 *  \return WBool true on Success
	 *  
	 *  \details Virtual overload of a setter of raw property data.
	 */
	virtual WBool SetRawValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value);

protected:
	WBool AddValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value);

	MapItems m_mapItems;
};

/**
 *  \class CFileConfiguration
 *  \brief Based on CMapConfiguration. Fills the tree from a file.
 */
class CPPSOURCES_API CFileConfiguration: public CMapConfiguration
{
public:
	/**
	 *  \brief Standard Constructor
	 *  
	 *  \details Standard Constructor. Initializes the configuration object.
	 */
	CFileConfiguration(DECL_FILE_LINE0);
	/**
	 *  \brief Constructor
	 *  
	 *  \param [in] _path file path.
	 *  \param [in] bWritable flag, whether this configuration object is writeable.
	 *  
	 *  \details Constructor. Initializes the configuration object.
	 */
	CFileConfiguration(DECL_FILE_LINE ConstRef(CFilePath) _path, WBool bWritable = false);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor. Releases allocated memory.
	 */
	virtual ~CFileConfiguration(void);

	/**
	 *  \brief Getter stored file path
	 *  
	 *  \return ConstRef(CFilePath)
	 *  
	 *  \details Getter stored file path.
	 */
	__inline ConstRef(CFilePath) get_path() const { return m_path; }

protected:
	WBool LoadConfig();
	WBool SaveConfig();

	CFilePath m_path;
};

/**
 *  \class CSystemConfiguration
 *  \brief Implements access to the environment of the program.
 */
class CPPSOURCES_API CSystemConfiguration: public CAbstractConfiguration
{
public:
	/**
	 *  \brief Standard Constructor
	 *  
	 *  \details Standard Constructor. Initializes the configuration object.
	 */
	CSystemConfiguration(void);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor. Releases allocated memory.
	 */
	virtual ~CSystemConfiguration(void);

	/**
	 *  \brief virtual overload to enumerate all keys
	 *  
	 *  \param [in] _prefix path to be searched
	 *  \param [out] _keys enumerated keys
	 *  \return WBool true on Success
	 *  
	 *  \details Virtual overload to enumerate all keys.
	 */
	virtual WBool Enumerate(ConstRef(CStringLiteral) _prefix, Ref(Keys) _keys);
	/**
	 *  \brief virtual overload of a getter of raw property data
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [out] _value value to be retrieved
	 *  \return WBool true on Success
	 *  
	 *  \details Virtual overload of a getter of raw property data.
	 */
	virtual WBool GetRawValue(ConstRef(CStringLiteral) _name, Ref(CStringBuffer) _value);
	/**
	 *  \brief virtual overload of a setter of raw property data
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [in] _value value to be stored
	 *  \return WBool true on Success
	 *  
	 *  \details Virtual overload of a setter of raw property data.
	 */
	virtual WBool SetRawValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value);
};

/**
 *  \class CApplicationConfiguration
 *  \brief Implements access to application infos.
 */
class CPPSOURCES_API CApplicationConfiguration: public CAbstractConfiguration
{
public:
	/**
	 *  \brief Standard Constructor
	 *  
	 *  \details Standard Constructor. Initializes the configuration object.
	 */
	CApplicationConfiguration(void);
	/**
	 *  \brief Constructor
	 *  
	 *  \param [in] _path argv[0] from executable's main
	 *  
	 *  \details Constructor. Initializes the configuration object.
	 */
	CApplicationConfiguration(CConstPointer _path);
	/**
	 *  \brief Constructor
	 *  
	 *  \param [in] _path argv[0] from executable's main
	 *  
	 *  \details Constructor. Initializes the configuration object.
	 */
	CApplicationConfiguration(ConstRef(CFilePath) _path);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Destructor. Releases allocated memory.
	 */
	virtual ~CApplicationConfiguration(void);

	/**
	 *  \brief virtual overload to enumerate all keys
	 *  
	 *  \param [in] _prefix path to be searched
	 *  \param [out] _keys enumerated keys
	 *  \return WBool true on Success
	 *  
	 *  \details Virtual overload to enumerate all keys.
	 */
	virtual WBool Enumerate(ConstRef(CStringLiteral) _prefix, Ref(Keys) _keys);
	/**
	 *  \brief virtual overload of a getter of raw property data
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [out] _value value to be retrieved
	 *  \return WBool true on Success
	 *  
	 *  \details Virtual overload of a getter of raw property data.
	 */
	virtual WBool GetRawValue(ConstRef(CStringLiteral) _name, Ref(CStringBuffer) _value);
	/**
	 *  \brief virtual overload of a setter of raw property data
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [in] _value value to be stored
	 *  \return WBool true on Success
	 *  
	 *  \details Virtual overload of a setter of raw property data.
	 */
	virtual WBool SetRawValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value);

protected:
	CFilePath m_path;
};

/**
 *  \class CConfigurationList
 *  \brief List of configuration objects, but with a configuration object interface, thus traversing the list to find a value.
 */
class CPPSOURCES_API CConfigurationList: public CAbstractConfiguration
{
public:
	typedef CDataDoubleLinkedListT<CAbstractConfiguration> ConfigList;
	typedef ConfigList::Iterator Iterator;

	CConfigurationList(DECL_FILE_LINE0);
	virtual ~CConfigurationList(void);

	/**
	 *  \brief Append a configuration object to the list of configuration objects
	 *  
	 *  \param [in] pConfig configuration object to be added
	 *  \return void.
	 *  
	 *  \details Append a configuration object to the list of configuration objects.
	 */
	__inline void Add(CAbstractConfiguration* pConfig) { m_configList.Append(pConfig); }
	/**
	 *  \brief Getter Iterator
	 *  
	 *  \return Iterator, head of list of configuration objects
	 *  
	 *  \details Getter Iterator. Used to traverse the list.
	 */
	__inline Iterator Begin() { return m_configList.Begin(); }
	/**
	 *  \brief Remove configuration object
	 *  
	 *  \param [in] it Iterator, pointing to the item to removed
	 *  \return void.
	 *  
	 *  \details Remove configuration object. The list of configuration objects is a doubly linked list, so it is safe to iterate the list and remove items.
	 */
	void Remove(Iterator it);

	/**
	 *  \brief virtual overload to enumerate all keys
	 *  
	 *  \param [in] _prefix path to be searched
	 *  \param [out] _keys enumerated keys
	 *  \return WBool true on Success
	 *  
	 *  \details Virtual overload to enumerate all keys.
	 */
	virtual WBool Enumerate(ConstRef(CStringLiteral) _prefix, Ref(Keys) _keys);
	/**
	 *  \brief virtual overload of a getter of raw property data
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [out] _value value to be retrieved
	 *  \return WBool true on Success
	 *  
	 *  \details Virtual overload of a getter of raw property data.
	 */
	virtual WBool GetRawValue(ConstRef(CStringLiteral) _name, Ref(CStringBuffer) _value);
	/**
	 *  \brief virtual overload of a setter of raw property data
	 *  
	 *  \param [in] _name path to be searched
	 *  \param [in] _value value to be stored
	 *  \return WBool true on Success
	 *  
	 *  \details Virtual overload of a setter of raw property data.
	 */
	virtual WBool SetRawValue(ConstRef(CStringLiteral) _name, ConstRef(CStringBuffer) _value);

protected:
	ConfigList m_configList;
};


