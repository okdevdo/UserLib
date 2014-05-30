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
 *  \file Option.h
 *  \brief Command line argument handling. Each possible argument is described as an option object.
 */
#pragma once

#include "CppSources.h"
#include "BaseException.h"

/**
 *  \class CAbstractOptionCallback
 *  \brief Command line argument handling. The callback will be called when the corresponding command line argument was found.
 */
class CPPSOURCES_API CAbstractOptionCallback: public CCppObject
{
public:
	virtual ~CAbstractOptionCallback();
	
	/**
	 *  \brief Option callback, called to handle the corresponding command line argument occurrence.
	 *  
	 *  \param [in] name 'long name' of the option.
	 *  \param [in] value value, if any, given on the command line with that option.
	 *  \return void.
	 *  
	 *  \details This method will be called to handle the command line argument found. This method is pure virtual and must be overwritten.
	 */
	virtual void invoke(const CStringLiteral& name, const CStringLiteral& value) const = 0;	
	/**
	 *  \brief Create a clone of the callback
	 *  
	 *  \return Ptr(CAbstractOptionCallback)
	 *  
	 *  \details Create a clone of the callback. This method is pure virtual and must be overwritten.
	 */
	virtual CAbstractOptionCallback* clone() const = 0;

protected:
	CAbstractOptionCallback();
	CAbstractOptionCallback(const CAbstractOptionCallback&);
};


/**
 *  \class COptionCallback
 *  \brief Command line argument handling. The callback will be called when the corresponding command line argument was found.
 */
template <class C>
class COptionCallback: public CAbstractOptionCallback
{
public:
	typedef void (C::*Callback)(const CStringLiteral& name, const CStringLiteral& value);
	
	/**
	 *  \brief Constructor
	 *  
	 *  \param [in] pObject object containing the callback
	 *  \param [in] method callback
	 *  
	 *  \details Constructor.
	 */
	COptionCallback(C* pObject, Callback method):
		_pObject(pObject),
		_method(method)
	{
	}
	/**
	 *  \brief Copy constructor
	 *  
	 *  \param [in] cb object to be copied
	 *  
	 *  \details Copy constructor.
	 */
	COptionCallback(const COptionCallback& cb):
		CAbstractOptionCallback(cb),
		_pObject(cb._pObject),
		_method(cb._method)
	{
	}
	
	virtual ~COptionCallback()
	{
	}
	
	/**
	 *  \brief Assignment operator
	 *  
	 *  \param [in] cb object to be copied
	 *  \return Ref(COptionCallback) *this.
	 *  
	 *  \details Assignment operator.
	 */
	COptionCallback& operator = (const COptionCallback& cb)
	{
		if (&cb != this)
		{
			this->_pObject = cb._pObject;
			this->_method  = cb._method;
		}
		return *this;
	}
	
	/**
	 *  \brief Option callback, called to handle the corresponding command line argument occurrence.
	 *  
	 *  \param [in] name 'long name' of the option.
	 *  \param [in] value value, if any, given on the command line with that option.
	 *  \return void.
	 *  
	 *  \details This method will be called to handle the command line argument found. This method is pure virtual in the base class and has been overwritten.
	 */
	void invoke(const CStringLiteral& name, const CStringLiteral& value) const
	{
		(_pObject->*_method)(name, value);
	}
	
	/**
	 *  \brief Create a clone of the callback
	 *  
	 *  \return Ptr(CAbstractOptionCallback)
	 *  
	 *  \details Create a clone of the callback. This method is pure virtual in the base class and has been overwritten.
	 */
	CAbstractOptionCallback* clone() const
	{
		return OK_NEW_OPERATOR COptionCallback(_pObject, _method);
	}
	
private:
	COptionCallback();
	
	C* _pObject;
	Callback _method;
};

/**
 *  \class COption
 *  \brief Command line argument handling. This class defines a possible command line argument.
 */
class CPPSOURCES_API COption: public CCppObject
{
public:
	/**
	 *  \brief Constructor
	 *  
	 *  \param [in] _fullname full name of the option
	 *  
	 *  \details Constructor.
	 */
	COption(CStringLiteral _fullname);
	/**
	 *  \brief Constructor
	 *  
	 *  \param [in] _fullname full name of the option
	 *  \param [in] _shortname the abbreviated, short name of the option
	 *  
	 *  \details Constructor.
	 */
	COption(CStringLiteral _fullname, CStringLiteral _shortname);
	/**
	 *  \brief Constructor
	 *  
	 *  \param [in] _fullname full name of the option
	 *  \param [in] _shortname the abbreviated, short name of the option
	 *  \param [in] _category category of the option, usage will sorted in respect of the category
	 *  \param [in] _description description of the option
	 *  \param [in] _required if this flag is true, the option is required and can not be missing on the command line.
	 *  
	 *  \details Constructor.
	 */
	COption(CStringLiteral _fullname, CStringLiteral _shortname, CStringLiteral _category, CStringLiteral _description, WBool _required = false);
	/**
	 *  \brief Copy Constructor
	 *  
	 *  \param [in] _copy the object to be copied
	 *  
	 *  \details Copy Constructor.
	 */
	COption(ConstRef(COption) _copy);
	/**
	 *  \brief Destructor
	 *  
	 *  \details Releasing allocated memory.
	 */
	virtual ~COption(void);
	
	/**
	 *  \brief Setter short name
	 *  
	 *  \param [in] _shortname short name of the option
	 *  \return Ref(COption) *this.
	 *  
	 *  \details Setter short name of the option. Short names are an alternate name to be used on the command line.
	 */
	Ref(COption) shortName(CStringLiteral _shortname);
	/**
	 *  \brief Setter category
	 *  
	 *  \param [in] _category category of the option
	 *  \return Ref(COption) *this.
	 *  
	 *  \details Setter category of the option. In the usage the options will be sorted in respect to the category.
	 */
	Ref(COption) category(CStringLiteral _category);
	/**
	 *  \brief Setter description
	 *  
	 *  \param [in] _description description of the option
	 *  \return Ref(COption) *this.
	 *  
	 *  \details Setter description of the option. In the usage this description will be given to the user.
	 */
	Ref(COption) description(CStringLiteral _description);
	/**
	 *  \brief Setter required flag
	 *  
	 *  \param [in] _required whether the option is required on the command line
	 *  \return Ref(COption) *this.
	 *  
	 *  \details Setter required flag of the option.
	 */
	Ref(COption) required(WBool _required);
	/**
	 *  \brief Setter repeatable flag
	 *  
	 *  \param [in] _repeatable whether the option can be repeated
	 *  \return Ref(COption) *this.
	 *  
	 *  \details Setter repeatable flag of the option. This flag is not really useful without setting the argument of the option.
	 */
	Ref(COption) repeatable(WBool _repeatable);
	/**
	 *  \brief Setter Argument
	 *  
	 *  \param [in] _name name of the argument
	 *  \param [in] _required whether the argument is required
	 *  \param [in] _position position of the argument (takes 0 for no position)
	 *  \return Ref(COption) *this.
	 *  
	 *  \details Setter Argument of the option. The position of the argument, if defined, has the following definition: If the full or short name has been omitted, but the argument has been placed in the defined position on the command line, the defined option will be recognized and handled.
	 */
	Ref(COption) argument(CStringLiteral _name, WBool _required = true, WInt _position = 0);
	/**
	 *  \brief Setter noArgument
	 *  
	 *  \return Ref(COption) *this.
	 *  
	 *  \details Explicit setter of not defining an argument.
	 */
	Ref(COption) noArgument();
	/**
	 *  \brief Setter group
	 *  
	 *  \param [in] _group the group of the option
	 *  \return Ref(COption) *this.
	 *  
	 *  \details Setter group of the option. Only one option of a group can be given on the command line.
	 */
	Ref(COption) group(CStringLiteral _group);
	/**
	 *  \brief Setter callback
	 *  
	 *  \param [in] _callback callback object
	 *  \return Ref(COption) *this.
	 *  
	 *  \details Setter callback of the option. If the option has been found on the command line, this callback will be called instead of the function handleOption of the application object.
	 */
	Ref(COption) callBack(CAbstractOptionCallback& _callback);

	/**
	 *  \brief Getter full name
	 *  
	 *  \return ConstRef(CStringBuffer)
	 *  
	 *  \details Getter full name of the option.
	 */
	ConstRef(CStringBuffer) fullname() const;
	ConstRef(CStringBuffer) get_Name() const;
	/**
	 *  \brief Getter short name
	 *  
	 *  \return ConstRef(CStringBuffer)
	 *  
	 *  \details Getter short name of the option.
	 */
	ConstRef(CStringBuffer) shortName() const;
	/**
	 *  \brief Getter category
	 *  
	 *  \return ConstRef(CStringBuffer)
	 *  
	 *  \details Getter category of the option.
	 */
	ConstRef(CStringBuffer) category() const;
	/**
	 *  \brief Getter description
	 *  
	 *  \return ConstRef(CStringBuffer)
	 *  
	 *  \details Getter description of the option.
	 */
	ConstRef(CStringBuffer) description() const;
	/**
	 *  \brief Getter flag required
	 *  
	 *  \return WBool
	 *  
	 *  \details Getter flag required of the option.
	 */
	WBool required() const;
	/**
	 *  \brief Getter flag repeatable
	 *  
	 *  \return WBool
	 *  
	 *  \details Getter flag repeatable of the option.
	 */
	WBool repeatable() const;
	/**
	 *  \brief Getter flag hasArgument
	 *  
	 *  \return WBool
	 *  
	 *  \details Getter flag hasArgument of the option.
	 */
	WBool hasArgument() const;
	/**
	 *  \brief Getter argument
	 *  
	 *  \return ConstRef(CStringBuffer)
	 *  
	 *  \details Getter argument of the option. This value is only valid if hasArgument is true.
	 */
	ConstRef(CStringBuffer) argument() const;
	/**
	 *  \brief Getter flag argumentRequired
	 *  
	 *  \return WBool
	 *  
	 *  \details Getter flag argumentRequired of the option. This value is only valid if hasArgument is true.
	 */
	WBool argumentRequired() const;
	/**
	 *  \brief Getter argumentPosition
	 *  
	 *  \return WInt
	 *  
	 *  \details Getter argumentPosition of the option. This value is only valid if hasArgument is true.
	 */
	WInt argumentPosition() const;
	/**
	 *  \brief Getter group
	 *  
	 *  \return ConstRef(CStringBuffer)
	 *  
	 *  \details Getter group of the option. Only one option of the group can be given on the command line.
	 */
	ConstRef(CStringBuffer) group() const;
	/**
	 *  \brief Getter callBack
	 *  
	 *  \return ConstPtr(CAbstractOptionCallback)
	 *  
	 *  \details Getter callBack of the option.
	 */
	CAbstractOptionCallback const* callBack() const;

	/**
	 *  \brief test, whether this option has the given short name
	 *  
	 *  \param [in] option the test value
	 *  \return WBool
	 *  
	 *  \details test, whether this option has the given short name
	 */
	WBool matchesShort(CStringLiteral option);
	/**
	 *  \brief test, whether this option has the given full name
	 *  
	 *  \param [in] option the test value
	 *  \return WBool
	 *  
	 *  \details test, whether this option has the given full name
	 */
	WBool matchesFull(CStringLiteral option);
	/**
	 *  \brief test, whether this option has the given partial, full name
	 *  
	 *  \param [in] option the test value
	 *  \return WBool
	 *  
	 *  \details test, whether this option has the given partial, full name
	 */
	WBool matchesPartial(CStringLiteral option);

	/**
	 *  \brief get a copy of this option
	 *  
	 *  \return Ptr(COption)
	 *  
	 *  \details get a copy of this option
	 */
	COption* clone();

private:
	COption(void);

	CStringBuffer m_fullname;
	CStringBuffer m_shortname;
	CStringBuffer m_category;
	CStringBuffer m_description;
	WBool m_required;
	WBool m_repeatable;
	CStringBuffer m_argname;
	WBool m_argrequired;
	WInt m_argposition;
	CStringBuffer m_group;
	CAbstractOptionCallback* m_pCallback;
};

DECL_EXCEPTION(CPPSOURCES_API, COptionException, CBaseException)
DECL_EXCEPTION(CPPSOURCES_API, COptionMissingOptionException, COptionException)
DECL_EXCEPTION(CPPSOURCES_API, COptionMissingArgumentException, COptionException)
DECL_EXCEPTION(CPPSOURCES_API, COptionUnexpectedArgumentException, COptionException)
DECL_EXCEPTION(CPPSOURCES_API, COptionIllegalArgumentException, COptionException)
DECL_EXCEPTION(CPPSOURCES_API, COptionUnknownException, COptionException)
DECL_EXCEPTION(CPPSOURCES_API, COptionAmbiguityException, COptionException)
DECL_EXCEPTION(CPPSOURCES_API, COptionNotRepeatableException, COptionException)
DECL_EXCEPTION(CPPSOURCES_API, COptionRequiredMissingException, COptionException)
DECL_EXCEPTION(CPPSOURCES_API, COptionGroupException, COptionException)

